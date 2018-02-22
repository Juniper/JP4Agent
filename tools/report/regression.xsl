<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format">
<xsl:template match="/">
  <html>
  <body>
  <h2>JP4Agent Regression</h2>
    <pre>
    <table border="1">
    <fo:block font-size="5pt" font-family="monospace">
      <tr bgcolor="#BDB76B">
      <td>Time</td>
      <xsl:for-each select="rafi/header">
        <xsl:if test="position()=1">
        <xsl:for-each select="step">
            <th><xsl:value-of select="@name"/></th>
        </xsl:for-each>
        </xsl:if>
      </xsl:for-each>
      <!--td>GTEST report</td-->
      <td>Logs</td>
      <td>Container</td>
      <!--td>Hash</td-->
      <td>VMX (Version)</td>
      <!--td>VMX GRN</td-->
      </tr>

      <xsl:for-each select="rafi/regression">
      <tr>
      <td>
      <xsl:value-of select="@time"/>
      </td>
      <xsl:for-each select="step">
      <xsl:choose>
          <xsl:when test="@result='-'">
              <xsl:variable name="status_line"><xsl:value-of select="@status"/></xsl:variable>
              <td title="{$status_line}"><xsl:value-of select="@result"/></td>
          </xsl:when>
          <xsl:when test="@result='P'">
              <xsl:variable name="status_line"><xsl:value-of select="@status"/></xsl:variable>
          <td bgcolor="#00FF00" title="{$status_line}">
          <a target="_blank" href="{@url_link}"><xsl:value-of select="@result"/></a>
          </td>
          </xsl:when>
          <xsl:otherwise>
              <xsl:variable name="status_line"><xsl:value-of select="@status"/></xsl:variable>
          <td bgcolor="#FF0000" title="{$status_line}">
          <a target="_blank" href="{@url_link}"><xsl:value-of select="@result"/></a>
          </td>
          </xsl:otherwise>
      </xsl:choose>
      </xsl:for-each> <!-- step -->
      <!--td>
      <html>
          <a target="_blank" href="./{@report_dir}/afi-gtest.xml">gtest report</a>
      </html>
      </td-->
      <td>
      <html>
          <a target="_blank" href="./{@report_dir}/">logs</a>
      </html>
      </td>

      <td>
          <xsl:value-of select="@container_name"/>
      </td>
      <!--td>
      <html>
          <a href="./{@branch}/rafi.xml">
          <xsl:value-of select="@afi_hash"/>
          </a>
      </html>
      </td-->
      <td>
      <html>
          <!--a href="./{@branch}/rafi.xml"-->
          <xsl:value-of select="@vmx_version"/>
          <!--/a-->
      </html>
      </td>
      <!--td>
      <xsl:variable name="vmx_grn_link">https://svl-svn.juniper.net/viewvc/junos-2009?view=revision<xsl:text disable-output-escaping="yes"><![CDATA[&]]></xsl:text>revision=<xsl:value-of select="@vmx_grn"/></xsl:variable>
      <a href="{$vmx_grn_link}"> <xsl:value-of select="@vmx_grn"/> </a>
      </td-->
      </tr>
      </xsl:for-each><!-- rafi/regression-->

    </fo:block>
    </table>
    </pre>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>

