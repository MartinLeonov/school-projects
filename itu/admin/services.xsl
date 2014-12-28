<?xml version="1.0" encoding="UTF-8"?>
<!-- Edited by XMLSpy® -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
    <head>

    <title>Serivces</title>
    <link href="css/bootstrap.min.css" rel="stylesheet" type="text/css" />
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
    <script>
      $( "button" ).click(function(){
        if($(this).text() == 'More information')
        {
          $(this).text("Less information");
          $(this).parent().children('[class=moreInfo]').show("fast");
        }
        else
        {
          $(this).text("More information");
          $(this).parent().children('[class=moreInfo]').hide("fast");
        }
      });

    </script>
  </head>
  <body>
   <!-- <nav class="navbar navbar-default" role="navigation">
      <div class="container-fluid">
        <div class="navbar-header">
          <a class="navbar-brand" href="#">domain.com</a>
        </div>
        <p class="navbar-text navbar-justified">Webhosting administration</p>
        <p class="navbar-text navbar-right">
          Signed in as <a href="#" class="navbar-link">Mark Otto</a>
          
          <span class="badge">14</span>
          
          <span class="glyphicon glyphicon-log-out" aria-hidden="true"></span>
          
        </p>
      </div>
    </nav>-->
    <div class="container">
    <div class="jumbotron">
          <h2>Services</h2>
          <div class="panel panel-default">
            <div class="panel-body">
      
              <!--<table class="table table-bordered">
                  <tr>
                    <td>Name</td>
                    <td>Expire</td>
                  </tr>
                <xsl:for-each select="services/service">
                <tr>
                  <td><xsl:value-of select="name"/></td>
                  <td><xsl:value-of select="expire"/></td>
                </tr>
                </xsl:for-each>
              </table>-->
              <xsl:for-each select="services/service">
                <div class="panel panel-info">
                  <div class="panel-heading">
                    <h3 class="panel-title"><xsl:value-of select="name"/></h3>
                  </div>
                  <div class="panel-body">
                    <button style="float:right;" type="button" class="btn btn-info">More information</button>
                    Status: 
                    <xsl:if test="status='active'">
                      <span class="label label-success">active</span>
                    </xsl:if>
                    <xsl:if test="status='inactive'">
                      <span class="label label-danger">inactive</span>
                    </xsl:if>
                    <span><br/>Expires: <xsl:value-of select="expire"/></span>
                    
                    <div class="moreInfo" style="display:none;">
                      <span>Cost: <xsl:value-of select="cost"/> $</span><br />
                      <span>Paid: <xsl:value-of select="paid"/></span><br />
                      <span>Start: <xsl:value-of select="start"/></span>
                    </div>
                  </div>
                </div>
              </xsl:for-each>
            </div>
          </div>
          </div>
    </div>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
