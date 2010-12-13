<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE xsl:stylesheet [<!ENTITY nbsp "&#160;">]>

<!--
===========================================================================
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2008-2010 Gerard Torrent
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
		method="xml" 
		encoding="UTF-8"
		omit-xml-declaration="yes"
		doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" 
		doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" 
		indent="yes"/>

<xsl:template match="/">
<html>
	<head>
		<title>ccruncher report: <xsl:value-of select="/ccruncher-report/segmentation/@name" /></title>
		<style type="text/css">
body {
	padding: 0px;
	margin: 0px;
	font: normal 10pt/12pt verdana,arial,sans-serif;
	color: black;
}
#main-container {
	width: 800px;
	margin: 0 auto;
	clear:both;
	padding-top: 10px;
	padding-left: 25px;
	padding-right: 25px;
	padding-bottom: 1px;
	background-color: white;
}
.gray-bar {
	margin-top: 20px;
	margin-bottom: 15px;
	padding-left: 5px;
	height: 40px;
	text-align: left;
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
	text-align: right;
	white-space:nowrap;
	font: normal 8pt/10pt verdana,arial,sans-serif;
}
table.its tr.odd {
	background-color: white;
	color: black;
	text-align: right;
	white-space:nowrap;
	font: normal 8pt/10pt verdana,arial,sans-serif;
}
		</style>
	</head>
	<body>
		<div id="main-container">
			<div class="gray-bar">
				<span class="big"><b><a style="text-decoration:none;color:black;" href="http://www.ccruncher.net">CCRUNCHER</a> REPORT</b></span>
			</div>
			<table width="100%"><tr><td align="center">
			<table class="its" border="0" cellpadding="4" cellspacing="4">
			<tr class="even">
				<td align="left">Segmentation name</td>
				<td align="center">
				<xsl:value-of select="/ccruncher-report/segmentation/@name"/>
				</td>
			</tr>
			<tr class="odd">
				<td align="left">Simulated values</td>
				<td align="center">
				<xsl:element name="a">
					<xsl:attribute name="href"><xsl:value-of select="/ccruncher-report/segmentation/@name" />.csv</xsl:attribute>
					<xsl:value-of select="/ccruncher-report/segmentation/@name" />.csv
				</xsl:element>
				</td>
			</tr>
			<tr class="even">
				<td align="left">Results file</td>
				<td align="center">
				<xsl:element name="a">
					<xsl:attribute name="href"><xsl:value-of select="/ccruncher-report/segmentation/@name" />.xml</xsl:attribute>
					<xsl:value-of select="/ccruncher-report/segmentation/@name" />.xml
				</xsl:element>
				</td>
			</tr>
			<tr class="odd">
				<td align="left">Number of simulations</td>
				<td align="center">
					<xsl:value-of select="/ccruncher-report/segmentation/segment[1]/size/@value"/>
				</td>
			</tr>
			</table>
			</td><td align="center">
			<xsl:if test="count(//ccruncher-report/segmentation/segment)>1">
				<xsl:element name="img">
					<xsl:attribute name="src"><xsl:value-of select="/ccruncher-report/segmentation/@name" />.png</xsl:attribute>
					<xsl:attribute name="width">300</xsl:attribute>
					<xsl:attribute name="alt">expected loss piechart</xsl:attribute>
				</xsl:element>
			</xsl:if>
			</td></tr></table>
			<xsl:apply-templates/>
			<hr style="margin-top:20px;"/>
			<p class="little" style="text-align:left; margin-bottom:30px;">
				<b>Note:</b> results are calculated from a Monte Carlo simulation and may differ across 
				simulations when using the same input.
			</p>
		</div>
	</body>
</html>
</xsl:template>

<xsl:template match="/ccruncher-report/segmentation/segment">
	<div class="gray-bar">
		<span class="big" style="text-transform:capitalize">
			<xsl:value-of select="@name"/> default loss simulation statistics
		</span>
	</div>
	<table width="100%" border="0"><tr>
	<td align="center" valign="middle">
		<xsl:element name="img">
			<xsl:attribute name="src"><xsl:value-of select="/ccruncher-report/segmentation/@name" />-<xsl:value-of select="/ccruncher-report/segmentation/segment/@name" />.png</xsl:attribute>
			<xsl:attribute name="width">300</xsl:attribute>
			<xsl:attribute name="alt">density function</xsl:attribute>
		</xsl:element>
	</td>
	<td align="center" valign="middle">
	<table class="its" border="0" cellpadding="4" cellspacing="4">
		<tr>
			<td>&nbsp;</td>
			<td class="little" align="center"><b>Mean</b></td>
			<td class="little" align="center"><b>StdErr</b></td>
			<td style="background-color: white;" colspan="4">&nbsp;</td>
		</tr>
		<tr class="even">
			<td align="center">Expected Loss</td>
			<td><xsl:value-of select="mean/@value"/></td>
			<td><xsl:value-of select="mean/@stderr"/></td>
			<td style="background-color: white;" colspan="4">&nbsp;</td>
		</tr>
		<tr>
			<td colspan="7">&nbsp;</td>
		</tr>
		<tr>
			<td class="little" nowrap="nowrap"><b>&nbsp;&nbsp;<a href="http://en.wikipedia.org/wiki/Value_at_risk">Value At Risk</a>&nbsp;&nbsp;</b></td>
			<td class="little" align="center"><b>Mean</b></td>
			<td class="little" align="center"><b>StdErr</b></td>
			<td>&nbsp;</td>
			<td class="little" nowrap="nowrap"><b><a href="http://en.wikipedia.org/wiki/Expected_shortfall">Expected Shortfall</a></b></td>
			<td class="little"><b>Mean</b></td>
			<td class="little"><b>StdErr</b></td>
		</tr>
		<tr class="even">
			<td align="center">VAR (90.00%)</td>
			<td><xsl:value-of select="VaR[@prob='0.9']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.9']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (90.00%)</td>
			<td><xsl:value-of select="ES[@prob='0.9']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.9']/@stderr"/></td>
		</tr>
		<tr class="odd">
			<td align="center">VAR (95.00%)</td>
			<td><xsl:value-of select="VaR[@prob='0.95']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.95']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (95.00%)</td>
			<td><xsl:value-of select="ES[@prob='0.95']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.95']/@stderr"/></td>
		</tr>
		<tr class="even">
			<td align="center">VAR (97.50%)</td>
			<td><xsl:value-of select="VaR[@prob='0.975']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.975']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (97.50%)</td>
			<td><xsl:value-of select="ES[@prob='0.975']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.975']/@stderr"/></td>
		</tr>
		<tr class="odd">
			<td align="center">VAR (99.00%)</td>
			<td><xsl:value-of select="VaR[@prob='0.99']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.99']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (99.00%)</td>
			<td><xsl:value-of select="ES[@prob='0.99']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.99']/@stderr"/></td>
		</tr>
		<tr class="even">
			<td align="center">VAR (99.25%)</td>
			<td><xsl:value-of select="VaR[@prob='0.9925']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.9925']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (99.25%)</td>
			<td><xsl:value-of select="ES[@prob='0.9925']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.9925']/@stderr"/></td>
		</tr>
		<tr class="odd">
			<td align="center">VAR (99.50%)</td>
			<td><xsl:value-of select="VaR[@prob='0.995']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.995']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (99.50%)</td>
			<td><xsl:value-of select="ES[@prob='0.995']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.995']/@stderr"/></td>
		</tr>
		<tr class="even">
			<td align="center">VAR (99.75%)</td>
			<td><xsl:value-of select="VaR[@prob='0.9975']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.9975']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (99.75%)</td>
			<td><xsl:value-of select="ES[@prob='0.9975']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.9975']/@stderr"/></td>
		</tr>
		<tr class="odd">
			<td align="center">VAR (99.90%)</td>
			<td><xsl:value-of select="VaR[@prob='0.999']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.999']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (99.90%)</td>
			<td><xsl:value-of select="ES[@prob='0.999']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.999']/@stderr"/></td>
		</tr>
		<tr class="even">
			<td align="center">VAR (99.99%)</td>
			<td><xsl:value-of select="VaR[@prob='0.9999']/@value"/></td>
			<td><xsl:value-of select="VaR[@prob='0.9999']/@stderr"/></td>
			<td style="background-color: white;">&nbsp;</td>
			<td align="center">ES (99.99%)</td>
			<td><xsl:value-of select="ES[@prob='0.9999']/@value"/></td>
			<td><xsl:value-of select="ES[@prob='0.9999']/@stderr"/></td>
		</tr>
	</table>
	</td>
	</tr></table>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>

