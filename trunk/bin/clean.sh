#!/bin/sh

rm -vf `find . -name \*\~`    2> /dev/null
rm -vf `find . -name \*\.dvi` 2> /dev/null
rm -vf `find . -name \*\.aux` 2> /dev/null
rm -vf `find . -name \*\.idx` 2> /dev/null
rm -vf `find . -name \*\.log` 2> /dev/null
rm -vf `find . -name \*\.lot` 2> /dev/null
rm -vf `find . -name \*\.out` 2> /dev/null
rm -vf `find . -name \*\.toc` 2> /dev/null
rm -vf `find . -name \*\.bbl` 2> /dev/null
rm -vf `find . -name \*\.blg` 2> /dev/null
rm -vf `find . -name \*\.idx` 2> /dev/null
rm -vf `find . -name \*\.ilg` 2> /dev/null
rm -vf `find . -name \*\.ind` 2> /dev/null
