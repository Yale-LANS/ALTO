#!/bin/sh

SCRIPT=$0
ROOT=$(dirname $SCRIPT)

TEMPLATE=$ROOT/FindLibrary.cmake.tmpl

while read libname header
do
	echo "Processing library $libname ($header)"

	# Replace '/' with '\/'
	header=${header//\//\\\/}

	cat $TEMPLATE					\
	| sed -e "s/@LIBNAME@/$libname/g"		\
	| sed -e "s/@HEADER@/$header/g"			\
	> $ROOT/Find$libname.cmake
done < $ROOT/simple-modules.txt

