<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version='1.0'
  xmlns="http://www.w3.org/TR/xhtml1/transitional"
  exclude-result-prefixes="#default">
  
  <!--  <xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/html/chunk.xsl"/> -->
  <xsl:import href="/usr/local/share/xsl/docbook/html/chunk.xsl"/>

  <!-- Parameter overrides -->
  <xsl:variable name="root.filename" select="'index'"/>
  <xsl:variable name="use.id.as.filename">1</xsl:variable>
  <xsl:variable name="generate.legalnotice.link">0</xsl:variable>
  <xsl:variable name="css.decoration">1</xsl:variable>
</xsl:stylesheet>
