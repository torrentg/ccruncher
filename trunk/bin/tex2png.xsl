<?xml version="1.0" encoding="UTF-8"?>

<!--
===========================================================================
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2013 Gerard Torrent
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
===========================================================================
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output 
	method="text" 
	encoding="UTF-8"
	omit-xml-declaration="yes"
	indent="no"/>

<xsl:template match="/">#!/bin/bash
retcode=0
<xsl:for-each select="//object[@class='equation']">
filename=<xsl:value-of select="@data"/>
name=${filename%.*};
extension=${filename##*.};
echo -n &quot;  $name &quot;
latex -jobname=$name  &gt;/dev/null 2&gt;/dev/null &lt;&lt; "_EOF_"
\documentclass{article} 
\usepackage[american]{babel}
\usepackage{array}
\usepackage{amsmath}
\usepackage{amsthm}
\usepackage{amssymb}
\pagestyle{empty} 
\begin{document} 
\begin{displaymath}<xsl:value-of select="."/>\end{displaymath}
\end{document}
_EOF_
if [ $? != 0 ]; then
  echo "[failed]";
  retcode=`expr $retcode + 1`;
else
  dvipng -T tight -z 9 -bg transparent -o $name.png $name.dvi &gt;/dev/null 2&gt;/dev/null
  if [ $? != 0 ]; then
    echo "[failed]";
    retcode=`expr $retcode + 1`;
  else
    rm -f $name.log
    rm -f $name.dvi
    rm -f $name.aux
    echo "[success]";
  fi
fi
</xsl:for-each>
exit $retcode;
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
