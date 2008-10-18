<?xml version="1.0" encoding="UTF-8"?>

<!--
===========================================================================
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2008 Gerard Torrent
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
#
# transform.xsl - report stylesheet (http://www.r-project.org) - $Rev: 461 $
# __________________________________________________________________________
#
# usage:
# xsltproc \-\-stringparam name portfolio-rest transform.xsl portfolio-rest.xml 
# __________________________________________________________________________
#
# 2008/10/18 - Gerard Torrent [gerard@mail.generacio.com]
#   . initial release
#
===========================================================================
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="xml" encoding="UTF-8"
		doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" 
		doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" indent="yes"/>

	<xsl:template match="/">

<html>
	<head>
		<title>ccruncher report: <xsl:value-of select="$name" /></title>
		<style type="text/css">
body {
	padding: 0px;
	margin: 0px;
	font: normal 8pt/13pt verdana,arial,sans-serif;
	color: black;
}
#main-container {
	width:650px;
	margin: 0 auto;
	clear:both;
	padding-top: 10px;
	padding-left: 25px;
	padding-right: 25px;
	padding-bottom: 1px;
	background-color: white;
}
.gray-bar {
	margin-top:20px;
	margin-bottom:15px;
	height: 40px;
	text-align: center;
	background-color: #E0E0E0;
	width: 100%;
	display: table;
	-moz-border-radius: 3px;
	-webkit-border-radius: 3px;
}
.big {
	display: table-cell;
	vertical-align: middle;
	font: normal 14pt/14pt verdana,arial,sans-serif;
}
.little {
	display: table-cell;
	vertical-align: middle;
	font: normal 8pt/13pt verdana,arial,sans-serif;
}
table.its tr.even {
	background-color: #def;
	color: black;
}
table.its tr.odd {
	background-color: white;
	color: black;
}
		</style>
	</head>  
	<body>
		<div id="main-container">
			<div class="gray-bar">
				<span class="big">Statistics</span>
			</div>
			<table width="100%" border="0"><tr>
			<td align="center" valign="middle">
			<table class="its" border="0" cellpadding="4" cellspacing="4">
				<tr class="even">
					<td>
						<b>Mean(Expected Loss)</b>
					</td>
					<td align="right"><xsl:value-of select="/ccruncher-report/mean/@value"/></td>
				</tr>
				<tr class="odd">
					<td>
						<b>Stddev(Expected Loss)</b>
					</td>
					<td align="right"><xsl:value-of select="/ccruncher-report/mean/@stderr"/></td>
				</tr>
				<tr class="even">
					<td>
						<b>Mean(Stddev Portfolio Loss)</b>
					</td>
					<td align="right"><xsl:value-of select="/ccruncher-report/stddev/@value"/></td>
				</tr>
				<tr class="odd">
					<td>
						<b>Stddev(Stddev Portfolio Loss)</b>
					</td>
					<td align="right"><xsl:value-of select="/ccruncher-report/stddev/@stderr"/></td>
				</tr>
				<tr class="even">
					<td>
						<b>Minimum loss simulated value</b>
					</td>
					<td align="right"><xsl:value-of select="/ccruncher-report/min/@value"/></td>
				</tr>
				<tr class="odd">
					<td>
						<b>Maximum loss simulated value</b>
					</td>
					<td align="right"><xsl:value-of select="/ccruncher-report/max/@value"/></td>
				</tr>
				<tr class="even">
					<td>
						<b>Number of simulations</b>
					</td>
					<td align="right"><xsl:value-of select="/ccruncher-report/size/@value"/></td>
				</tr>
				<tr class="odd">
					<td>
						<b>Simulations file</b>
					</td>
					<td align="center">
						<xsl:element name="a">
							<xsl:attribute name="href"><xsl:value-of select="$name" />.out</xsl:attribute>
							file
						</xsl:element>
					</td>
				</tr>
				<tr class="even">
					<td>
						<b>Results file</b>
					</td>
					<td align="center">
						<xsl:element name="a">
							<xsl:attribute name="href"><xsl:value-of select="$name" />.xml</xsl:attribute>
							file
						</xsl:element>
					</td>
				</tr>
			</table>
			</td>
			<td align="center" valign="top">
			<table border="1" cellpadding="4" cellspacing="4" style="border:none;">
				<tr>
					<td align="center" style="border:none;">
					</td>
					<td colspan="2" align="center">
						<b><a style="text-decoration:none;color:black;" href="http://en.wikipedia.org/wiki/Value_at_risk">Value At Risk</a></b>
					</td>
					<td colspan="2" align="center" nowrap="nowrap">
						<b><a style="text-decoration:none;color:black;" href="http://en.wikipedia.org/wiki/Expected_shortfall">Expected Shortfall</a></b>
					</td>
				</tr>
				<tr>
					<td align="center" style="border:none;">
					</td>
					<td align="center">
						<b>Mean</b>
					</td>
					<td align="center">
						<b>Stddev</b>
					</td>
					<td align="center">
						<b>Mean</b>
					</td>
					<td align="center">
						<b>Stddev</b>
					</td>
				</tr>
				<tr>
					<td align="center"><b>90%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>95%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.95']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.95']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.95']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.95']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>97.5%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.975']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.975']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.975']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.975']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>99%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.99']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.99']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.99']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.99']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>99.25%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9925']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9925']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9925']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9925']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>99.5%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.995']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.995']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.995']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.995']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>99.75%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9975']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9975']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9975']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9975']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>99.9%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.999']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.999']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.999']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.999']/@stderr"/></td>
				</tr>
				<tr>
					<td align="center"><b>99.99%</b></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9999']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/VaR[@prob='0.9999']/@stderr"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9999']/@value"/></td>
					<td align="center"><xsl:value-of select="/ccruncher-report/ES[@prob='0.9999']/@stderr"/></td>
				</tr>
			</table>
			</td>
			</tr></table>
			<div class="gray-bar">
				<span class="big">Graphics</span>
			</div>
			<center>
				<xsl:element name="img">
					<xsl:attribute name="src"><xsl:value-of select="$name" />.png</xsl:attribute>
					<xsl:attribute name="alt">graphics</xsl:attribute>
				</xsl:element>
			</center>
			<div class="gray-bar">
				<span class="little">
					risk computed by <a href="http://www.generacio.com/ccruncher/">ccruncher</a>
				</span>
			</div>
		</div>
	</body>
</html>

</xsl:template>

</xsl:stylesheet>

