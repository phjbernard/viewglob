
# Source the user's run-control file.
[ -f ~/.bashrc ] && . ~/.bashrc

if [ "$VG_SANDBOX" = yep ]; then
	# This is all for the sandbox shell.
	
	# Disable history expansion.
	set +o history

else
	# This is all for the user's shell.

	if [ "$VG_ASTERISK" = yep ]; then
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
	export PROMPT_COMMAND="${PROMPT_COMMAND}"'printf "\033P${PWD}\033\\"'
fi

# Don't want to clutter the environment.
unset VG_ASTERISK
unset VG_SANDBOX

