#!/bin/bash
args=("$(wslpath -w "$(dirname "$0")/sign.cmd")")
for arg in "$@"; do
	case "$arg" in
		*/*) args+=("$(wslpath -w "$arg")") ;;
		*)   args+=("$arg") ;;
	esac
done
exec cmd.exe /c "${args[@]}"
