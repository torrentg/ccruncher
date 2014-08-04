<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE xsl:stylesheet [
  <!ENTITY nbsp "&#160;">
  <!ENTITY copy "&#169;">
]>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" version="1.0">

  <xsl:output method="html" encoding="utf-8" omit-xml-declaration="no" indent="no" 
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8" />
        <meta name="robots" content="noarchive"/>
        <title>
          CCruncher repo: <xsl:value-of select="/svn/index/@path"/>
        </title>
        <link rel="stylesheet" href="/ccruncher.css" type="text/css"/>
      </head>
      <body>
    <div id="container">
      <div id="header">
        <span class="cell">
          <a href="/index.html" class="title">&nbsp;CCRUNCHER</a>
        </span>
        <span class="cell subtitle">
          Open-Source Project for<br/>Credit Risk Modeling
        </span>
        <span class="cell">
          <img src="/logo.png" alt="logo" height="80" style="padding-right: 1em;"/>
        </span>
      </div>
      <div id="navbar">
        <ul>
          <li>
            <a href="/index.html">Home</a>
          </li>
          <li><a href="#">About</a>
            <ul>
              <li><a href="/faq.html">F.A.Q.</a></li>
              <li><a href="/features.html">Features</a></li>
              <li><a href="/screenshots.html">Screenshots</a></li>
            </ul>
          </li>
          <li><a href="#">Let's work!</a>
            <ul>
              <li><a href="http://sourceforge.net/projects/ccruncher/">Download&nbsp;<img src="/external.png" alt="" style="border:none;"/></a></li>
              <li><a href="/gstarted.html">Getting Started</a></li>
            </ul>
          </li>
          <li><a href="#">Documentation</a>
            <ul>
              <li><a href="/ccruncher.pdf">Technical Doc.&nbsp;<img src="/pdf.png" width="14" alt="" style="border:none;"/></a></li>
              <li><a href="/ifileref.html">Input File</a></li>
              <li><a href="/ofileref.html">Output Files</a></li>
              <li><a href="/dissemination.html">Dissemination</a></li>
            </ul>
          </li>
          <li><a class="active" href="#">Development</a>
            <ul>
              <li><a href="/dependencies.html">Dependencies</a></li>
              <li><a href="http://www.ccruncher.net/svn/">Repository (svn)</a></li>
            </ul>
          </li>
        </ul>
      </div>
      <div id="content">

<h1>Repository of Source Code</h1>
<h2>CCruncher - Revision <xsl:value-of select="/svn/index/@rev"/>: <xsl:value-of select="/svn/index/@path"/></h2>
<p>
<ul style="min-height: 300px;">
  <xsl:apply-templates select="/svn/index/updir"/>
  <xsl:apply-templates select="/svn/index/dir">
    <xsl:sort select="@name"/>
  </xsl:apply-templates>
  <xsl:apply-templates select="/svn/index/file">
    <xsl:sort select="@name"/>
  </xsl:apply-templates>
</ul>
</p>
</div>

      <div id="footer">
        <span id="copyright" >
          Copyright &copy; 2004-2014 - <a href="mailto:gtorrent@ccruncher.net">CCruncher</a>
        </span>
        <span id="modified">
          Last modified: 15-Jul-2014
        </span>
      </div>

  </div>

      </body>
    </html>
  </xsl:template>

<xsl:template match="updir">
  <li><a href="{@href}">..</a></li>
</xsl:template>
<xsl:template match="dir">
  <li><a href="{@href}"><xsl:value-of select="@name"/>/</a></li>
</xsl:template>
<xsl:template match="file">
  <li><a href="{@href}"><xsl:value-of select="@name"/></a></li>
</xsl:template>

</xsl:stylesheet>