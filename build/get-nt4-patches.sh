#!/bin/bash
# Get dependencies for NT4 build

set -e

cd "$(dirname "$0")"/..

for i in usb; do
	echo "Downloading $i..."
	curl -fL https://content.legacyupdate.net/legacyupdate/patches/nt4/$i.zip -o /tmp/$i.zip
	unzip -o /tmp/$i.zip -d setup/patches/nt4/$i
	rm /tmp/$i.zip
done
