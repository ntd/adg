<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:str="http://exslt.org/strings">

<xsl:output method="text" omit-xml-declaration="yes" indent="no" encoding="utf-8"/>
<xsl:strip-space elements="*"/>

<xsl:template match="/chapter/title|subtitle">
</xsl:template>

<xsl:template match="simplesect">
<xsl:apply-templates/>
<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="title">
<xsl:value-of select="."/>
<xsl:text>
</xsl:text>
<xsl:value-of select="str:padding(string-length(),'=')"/>
<xsl:text>

</xsl:text>
</xsl:template>

<xsl:template match="para">
<xsl:apply-templates/>
<xsl:text>

</xsl:text>
</xsl:template>

<xsl:template match="ulink"><xsl:value-of select="."/> (<xsl:value-of select="@url"/>)</xsl:template>

<xsl:template match="programlisting">
------------------------------------------------------------------
<xsl:value-of select="."/>
------------------------------------------------------------------
<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="itemizedlist|orderedlist">
<xsl:apply-templates/> 
<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="itemizedlist[@mark='circle']/listitem[not(@override='disc')]|listitem[@override='circle']">
<xsl:call-template name="item"> 
  <xsl:with-param name="symbol" select="' o '"/>
</xsl:call-template>
</xsl:template>

<xsl:template match="listitem">
<xsl:call-template name="item"> 
  <xsl:with-param name="symbol" select="' * '"/>
</xsl:call-template>
</xsl:template>

<xsl:template match="orderedlist/listitem">
<xsl:call-template name="item">
  <xsl:with-param name="symbol"><xsl:number format="1. "/></xsl:with-param>
</xsl:call-template>
</xsl:template>

<xsl:template name="item">
<xsl:param name="symbol"/>
<xsl:value-of select="$symbol"/><xsl:apply-templates/><xsl:text>
</xsl:text>
</xsl:template>

</xsl:stylesheet>
