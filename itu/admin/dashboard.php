<?php
error_reporting(-1);
ini_set('display_errors', 'On');

if (!isset($_SESSION)) {
    session_start();
}

if (!isset($_SESSION['login']) || $_SESSION['login'] !== true)
{
  header('Location: index.php?loginRequired');
}

include('functions.php');
if(isset($_POST['DomainSelect']))
{
  $_SESSION['domain'] = $_POST['DomainSelect'];
  $_SESSION['active_domain'] = $_POST['DomainSelect'];
}

if(!isset($_SESSION['active_domain']))
{
  $domain = getDefaultDomain($_SESSION['id']);

  if($domain == null)
    $_SESSION['domain']= 'NOT EXISTS';
  else {
    $_SESSION['domain'] = $domain['name'] . '.' . $domain['tld'];
  }
}


?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Webhosting administration</title>
    <link href="css/bootstrap.min.css" rel="stylesheet">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
  </head>
  <body>
    <nav class="navbar navbar-default" role="navigation">
      <div class="container-fluid">
        <div class="navbar-header">
          <p class="navbar-text navbar-justified">Active domain:</p>
        </div>
         <form method="post" role="form" style="display:inline-block;margin:8px;float:left;" >
          <select name="DomainSelect" class="form-control" onchange="this.form.submit()">
          <?php
            $domains = getDomains($_SESSION['id']);

            foreach($domains as $domain)
            {
          ?>
              <option value="<?php echo($domain['fullname']); ?>"
              <?php if ($domain['fullname'] ==  $_SESSION['domain']) echo('selected="true"'); ?> ><?php echo($domain['fullname']); ?></option>
          <?php
            } 
          ?>
          </select>
          </form>
        <p class="navbar-text navbar-justified">Webhosting administration</p>
        <p class="navbar-text navbar-right">
          Signed in as <a href="manage.php?site=setting-acc" class="navbar-link"><?php echo($_SESSION['name'] . ' ' . $_SESSION['surname']); ?></a>
          &nbsp;&nbsp;
          <span class="badge">14</span>
          &nbsp;&nbsp;
          <a href="logout.php"><span class="glyphicon glyphicon-log-out" aria-hidden="true"></span></a>
          &nbsp;&nbsp;
        </p>
      </div>
    </nav>
    <div class="container">
      <div class="page-header">
        <h1>Webhosting administration <small><?php echo($_SESSION['domain']);?></small></h1>
      </div>
    </div>
    <div class="jumbotron">
      <div class="container">
        <div class="row">
          <div class="col-md-4 col-sm-6">
            <div class="thumbnail">
              <div class="caption">
                <h3>FTP accounts</h3>
                <p><small>manage FTP accounts, manage files & folders</small></p>
                <p><a href="manage.php?site=ftp" class="btn btn-primary" role="button">Open</a><!-- <a href="#" class="btn btn-default" role="button">Button</a>--></p>
              </div>
            </div>
          </div>
          <div class="col-md-4 col-sm-6">
            <div class="thumbnail">
              <div class="caption">
                <h3>Email boxes</h3>
                <p><small>setup email boxes, browse emails, set email notification</small></p>
                <p><a href="manage.php?site=email" class="btn btn-primary" role="button">Open</a></p>
              </div>
            </div>
          </div>
          <div class="col-md-4 col-sm-6">
            <div class="thumbnail">
              <div class="caption">
                <h3>MySQL Database</h3>
                <p><small>manage MySQL databases, browse curent databases</small></p>
                <p><a href="manage.php?site=sql" class="btn btn-primary" role="button">Open</a></p>
              </div>
            </div>
          </div>
          <div class="col-md-4 col-sm-6">
            <div class="thumbnail">
              <div class="caption">
                <h3>DNS records</h3>
                <p><small>modify details on registered domain, browse DNS records</small></p>
                <p><a href="manage.php?site=dns" class="btn btn-primary" role="button">Open</a></p>
              </div>
            </div>
          </div>
          <div class="col-md-4 col-sm-6">
            <div class="thumbnail">
              <div class="caption">
                <h3>Stats</h3>
                <p><small>used space, visits counter, users informations</small></p>
                <p><a href="manage.php?site=stats" class="btn btn-primary" role="button">Open</a></p>
              </div>
            </div>
          </div>
          <div class="col-md-4 col-sm-6">
            <div class="thumbnail">
              <div class="caption">
                <h3>Account settings</h3>
                <p><small>change password, contact details, notifications</small></p>
                <p><a href="manage.php?site=setting-acc" class="btn btn-primary" role="button">Open</a></p>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </body>
</html>