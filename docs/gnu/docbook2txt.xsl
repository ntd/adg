<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:str="http://exslt.org/strings">

<xsl:output method="text" omit-xml-declaration="yes" indent="no" encoding="utf-8"/>
<xsl:strip-space elements="*"/>

<xsl:template match="refmeta|refnamediv">
</xsl:template>

<xsl:template match="refsect1">
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

<xsl:template match="itemizedlist">
<xsl:apply-templates select="listitem"/> 
<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="listitem">* <xsl:apply-templates/><xsl:text>
</xsl:text>
</xsl:template>

</xsl:stylesheet>
