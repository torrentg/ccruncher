#!/bin/sh

#=============================================================
#
# description: 
#   convert TeX formulas in html documents to png images
#
# html format:
#   <object class="equation" data="filename.png" type="image/png">
#     y = \int_0^\infty \gamma^2 \cos(x) dx 
#   </object>
#
# examples:
#   tex2png.sh file1.html file2.html ...
#
# dependencies:
#   shell, xsltproc, latex, dvipng
#
# retcodes:
#   0    : OK
#   other: KO
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
progname=tex2png.sh
numversion="2.0"
svnversion="R849"
retcode=0
xslfile=$(dirname "$0")/tex2png.xsl;
xslfile=$(readlink -f "$xslfile");

#-------------------------------------------------------------
# checking dependences
#-------------------------------------------------------------
if [ ! -f $xslfile ]
then
  echo "aborting: file $xslfile does not exist";
fi
command -v xsltproc >/dev/null 2>&1 || { echo "aborting: xsltproc command not found" >&2; exit 1; }
command -v latex >/dev/null 2>&1 || { echo "aborting: latex command not found" >&2; exit 1; }
command -v dvipng >/dev/null 2>&1 || { echo "aborting: dvipng command not found" >&2; exit 1; }

#-------------------------------------------------------------
# process html file
#-------------------------------------------------------------
process() {

  name=${1%.*};
  extension=${1##*.};

  echo "processing file $1 ...";
  xsltproc --html --nonet -o ${name}-$$.sh $xslfile $1;
  if [ $? != 0 ]; then
    echo "  error processing html!";
    retcode=`expr $retcode + 1`;
    return;
  fi

  sh ${name}-$$.sh;
  if [ $? != 0 ]; then
    echo "error processing images!";
    retcode=`expr $retcode + 1`;
  else
    rm -f ${name}-$$.sh
  fi
  
}

#-------------------------------------------------------------
# main procedure
#-------------------------------------------------------------
for filename in "$@"
do
  cwd=$PWD;
  cd $(dirname $filename);
  process $(basename $filename);
  cd $cwd;
done

exit $retcode;
