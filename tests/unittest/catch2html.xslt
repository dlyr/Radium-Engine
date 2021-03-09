<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
        <xsl:output method="html" version="4.0" encoding="UTF-8" indent="yes"/>
        <xsl:template match="/">
        <html>
        <head>
                <title>Unit Test Results</title>
                <style type="text/css">
                        td#passed {
                                color: green;
                                font-weight: bold;
                        }
                        td#failed {
                                color: red;
                                font-weight: bold;
                        }
                        <!-- borrowod from here http://red-team-design.com/practical-css3-tables-with-rounded-corners/ -->
                        body {
                                width: 80%;
                                margin: 40px auto;
                                font-family: 'trebuchet MS', 'Lucida sans', Arial;
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
                                -moz-border-radius: 6px;
                                -webkit-border-radius: 6px;
                                border-radius: 6px;
                                -webkit-box-shadow: 0 1px 1px #ccc; 
                                -moz-box-shadow: 0 1px 1px #ccc; 
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
                                background-image: -webkit-gradient(linear, left top, left bottom, from(#ebf3fc), to(#dce9f9));
                                background-image: -webkit-linear-gradient(top, #ebf3fc, #dce9f9);
                                background-image:    -moz-linear-gradient(top, #ebf3fc, #dce9f9);
                                background-image:     -ms-linear-gradient(top, #ebf3fc, #dce9f9);
                                background-image:      -o-linear-gradient(top, #ebf3fc, #dce9f9);
                                background-image:         linear-gradient(top, #ebf3fc, #dce9f9);
                                -webkit-box-shadow: 0 1px 0 rgba(255,255,255,.8) inset; 
                                -moz-box-shadow:0 1px 0 rgba(255,255,255,.8) inset;  
                                box-shadow: 0 1px 0 rgba(255,255,255,.8) inset;        
                                border-top: none;
                                text-shadow: 0 1px 0 rgba(255,255,255,.5); 
                        }
                        .bordered td:first-child, .bordered th:first-child {
                                border-left: none;
                        }
                        .bordered th:first-child {
                                -moz-border-radius: 6px 0 0 0;
                                -webkit-border-radius: 6px 0 0 0;
                                border-radius: 6px 0 0 0;
                        }
                        .bordered th:last-child {
                                -moz-border-radius: 0 6px 0 0;
                                -webkit-border-radius: 0 6px 0 0;
                                border-radius: 0 6px 0 0;
                        }
                        .bordered th:only-child{
                                -moz-border-radius: 6px 6px 0 0;
                                -webkit-border-radius: 6px 6px 0 0;
                                border-radius: 6px 6px 0 0;
                        }
                        .bordered tr:last-child td:first-child {
                                -moz-border-radius: 0 0 0 6px;
                                -webkit-border-radius: 0 0 0 6px;
                                border-radius: 0 0 0 6px;
                        }
                        .bordered tr:last-child td:last-child {
                                -moz-border-radius: 0 0 6px 0;
                                -webkit-border-radius: 0 0 6px 0;
                                border-radius: 0 0 6px 0;
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
                <tr><th style="width:30%">SectionName(result:<xsl:value-of select="OverallResult/@success"/>) </th>
                        <th>Successes</th>
                        <th>Failures</th>
                </tr>
                <xsl:for-each select="Section">
                <tr>
                        <td><xsl:value-of select="@name"/></td>
                        <td><xsl:value-of select="OverallResults/@successes"/></td>
                        <td><xsl:value-of select="OverallResults/@failures"/></td>
                </tr>
                </xsl:for-each>
        </table>
</xsl:template>
</xsl:stylesheet>