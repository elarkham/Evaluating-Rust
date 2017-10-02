#!/bin/sh

# Allow prompting the user.
exec < /dev/tty

verify_commit() {
	SUBJECT="$1"
	RET=0

	if [ ${#SUBJECT} -gt 50 ]; then
		printf -- \
	"error: commit subject longer than 50 characters\n"
		RET=-1
	fi

	if [ "$SUBJECT" != "${SUBJECT%  *}" ]; then
		printf -- \
	"error: commit subject delimited by multiple spaces between words\n"
		RET=-1
	fi

	if ! [ "$SUBJECT" == "$(echo "$SUBJECT" | grep -oe "[A-Z].*")" ]; then
		printf -- \
	"error: commit subject first word does not begin with a captial letter\n"
		RET=-1
	fi

	if [ "$SUBJECT" == "$(echo "$SUBJECT" | grep -oe ".*\.")" ]; then
		printf -- \
	"error: commit subject ends with a period\n"
		RET=-1
	fi

	return $RET
}

while true; do
	verify_commit "$(head -n 1 "$1")"
	if [ $? -eq 0 ]; then
		exit 0
	fi

	printf -- "Validation failed, return to \"%s\"? [Y/n]\n" "$EDITOR"

	read REPLY
	case "$REPLY" in
	Y|y)
		$EDITOR $1
		;;
	N|n)
		exit -1
		;;
	*)
		$EDITOR $1
		;;
	esac
done
