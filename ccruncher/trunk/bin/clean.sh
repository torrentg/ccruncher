#!/bin/sh

rm -v `find . -name \*\~`    2> /dev/null
rm -v `find . -name \*\.dvi` 2> /dev/null
rm -v `find . -name \*\.aux` 2> /dev/null
rm -v `find . -name \*\.idx` 2> /dev/null
rm -v `find . -name \*\.log` 2> /dev/null
rm -v `find . -name \*\.lot` 2> /dev/null
rm -v `find . -name \*\.out` 2> /dev/null
rm -v `find . -name \*\.toc` 2> /dev/null
