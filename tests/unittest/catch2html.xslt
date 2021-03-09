<?xml version="1.0" encoding="UTF-8" ?>

<!-- https://www.programmersought.com/article/21196208617/ -->
<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" version="4.0" encoding="UTF-8" indent="yes"/>
  <xsl:template match="/">
    <html>
      <head>
        <title>Unit Test Results</title>
	<link rel="stylesheet"
          href="https://fonts.googleapis.com/css?family=Roboto" />
        <style type="text/css">
	  .passed {
          color: green;
          font-weight: bold;
          }
          .failed {
          color: red;
          font-weight: bold;
          }
          td.passed {
          color: green;
          font-weight: bold;
          }
          td.failed {
	  color: black;
          background: red;
          font-weight: bold;
          }
          <!-- borrowod from here http://red-team-design.com/practical-css3-tables-with-rounded-corners/ -->
          body {
          width: 80%;
          margin: 40px auto;
          font-size: 14px;
          color: #444;
	  font-family: 'Roboto', sans;
          }
          table {
          *border-collapse: collapse; /* IE7 and lower */
          border-spacing: 0;
          width: 100%;
          }
          .bordered {
          border: solid #ccc 1px;
          box-shadow: 0 1px 1px #ccc;
          }
          .bordered tr:hover {
          background: #fbf8e9;
          -o-transition: all 0.1s ease-in-out;
          -webkit-transition: all 0.1s ease-in-out;
          -moz-transition: all 0.1s ease-in-out;
          -ms-transition: all 0.1s ease-in-out;
          transition: all 0.1s ease-in-out;
          }
          .bordered td, .bordered th {
          border-left: 1px solid #ccc;
          border-top: 1px solid #ccc;
          padding: 10px;
          text-align: left;
          }
          .bordered th {
          background-color: #dce9f9;
          -webkit-box-shadow: 0 1px 0 rgba(255,255,255,.8) inset;
          -moz-box-shadow:0 1px 0 rgba(255,255,255,.8) inset;
          box-shadow: 0 1px 0 rgba(255,255,255,.8) inset;
          border-top: none;
          text-shadow: 0 1px 0 rgba(255,255,255,.5);
          }
          .bordered td:first-child, .bordered th:first-child {
          border-left: none;
          }
	  .bordered tr.section {
	  text-align: center;
          background-color: #f0f0f0;
          -webkit-box-shadow: 0 1px 0 rgba(255,255,255,.8) inset;
          -moz-box-shadow:0 1px 0 rgba(255,255,255,.8) inset;
          box-shadow: 0 1px 0 rgba(255,255,255,.8) inset;
          border-top: none;
          text-shadow: 0 1px 0 rgba(255,255,255,.5);
          }
	  .bordered .result  {
	  text-align: center;
	  }

	  .bordered .count {
	  text-align: right;
	  }
        </style>
      </head>
      <body>
        <xsl:apply-templates/>
      </body>
    </html>

  </xsl:template>

  <xsl:template match="Group">
    <h1>Unit Test Run <xsl:value-of select="@timestamp"/></h1>
    <p>
      Executed <b><xsl:value-of select="count(TestCase)"/></b> test cases
      <b><xsl:value-of select="OverallResults/@successes"/></b> test cases success.
      <b><xsl:value-of select="OverallResults/@failures"/></b> test cases failed.
    </p>

    <table class="bordered">
      <tr><th> Test/section name</th>
      <th class="result">Successes</th>
      <th class="result">Failures</th>
      </tr>

      <xsl:for-each select="TestCase">
        <tr class="section"><td>  <xsl:value-of select="@name"/> </td>
	<td colspan="2" class="result">	  <xsl:choose>

	  <xsl:when test="OverallResult/@success = 'true'">
	    <span class="passed">SUCCESS</span>
	  </xsl:when>
	  <xsl:otherwise>
            <span class="failed">FAILED</span>
	  </xsl:otherwise>
	</xsl:choose>
	</td>
	</tr>

        <xsl:for-each select="Section">
          <tr>
            <td><xsl:value-of select="@name"/></td>
            <td  class="count"><xsl:value-of select="OverallResults/@successes"/></td>
	    <xsl:choose>
	      <xsl:when test="OverallResults/@failures>0">
                <td class="failed count"><xsl:value-of select="OverallResults/@failures"/></td>
	      </xsl:when>
	      <xsl:otherwise>
                <td class="count"><xsl:value-of select="OverallResults/@failures"/></td>
	      </xsl:otherwise>
	    </xsl:choose>
          </tr>
        </xsl:for-each>
      </xsl:for-each>


    </table>
  </xsl:template>
</xsl:stylesheet>
