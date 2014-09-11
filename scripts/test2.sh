#!/bin/sh

set -e

debug () {
        if [ $DEBUG ]; then
                echo "$@"
        fi
}

exported_configs=

parse_kconfig () {
	# input file
	in_file=$1

	# config symbol
	config=

	# flags to remember where we are
	in_choice=
	in_help=

	while read token arg
	do
		debug "$token $arg"
		# Anything following '#' is a comment. Discard it.
		arg="${arg%%#*}"

		case $token in
		\#*)
			# skip a comment line
			continue
			;;
		choice)
			if [ "$in_help" ]; then
				continue
			fi
			debug "=== choice ==="
			in_choice=1
			;;
		endchoice)
			if [ "$in_help" ]; then
				continue
			fi
			debug "=== endchoice ==="
			choice_is_exported=
			;;
		help)
			if [ "$in_help" ]; then
				continue
			fi
			debug "=== help ==="
			in_help=1
			;;
		config|menuconfig)
			if [ "$in_help" ]; then
				continue
			fi
			config=$arg
			debug "=== config '$config'==="
			if [ "$choice_is_exported" ]; then
				debug "=== Exporting '$config' ==="
				exported_configs="$exported_configs $config"
			fi
			;;
		source)
			if [ "$in_help" ]; then
				continue
			fi
			new_kconfig=$(expr $arg : '"\(.*\)"' )
			debug "=== Entering '$new_kconfig' ==="
			parse_kconfig $new_kconfig
			;;
		@export@)
			if [ "$in_choice" ]; then
				choice_is_exported=1
			fi

			if [ "$config" -a -z "$choice_is_exported" ]; then
				debug "=== Exporting '$config' ==="
				exported_configs="$exported_configs $config"
			fi
			;;
		"")
			# We assume each entry ends with a blank line
			config=
			in_choice=
			in_help=
			continue;;
		*)
			;;
		esac

	done < $in_file

	config=
	in_choice=
	in_help=
}

is_exported () {

	for config in $exported_configs
	do
		if [ "$1" = "$config" ]; then
			echo y
			return
		fi
	done

	return
}

get_exported_symbols () {

	parse_kconfig Kconfig

	echo exported_configs:
	echo $exported_configs

	while read line
	do
		case $line in
		CONFIG_*=y)
			tmp=${line#CONFIG_}
			tmp=${tmp%=y}
			if [ "$(is_exported $tmp)" = y ]; then
				echo $tmp is_exported
			fi
			;;
		*)
			;;
		esac
	done < .config
}


get_exported_symbols

