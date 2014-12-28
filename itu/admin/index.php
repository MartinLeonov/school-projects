<?php
error_reporting(-1);
ini_set('display_errors', 'On');


if (!isset($_SESSION)) {
    session_start();
}

if (isset($_SESSION['login']) and $_SESSION['login']) {
  header('LOCATION:dashboard.php'); exit();
}

$email = $password = $error = '';

if(isset($_POST['submit'])){
  require('db.php');
  $email = ($_POST['email']); $password = sha1(($_POST['password']));


  
  $qz = "SELECT ID, name, surname FROM CLIENT WHERE email='".$email."' AND password=UNHEX('".$password."')" ;
  $qz = str_replace("\'","",$qz);
  $result = mysqli_query($conn,$qz);


  if ($result->num_rows != 0){
    $row = $result->fetch_assoc();
    $_SESSION['login'] = true; 
    $_SESSION['name'] = $row['name']; 
    $_SESSION['surname'] = $row['surname']; 
    $_SESSION['id'] = $row['ID']; 

  
    if($_POST['backSite'] !== "")
      $site = "LOCATION:manage.php?&site=".$_POST['backSite'];
    else
      $site = "LOCATION:dashboard.php";


    header('Cache-control: private'); 
    header($site); exit();
  }
  else {
    $_SESSION['POST'] = $email;
    header('LOCATION:index.php'); die();
    $error = "Invalid combination of email and password";

  }
  $result->close();
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
    <style>
      body {
        padding-bottom: 40px;
        background-image: url('background1.png');
        background-size: cover;
      }
      .form-signin {
        background-color: #eee;
        max-width: 330px;
        padding: 15px;
        margin: 0 auto;
        border-radius: 10px;
      }
      .form-signin .form-signin-heading,
      .form-signin .checkbox {
        margin-bottom: 10px;
      }
      .form-signin .checkbox {
        font-weight: normal;
      }
      .form-signin .form-control {
        position: relative;
        height: auto;
        -webkit-box-sizing: border-box;
           -moz-box-sizing: border-box;
                box-sizing: border-box;
        padding: 10px;
        font-size: 16px;
      }
      .form-signin .form-control:focus {
        z-index: 2;
      }
      .form-signin input[type="email"] {
        margin-bottom: -1px;
        border-bottom-right-radius: 0;
        border-bottom-left-radius: 0;
      }
      .form-signin input[type="password"] {
        margin-bottom: 10px;
        border-top-left-radius: 0;
        border-top-right-radius: 0;
      }
      .jumbotron {
        background-color: transparent;
      }
      .page-header {
        border-bottom: 1px solid #999;
      }
    </style>
  </head>
  <body>
    <nav class="navbar navbar-default" role="navigation">
      <div class="container-fluid">
        <p class="navbar-text navbar-justified">Webhosting administration</p>
      </div>
    </nav>
    <div class="container">
      <div class="page-header">
        <h1>Webhosting administration</h1>
      </div>
    </div>
    <div class="jumbotron">
        <?php if (isset($_SESSION['POST'])): ?>
    <div class="alert alert-danger" role="alert"><strong>Oh snap!</strong> You entered wrong e-mail address or password. Here are some advices
      <ul>
        <li><strong>Check if you entered your e-mail address and password correctly.</strong></li>
        <li><strong>If problems continue, please send e-mail to our support: support@bestWebHosting.com</strong></li>
      </ul>
    </div>
    <?php endif; ?>
    <?php if (isset($_GET['loginRequired'])): ?>
    <div class="alert alert-warning" role="alert"><strong>Warning!</strong> You have to login before you're able to manage your webhosting.</div>
    <?php endif; ?>
      <div class="container">
        <form class="form-signin" role="form" method="post">
          <h2 class="form-signin-heading">Please sign in</h2>
          <input name="backSite" type="hidden" value="<?php if (isset($_GET['back'])) echo($_GET['back']); ?>"/>
          <label for="inputEmail" class="sr-only">Email address</label>
          <input name="email" type="email" id="inputEmail" class="form-control" placeholder="Email address" required autofocus
          <?php if (isset($_SESSION['POST'])) echo('value="'.$_SESSION['POST'].'"'); ?> >
          <label for="inputPassword" class="sr-only">Password</label>
          <input name="password" type="password" id="inputPassword" class="form-control" placeholder="Password" required>
          <button name="submit" class="btn btn-lg btn-primary btn-block" type="submit">Sign in</button>
        </form>
      </div>
    </div>
  </body>
</html>

<?php
unset($_SESSION['POST']);
?>
