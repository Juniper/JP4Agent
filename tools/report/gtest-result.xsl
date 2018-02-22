<?xml version='1.0'?>
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="testsuites">
        <html>
            <head>
                <title> GTest Report - <xsl:value-of select="@name"/> </title>
            </head>

            <!--body bgcolor="#e0e0f0" -->
            <body>
                <div align="center">
                    <h3>
                        <b> GTest Report - <xsl:value-of select="@name"/> </b>
                    </h3>
                </div>
                <table cols="4" width="80%" align="center">
                    <tr>
                        <td width="25%"> </td>
                        <td width="25%"> </td>
                        <td width="25%"> </td>
                        <td width="25%"> </td>
                    </tr>
                    <xsl:apply-templates/>
                </table>
                <p/>
                <hr align="center" width="80%" color="maroon" />
            </body>
        </html>
    </xsl:template>

    <xsl:template match="testsuite">
        <!--tr bgcolor="#f0e0f0"-->
        <tr bgcolor="#BDB76B">
            <td colspan="4">
                Suite: <xsl:value-of select="@name"/>
            </td>
        </tr>
        <xsl:apply-templates/>
        <tr>
            <td width="25%">
                Tests: <xsl:value-of select="@tests"/>
            </td>
            <td width="25%">
                Disabled: <xsl:value-of select="@disabled"/>
            </td>
            <td width="25%">
                Failures: <xsl:value-of select="@failures"/>
            </td>
            <td width="25%">
                Errors: <xsl:value-of select="@errors"/>
            </td>
        </tr>
    </xsl:template>

    <xsl:template match="testcase">
        <tr>
            <td> </td>
            <td colspan="2">
                Test: <xsl:value-of select="@name"/>
            </td>
            <xsl:choose>
                <xsl:when test="child::*">
                    <td bgcolor="#ff0000"> Failed </td>
                </xsl:when>
                <xsl:otherwise>
                    <td bgcolor="#00ff00"> Passed </td>
               </xsl:otherwise>
            </xsl:choose>
        </tr>
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="failure">
        <tr><td colspan="4">
                <table width="100%">
                    <tr>
                        <th width="15%"> File Name </th>
                        <td width="50%" bgcolor="#e0eee0">
                            <xsl:value-of select="substring-before(.,':')"/>
                        </td>
                        <th width="20%"> Line Number </th>
                        <td width="10%" bgcolor="#e0eee0">
                            <xsl:value-of select='substring-after(substring-before(.,"&#x000A;"),":")'/>
                        </td>
                    </tr>
                    <tr>
                        <th width="15%"> message </th>
                        <td colspan="3" width="85%" bgcolor="#e0eee0">
                            <xsl:value-of select="@message"/>
                        </td>
                    </tr>
                </table>
        </td></tr>
    </xsl:template>
</xsl:stylesheet>
