<?xml version="1.0" encoding="UTF-8" ?>

<!-- https://www.programmersought.com/article/21196208617/ -->
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
        <xsl:output method="html" version="4.0" encoding="UTF-8" indent="yes"/>
        <xsl:template match="/">
        <html>
        <head>
                <title>Unit Test Results</title>
                <style type="text/css">
                        td.passed {
                                color: green;
                                font-weight: bold;
                        }
                        td.failed {
                                background: red;
                                font-weight: bold;
                        }
                        <!-- borrowod from here http://red-team-design.com/practical-css3-tables-with-rounded-corners/ -->
                        body {
                                width: 80%;
                                margin: 40px auto;
                                font-size: 14px;
                                color: #444;
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
        <xsl:apply-templates/>
</xsl:template>
 
<xsl:template match="TestCase">
        <h2><xsl:value-of select="@name"/></h2>
 
        <table class="bordered">
          <tr><th style="width:30%">SectionName
	  <xsl:choose>
	    <xsl:when test="OverallResult/@success = 'true'">
	      SUCCES
	    </xsl:when>
	    <xsl:otherwise>
              FAILURE         
	    </xsl:otherwise>
	  </xsl:choose>

	</th>
                        <th>Successes</th>
                        <th>Failures</th>
                </tr>
                <xsl:for-each select="Section">
                <tr>
                        <td><xsl:value-of select="@name"/></td>
                        <td><xsl:value-of select="OverallResults/@successes"/></td>
			<xsl:choose>
			  <xsl:when test="OverallResults/@failures>0">
                            <td class="failed"><xsl:value-of select="OverallResults/@failures"/></td>
			  </xsl:when>
			  <xsl:otherwise>
                            <td><xsl:value-of select="OverallResults/@failures"/></td>
			  </xsl:otherwise>
			</xsl:choose>
                </tr>
                </xsl:for-each>
        </table>
</xsl:template>
</xsl:stylesheet>
