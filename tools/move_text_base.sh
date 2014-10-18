#!/bin/sh

tools/genboardscfg.py

for config_mk in $(find board -name config.mk)
do
    # Get the value of CONFIG_SYS_TEXT_BASE
    text_base=$(make -f $config_mk --print-data-base 2>/dev/null | \
                sed -n -e 's/^CONFIG_SYS_TEXT_BASE = \(.*\)/\1/p')

    if [ -z "$text_base" ]; then
        continue
    fi

    if [ "$text_base" = 0 ]; then
        text_base=0x00000000
    fi

    sed -i -e '/CONFIG_SYS_TEXT_BASE *= .*/d' $config_mk
    if ! grep -q -v -e '#' -e '^$' $config_mk ; then
        # remove the file if only comments or empty lines are left
        git rm -f $config_mk
    else
        git add $config_mk
    fi

    # board name
    board=$(basename $(dirname $config_mk))
    # target name(s)
    targets=$(awk '($1 !~ /^#/ && $6 == "'$board'") { print $7 }' boards.cfg)

    for target in $targets
    do
        echo CONFIG_SYS_TEXT_BASE=$text_base >> configs/${target}_defconfig
        echo CONFIG_TEXT_BASE_IN_KCONFIG=y >> configs/${target}_defconfig

        make ${target}_defconfig
        make savedefconfig
        mv defconfig configs/${target}_defconfig
    done
done
