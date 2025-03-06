#!/bin/bash
# Extracts HTML Help Workshop to /opt/htmlhelp.

set -e

if [[ $UID != 0 ]]; then
	echo "This script needs to be run as root (sorry)" >&2
	exit 1
fi

curl -fsSL 'https://web.archive.org/web/20200918004813/http://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe' -o /tmp/htmlhelp.exe
7z x /tmp/htmlhelp.exe -o/tmp/htmlhelp
mkdir -p /opt/htmlhelp
mv /tmp/htmlhelp/hhc.exe /opt/htmlhelp/hhc.exe
chmod +x /opt/htmlhelp/hhc.exe
rm -r /tmp/htmlhelp.exe /tmp/htmlhelp
