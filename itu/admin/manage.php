<?php
error_reporting(-1);
ini_set('display_errors', 'On');

if (!isset($_SESSION)) {
    session_start();
}

if (!isset($_SESSION['login']) || $_SESSION['login'] !== true)
{
  header('Location: index.php?loginRequired&back='.$_GET['site']);
  exit();
}

if(isset($_POST['DomainSelect']))
{
  $_SESSION['domain'] = $_POST['DomainSelect'];
  $_SESSION['active_domain'] = $_POST['DomainSelect'];
}

include('functions.php');
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
<html lang="en" ng-app="email">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Webhosting administration</title>
    <link href="css/bootstrap.min.css" rel="stylesheet">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/jquery.bootstrapvalidator/0.5.3/css/bootstrapValidator.min.css"/>
    <script type="text/javascript" src="//cdnjs.cloudflare.com/ajax/libs/jquery.bootstrapvalidator/0.5.3/js/bootstrapValidator.min.js"></script>
    <script src="js/chart.js"></script>
    <style type="text/css">
      .bs-sidebar .nav > .active > ul {
          display: block;
          margin-bottom: 8px;
      }
      .canvas-holder-half {
          width: 50%;
          float: left;
      }
    </style>
  </head>
  <body>
    <nav class="navbar navbar-default" role="navigation">
      <div class="container-fluid">
        <div class="navbar-header">
          <p class="navbar-text navbar-justified">Active domain:</p>

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

        </div>
        <p class="navbar-text navbar-justified"><a class="navbar-link" href="dashboard.php">Webhosting administration</a></p>
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
        <div class="col-xs-6 col-md-4">
          <ul class="nav nav-pills nav-stacked">
            <li role="presentation" 
              <?php if(isset($_GET['site']) and $_GET['site'] == 'ftp'): ?> class="active" <?php endif; ?>>
              <a href="manage.php?site=ftp">FTP</a>
            </li>
            <li role="presentation" 
              <?php if(isset($_GET['site']) and $_GET['site'] == 'email'): ?>class="active" <?php endif; ?>>
              <a href="manage.php?site=email">Email</a>
            </li>
            <li role="presentation" 
              <?php if(isset($_GET['site']) and $_GET['site'] == 'sql'): ?>class="active" <?php endif; ?>>
              <a href="manage.php?site=sql">MySQL</a>
            </li>
            <li role="presentation" 
              <?php if(isset($_GET['site']) and $_GET['site'] == 'dns'): ?>class="active" <?php endif; ?>>
              <a href="manage.php?site=dns">DNS</a>
            </li>
            <li role="presentation" 
              <?php if(isset($_GET['site']) and $_GET['site'] == 'stats'): ?>class="active" <?php endif; ?>>
              <a href="manage.php?site=stats">Stats</a>
            </li>

            <li style="padding-top:20px;" role="presentation" 
              <?php if(isset($_GET['site']) and ($_GET['site'] == 'setting-acc' or $_GET['site'] == 'setting-srvc')): ?>class="active" <?php endif; ?>>
              <a href="manage.php?site=setting-acc">Accounts setting</a>
            </li>
          </ul>
        </div>
        
        <?php
          if(!isset($_GET['site']) || !file_exists($_GET['site'] . ".php"))
            header('Location: dashboard.php');

          require($_GET['site'] . ".php");
        ?>

      </div>
    </div>
  </body>
</html>
