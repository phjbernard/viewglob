#!/bin/sh

SCRIPT_NAME=${0##*/}
VG_NO_ASTERISK=

while getopts ":t" opt; do
	case $opt in
		t) VG_NO_ASTERISK=yep ;;
		\?) echo "$SCRIPT_NAME: invalid flag." >&2 ;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $VG_NO_ASTERISK ]; then
	# Put a little asterisk on the front as an indicator that
	# this is a viewglob-controlled shell.
	export PS1="\[\033[44;1;33m\]*\[\033[0m\]${PS1}"
fi

# If the user has a PROMPT_COMMAND, delimit it with ;.
if [ "x${PROMPT_COMMAND}" != x ]; then
	PROMPT_COMMAND="${PROMPT_COMMAND};"
fi


# Adding semaphores to the ends of these variables.
export PS1="${PS1}\[\033[0;30m\]\[\033[0m\]\[\033[1;37m\]\[\033[0m\]"
export PS2="${PS2}\[\033[0;34m\]\[\033[0m\]\[\033[0;31m\]\[\033[0m\]"
export PROMPT_COMMAND="${PROMPT_COMMAND}"'echo -ne "\033P${PWD}\033\\"'

