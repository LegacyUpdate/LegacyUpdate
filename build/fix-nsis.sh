#!/bin/bash
# Fixes NSIS binaries so they run on Pentium/486. Debian's build of NSIS is compiled with MinGW,
# but they lack any -mcpu flag, so the binaries receive a default of i686. To fix this, we'll
# download the official Windows build of NSIS and extract its binaries. We only handle stubs though,
# because we provide our own plugin builds in this repo.

set -e

if [[ $UID != 0 ]]; then
	echo "This script needs to be run as root (sorry)" >&2
	exit 1
fi

if [[ ! -d /usr/share/nsis/Stubs ]]; then
	echo "NSIS not installed, or Stubs directory is broken" >&2
	exit 1
fi

if [[ -d /usr/share/nsis/Stubs_old ]]; then
	echo "NSIS stubs are already fixed" >&2
	exit 0
fi

apt-get install -qy curl p7zip-full

mkdir /tmp/nsis
cd /tmp/nsis

curl -fSL https://prdownloads.sourceforge.net/nsis/NSIS%203/3.10/nsis-3.10-setup.exe -o nsis.exe
7z x nsis.exe

mv /usr/share/nsis/Stubs{,_old}
cp -ra Stubs /usr/share/nsis/Stubs

rm -rf /tmp/nsis
