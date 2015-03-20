#!/bin/bash

if [[ ! -f Makefile ]];
then
	echo Run this script from the root folder of BOSSA
	exit 1
fi

make clean

VERSION=`cat Makefile | grep ^VERSION= | tr '=' ' ' | awk '{ print $2; }'`
OS=`uname -s | cut -c -7`

if [[ x$OS == xDarwin ]];
then
	# OSX
	make bin/bossac -j4
	strip bin/bossac
	GCC_ARCH=`gcc -v -arch i386 2>&1 | awk '/Target/ { print $2 }'`
elif [[ x$OS == xMINGW32 ]];
then
	# Windows
	make bin/bossac.exe -j4
	strip --strip-all bin/bossac.exe
	GCC_ARCH=`gcc -v 2>&1 | awk '/Target/ { print $2 }'`
else
	# Linux
	make bin/bossac -j4
	strip --strip-all bin/bossac
	GCC_ARCH=`gcc -v 2>&1 | awk '/Target/ { print $2 }'`
fi

mkdir "arduino/bossac-$VERSION"
cp bin/bossac* "arduino/bossac-$VERSION"
cd arduino
tar cfz bossac-$VERSION-$GCC_ARCH.tar.gz bossac-$VERSION
cd ..
rm -r "arduino/bossac-$VERSION"

