<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE xsl:stylesheet [
  <!ENTITY nbsp "&#160;">
  <!ENTITY copy "&#169;">
]>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" version="1.0">
  <xsl:output method="html" encoding="utf-8" indent="yes"/>
  <xsl:template match="/">
<html lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <meta http-equiv="x-ua-compatible" content="ie=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no" />
    <title>CCruncher repo: <xsl:value-of select="/svn/index/@path"/></title>
    <link rel="stylesheet" href="/css/font-awesome.min.css" type="text/css" />
    <link rel="stylesheet" href="/css/bootstrap.min.css" type="text/css" />
    <link rel="stylesheet" href="/css/ccruncher.css" type="text/css" />
    <script src="/js/jquery-3.1.1.min.js"></script>
    <script src="/js/tether.min.js"></script>
    <script src="/js/bootstrap.min.js"></script>
  </head>
  <body>
    <div class="container-fluid">
      <nav id="navbar" class="navbar navbar-dark">
        <a class="navbar-brand" href="/index.html">CCRUNCHER</a>
        <button class="navbar-toggler hidden-sm-up float-xs-right" type="button" data-toggle="collapse" data-target="#navbarResponsive" aria-controls="navbarResponsive" aria-expanded="false" aria-label="Toggle navigation"></button>
        <div class="collapse navbar-toggleable-xs" id="navbarResponsive">
          <ul class="nav navbar-nav">
            <li class="nav-item dropdown">
              <a class="nav-link dropdown-toggle" href="#" id="dropdown1" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                About
              </a>
              <div class="dropdown-menu" aria-labelledby="dropdown1">
                <a class="dropdown-item" href="/faq.html">F.A.Q.</a>
                <a class="dropdown-item" href="/features.html">Features</a>
                <a class="dropdown-item" href="/screenshots.html">Screenshots</a>
              </div>
            </li>
            <li class="nav-item dropdown">
              <a class="nav-link dropdown-toggle" href="#" id="dropdown2" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                Let's work!
              </a>
              <div class="dropdown-menu" aria-labelledby="dropdown2">
                <a class="dropdown-item" href="/download.html">Download</a>
                <a class="dropdown-item" href="/gstarted.html">Getting Started</a>
              </div>
            </li>
            <li class="nav-item dropdown">
              <a class="nav-link dropdown-toggle" href="#" id="dropdown3" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                Documentation
              </a>
              <div class="dropdown-menu" aria-labelledby="dropdown3">
                <a class="dropdown-item" href="/ccruncher.pdf">Technical Doc.&nbsp;<i class="fa fa-file-pdf-o"></i></a>
                <a class="dropdown-item" href="/ifileref.html">Input File</a>
                <a class="dropdown-item" href="/ofileref.html">Output Files</a>
                <a class="dropdown-item" href="/dissemination.html">Dissemination</a>
              </div>
            </li>
            <li class="nav-item dropdown">
              <a class="nav-link dropdown-toggle active" href="#" id="dropdown4" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                Development
              </a>
              <div class="dropdown-menu" aria-labelledby="dropdown4">
                <a class="dropdown-item" href="/dependencies.html">Dependencies</a>
                <a class="dropdown-item" href="http://www.ccruncher.net/svn/">Repository (svn)</a>
              </div>
            </li>
          </ul>
        </div>
      </nav>
      <main style="margin-top: 1rem;">
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
      </main>
      <footer>
        <div id="footer" class="card card-block">
          <span id="copyright">
            Copyright &copy; 2004-2016 - <a href="mailto:gtorrent@ccruncher.net">CCruncher</a>&nbsp;&nbsp;
          </span>
          <span id="modified" class="float-sm-right">
            Last modified: 04-Dec-2016
          </span>
        </div>
      </footer>
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
