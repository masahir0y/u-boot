#!/usr/bin/env python
#
# Author: Masahiro Yamada <yamada.m@jp.panasonic.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

"""
Move the specified config option to Kconfig
"""

import fnmatch
import multiprocessing
import optparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

SHOW_GNU_MAKE = 'scripts/show-gnu-make'
SLEEP_TIME=0.03

STATE_IDLE = 0
STATE_DEFCONFIG = 1
STATE_SAVEDEFCONFIG = 2

FIXUP_TABLE = (
    # field, value, CONFIG, position
    ('board', 'tb100', 'TARGET_TB100', 'ARC'),
    ('defconfig', 'integratorap_cm720t_defconfig', 'TARGET_INTEGRATORAP_CM720T', 'ARM'),
    ('board', 'enbw_cmc', 'TARGET_ENBW_CMC', 'ARCH_DAVINCI'),
    ('board', 'smdkv310', 'TARGET_SMDKV310', 'ARCH_EXYNOS'),
    ('config', 'k2hk_evm', 'TARGET_K2HK_EVM', 'ARCH_KEYSTONE'),
    ('config', 'openrd', 'TARGET_OPENRD', 'KIRKWOOD'),
    ('config', 'nhk8815', 'NOMADIK_NHK8815', 'ARCH_NOMADIK'),
    ('board', 'am3517evm', 'TARGET_AM3517_EVM', 'OMAP34XX'),
    ('board', 'duovero', 'TARGET_DUOVERO', 'OMAP44XX'),
    ('board', 'cm_t54', 'TARGET_CM_T54', 'OMAP54XX'),
    ('board', 'edminiv2', 'TARGET_EDMINIV2', 'ORION5X'),
    ('board', 'armadillo-800eva', 'TARGET_ARMADILLO_800EVA', 'RMOBILE'),
    ('board', 'goni', 'TARGET_S5P_GONI', 'ARCH_S5PC1XX'),
    ('soc', 'tegra20', 'TEGRA20', 'TEGRA'),
    ('board', 'harmony', 'TARGET_HARMONY', 'TEGRA20'),
    ('board', 'apalis_t30', 'TARGET_APALIS_T30', 'TEGRA30'),
    ('board', 'dalmore', 'TARGET_DALMORE', 'TEGRA114'),
    ('board', 'jetson-tk1', 'TARGET_JETSON_TK1', 'TEGRA124'),
    ('config', 'ph1_pro4', 'MACH_PH1_PRO4', 'ARCH_UNIPHIER'),
    ('config', 'zynq_zed', 'TARGET_ZYNQ_ZED', 'ZYNQ'),
    ('board', 'atngw100', 'TARGET_ATNGW100', 'AVR32'),
    ('board', 'bct-brettl2', 'TARGET_BCT_BRETTL2', 'BLACKFIN'),
    ('board', 'm52277evb', 'TARGET_M52277EVB', 'M68K'),
    ('board', 'microblaze-generic', 'TARGET_MICROBLAZE_GENERIC', 'MICROBLAZE'),
    ('config', 'qemu-mips', 'TARGET_QEMU_MIPS', 'MIPS'),
    ('board', 'adp-ag101', 'TARGET_ADP_AG101', 'NDS32'),
    ('board', 'nios2-generic', 'TARGET_NIOS2_GENERIC', 'NIOS2'),
    ('board', 'openrisc-generic', 'TARGET_OPENRISC_GENERIC', 'OPENRISC'),
    ('cpu', '74xx_7xx', '74xx_7xx', 'PPC'),
    ('config', 'P3G4', 'TARGET_P3G4', '74xx_7xx'),
    ('board', 'a3000', 'TARGET_A3000', 'MPC824X'),
    ('config', 'cogent_mpc8xx', 'TARGET_COGENT_MPC8XX', '8xx'),
    ('board', 'atc', 'TARGET_ATC', 'MPC8260'),
    ('board', 'mpc8308_p1m', 'TARGET_MPC8308_P1M', 'MPC83xx'),
    ('board', 'sbc8548', 'TARGET_SBC8548', 'MPC85xx'),
    ('board', 'sbc8641d', 'TARGET_SBC8641D', 'MPC86xx'),
    ('board', 'csb272', 'TARGET_CSB272', '4xx'),
    ('board', 'cmi', 'TARGET_CMI_MPC5XX', '5xx'),
    ('board', 'a3m071', 'TARGET_A3M071', 'MPC5xxx'),
    ('board', 'pdm360ng', 'TARGET_PDM360NG', 'MPC512X'),
    ('board', 'grsim_leon2', 'TARGET_GRSIM_LEON2', 'SPARC'),
    ('board', 'rsk7203', 'TARGET_RSK7203', 'SH'),
    ('board', 'coreboot', 'TARGET_COREBOOT', 'X86'),
)

### helper functions ###
def get_terminal_columns():
    """Get the width of the terminal.

    Returns:
      The width of the terminal, or zero if the stdout is not
      associated with tty.
    """
    try:
        return shutil.get_terminal_size().columns # Python 3.3~
    except AttributeError:
        import fcntl
        import termios
        import struct
        arg = struct.pack('hhhh', 0, 0, 0, 0)
        try:
            ret = fcntl.ioctl(sys.stdout.fileno(), termios.TIOCGWINSZ, arg)
        except IOError as exception:
            # If 'Inappropriate ioctl for device' error occurs,
            # stdout is probably redirected. Return 0.
            return 0
        return struct.unpack('hhhh', ret)[1]

def get_devnull():
    """Get the file object of '/dev/null' device."""
    try:
        devnull = subprocess.DEVNULL # py3k
    except AttributeError:
        devnull = open(os.devnull, 'wb')
    return devnull

def check_top_directory():
    """Exit if we are not at the top of source directory."""
    for f in ('README', 'Licenses'):
        if not os.path.exists(f):
            sys.exit('Please run at the top of source directory.')

def get_make_cmd():
    """Get the command name of GNU Make."""
    process = subprocess.Popen([SHOW_GNU_MAKE], stdout=subprocess.PIPE)
    ret = process.communicate()
    if process.returncode:
        sys.exit('GNU Make not found')
    return ret[0].rstrip()

RE_LIST = (
    ('arch', re.compile(r'CONFIG_SYS_ARCH="(.*)"')),
    ('cpu', re.compile(r'CONFIG_SYS_CPU="(.*)"')),
    ('soc', re.compile(r'CONFIG_SYS_SOC="(.*)"')),
    ('vendor', re.compile(r'CONFIG_SYS_VENDOR="(.*)"')),
    ('board', re.compile(r'CONFIG_SYS_BOARD="(.*)"')),
    ('config', re.compile(r'CONFIG_SYS_CONFIG_NAME="(.*)"')),
    ('spl', re.compile(r'CONFIG_SPL=(.*)')),
    ('tpl', re.compile(r'CONFIG_TPL=(.*)'))
)

def parse_dotconfig(defconfig, dotconfig):
    """Parse .config file and return board paramters."

    Arguments:
      dotconfig: Board (defconfig) name
    """
    params = {}
    for line in open(dotconfig):
        for key, pattern in RE_LIST:
            m = pattern.match(line)
            if m and m.group(1):
                params[key] = m.group(1)
                break

    params['defconfig'] = defconfig

    return params

def fixup_defconfig(src, dst, params):
    need_fixup = []
    for key, value, config, position in FIXUP_TABLE:
        if params.get(key) == value:
            need_fixup.append((config, position))

    if not need_fixup:
        shutil.move(src, dst)
        return

    prefix = '+'
    if params.get('spl') == 'y':
        prefix += 'S'
    if params.get('tpl') == 'y':
        prefix += 'T'
    if prefix == '+':
        prefix = ''
    else:
        prefix += ':'

    with open(src) as f:
        lines = f.readlines()

    for config, position in need_fixup:
        config = prefix + 'CONFIG_' + config + '=y\n'
        position = prefix + 'CONFIG_' + position + '=y\n'
        try:
            lines.insert(lines.index(position) + 1, config)
        except ValueError:
            sys.exit("\n'%s' not found in '%s'" % (position,
                                                   os.path.basename(dst)))

    with open(dst, 'w') as f:
        f.writelines(lines)

### classes ###

class Slot:

    """A slot to store a subprocess.

    Each instance of this class handles one subprocess.
    This class is useful to control multiple processes
    for faster processing.
    """

    def __init__(self, devnull, make_cmd):
        """Create a new slot.

        Arguments:
          output: File object which the result is written to
        """
        self.build_dir = tempfile.mkdtemp()
        self.devnull = devnull
        self.make_cmd = (make_cmd, 'O=' + self.build_dir)
        self.state = STATE_IDLE

    def __del__(self):
        """Delete the working directory"""
        if self.state != STATE_IDLE:
            while self.ps.poll() == None:
                pass
        shutil.rmtree(self.build_dir)

    def add(self, defconfig):
        """Add a new subprocess to the slot.

        Fails if the slot is occupied, that is, the current subprocess
        is still running.

        Arguments:
          defconfig: Board (defconfig) name

        Returns:
          Return True on success or False on fail
        """
        if self.state != STATE_IDLE:
            return False
        cmd = list(self.make_cmd)
        cmd.append(defconfig)
        self.ps = subprocess.Popen(cmd, stdout=self.devnull)
        self.defconfig = defconfig
        self.state = STATE_DEFCONFIG
        return True

    def poll(self):
        """Check if the subprocess is running and invoke the .config
        parser if the subprocess is terminated.

        Returns:
          Return True if the subprocess is terminated, False otherwise
        """
        if self.state == STATE_IDLE:
            return True

        if self.ps.poll() == None:
            return False

        if self.ps.poll() != 0:
            sys.exit("failed to process '%s'" % self.defconfig)

        if self.state == STATE_SAVEDEFCONFIG:
            fixup_defconfig(os.path.join(self.build_dir, 'defconfig'),
                            os.path.join('configs', self.defconfig),
                            self.params)

            self.state = STATE_IDLE
            return True

        assert self.state == STATE_DEFCONFIG

        cmd = list(self.make_cmd)
        cmd.append('savedefconfig')
        self.ps = subprocess.Popen(cmd, stdout=self.devnull)
        self.state = STATE_SAVEDEFCONFIG

        self.params = parse_dotconfig(self.defconfig,
                                      os.path.join(self.build_dir, '.config'))

        return False

class Slots:

    """Controller of the array of subprocess slots."""

    def __init__(self, jobs):
        """Create a new slots controller.

        Arguments:
          jobs: A number of slots to instantiate
        """
        self.slots = []
        devnull = get_devnull()
        make_cmd = get_make_cmd()
        for i in range(jobs):
            self.slots.append(Slot(devnull, make_cmd))

    def add(self, defconfig):
        """Add a new subprocess if a vacant slot is available.

        Arguments:
          defconfig: Board (defconfig) name

        Returns:
          Return True on success or False on fail
        """
        for slot in self.slots:
            if slot.add(defconfig):
                return True
        return False

    def available(self):
        """Check if there is a vacant slot.

        Returns:
          Return True if a vacant slot is found, False if all slots are full
        """
        for slot in self.slots:
            if slot.poll():
                return True
        return False

    def empty(self):
        """Check if all slots are vacant.

        Returns:
          Return True if all slots are vacant, False if at least one slot
          is running
        """
        ret = True
        for slot in self.slots:
            if not slot.poll():
                ret = False
        return ret

class Indicator:

    """A class to control the progress indicator."""

    MIN_WIDTH = 15
    MAX_WIDTH = 70

    def __init__(self, total):
        """Create an instance.

        Arguments:
          total: A number of boards
        """
        self.total = total
        self.cur = 0
        width = get_terminal_columns()
        width = min(width, self.MAX_WIDTH)
        width -= self.MIN_WIDTH
        if width > 0:
            self.enabled = True
        else:
            self.enabled = False
        self.width = width

    def inc(self):
        """Increment the counter and show the progress bar."""
        if not self.enabled:
            return
        self.cur += 1
        arrow_len = self.width * self.cur // self.total
        msg = '%4d/%d [' % (self.cur, self.total)
        msg += '=' * arrow_len + '>' + ' ' * (self.width - arrow_len) + ']'
        sys.stdout.write('\r' + msg)
        sys.stdout.flush()

def savedefconfig(jobs=1):
    check_top_directory()
    print 'Doing defconfig & savedefconfig ...  (jobs: %d)' % jobs

    # All the defconfig files to be processed
    defconfigs = []
    for (dirpath, dirnames, filenames) in os.walk('configs'):
        dirpath = dirpath[len('configs') + 1:]
        for filename in fnmatch.filter(filenames, '*_defconfig'):
            if fnmatch.fnmatch(filename, '.*'):
                continue
            defconfigs.append(os.path.join(dirpath, filename))

    slots = Slots(jobs)
    indicator = Indicator(len(defconfigs))

    # Main loop to process defconfig files:
    #  Add a new subprocess into a vacant slot.
    #  Sleep if there is no available slot.
    for defconfig in defconfigs:
        while not slots.add(defconfig):
            while not slots.available():
                # No available slot: sleep for a while
                time.sleep(SLEEP_TIME)
        indicator.inc()

    # wait until all the subprocesses finish
    while not slots.empty():
        time.sleep(SLEEP_TIME)
    print ''

def main():
    try:
        cpu_count = multiprocessing.cpu_count()
    except NotImplementedError:
        cpu_count = 1

    parser = optparse.OptionParser()
    # Add options here
    parser.add_option('-j', '--jobs', type='int', default=cpu_count,
                      help='the number of jobs to run simultaneously')
    (options, args) = parser.parse_args()

    savedefconfig(options.jobs)

if __name__ == '__main__':
    main()
