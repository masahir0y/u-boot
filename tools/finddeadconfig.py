#!/usr/bin/env python
#
# Author: Masahiro Yamada <yamada.m@jp.panasonic.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

import os
import fnmatch
import glob
import subprocess

'''
Find dead config

Run 'tools/genboardscfg.py' to create boards.cfg file.

Run 'tools/genboardscfg.py -h' for available options.
'''

verbose = False

prune = ['./scripts/kconfig', './scripts/basic']
skip_files = ['./arch/arm/include/asm/mach-types.h']
referenced_configs = []
defined_configs = []

def log(msg):
    if verbose:
        print msg

def gen_gtags():
    subprocess.check_call(['gtags'])

def parse_extra_options(value):
    value = value.strip('"\n')
    for option in value.split(','):
        equal = option.find('=')
        if equal != -1:
            option = option[:equal]
        config = 'CONFIG_' + option
        if not config in defined_configs:
            log('appending %s to defined_configs' % config)
            defined_configs.append(config)

def parse_defconfigs():
    for defconfig in glob.glob('configs/*_defconfig'):
        log('parsing %s' % defconfig)
        with open(defconfig) as f:
            for line in f:
                colon = line.find(':CONFIG_')
                line = line[colon + 1:]
                front, match, rear = line.partition('CONFIG_SYS_EXTRA_OPTIONS=')
                if match:
                    parse_extra_options(rear)
                else:
                    equal = line.find('=')
                    config = line[:equal]
                    if not config in defined_configs:
                        log('appending %s to defined_configs' % config)
                        defined_configs.append(config)

def add_references(filepath):
    with open(filepath) as f:
        for line in f:
            pos = 0
            while True:
                start = line.find('CONFIG_', pos)
                if start == -1:
                    break
                pos = start + 7
                if start != 0:
                    if line[start - 1].isalnum() or line[start - 1] == '_':
                        continue
                while line[pos].isalnum() or line[pos] == '_':
                    pos += 1
                config = line[start:pos]
                if not config in referenced_configs:
                    log('appending %s to referenced_configs' % config)
                    referenced_configs.append(config)

def is_skip_dir(dirpath):
    for d in prune:
        if os.path.commonprefix([d, dirpath]) == d:
            return True
    return False

def walk():
    for (dirpath, dirnames, filenames) in os.walk('.'):
        if is_skip_dir(dirpath):
            log('skipped %s' % dirpath)
            continue
        for filename in fnmatch.filter(filenames, '*.[ch]') + fnmatch.filter(
                                                        filenames, 'Makefile'):
            filepath = os.path.join(dirpath, filename)
            if filepath in skip_files:
                continue
            log('parsing %s' % filepath)
            add_references(filepath)

def check_defined():
    for config in referenced_configs:
        log('checking %s' % config)
        if config in defined_configs:
            continue
        if subprocess.check_output(['global', config]) != '':
            continue
        print '%s is not defined' % config

def main():
    #gen_gtags()
    parse_defconfigs()
    walk()
    print 'Number of configs: %d' % len(referenced_configs)
    check_defined()

if __name__ == '__main__':
    main()
