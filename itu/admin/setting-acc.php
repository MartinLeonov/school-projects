<?php
if(!isset($_SESSION['domain']))
{
  exit();
}


$webhosting = getWebHosting($_SESSION['id']);
require('db.php');


if(isset($_POST['submit'])){
  require('db.php');
  $addError = false;

  $formNotComplete = false;
  if(!filter_var(trim($_POST['email']), FILTER_VALIDATE_EMAIL) ||
     strlen(trim($_POST['phone'])) < 9 || strlen(trim($_POST['curPass'])) < 4)
    $formNotComplete = true;

  if(!$formNotComplete)
  {
    $email = trim($_POST['email']); 
    $phone = trim($_POST['phone']);
    if(isset($_POST['newPass']) and strlen(trim($_POST['newPass'])) >= 4)
      $newpass = sha1(trim($_POST['newPass']));
    $oldpass = $_POST['curPass'];
    $password = sha1(($oldpass));


  
    $qz = "SELECT ID FROM CLIENT WHERE id='".$_SESSION['id']."' AND password=UNHEX('".$password."')" ;
    $qz = str_replace("\'","",$qz);
    $result = mysqli_query($conn,$qz);
    $badpassword = true;
 
    if ($result->num_rows != 0)
    {
      $badpassword = false;
      $addItemSuccess = updateUserInfo($_SESSION['id'], $email, $phone);
      if($addItemSuccess and isset($newpass))
      {
        $addItemSuccess = updateUserPassword($_SESSION['id'], $newpass);
        if($addItemSuccess)
          $passwordEcho = true;
      }

      
    }

  }

  /*if((isset($addItemSuccess) &&!$addItemSuccess) || $formNotComplete 
    || (isset($badpassword) && $badpassword))
  {
    $POST_email = isset($_POST['email']) ? $_POST['email'] : "";
    $POST_phone = isset($_POST['phone']) ? $_POST['phone'] : "";
    $POST_pass = isset($_POST['newPass']) ? $_POST['newPass'] : "";
    $addError = true;
  }*/

}

$user = getUser($_SESSION['id']);
?>

<div class="col-xs-12 col-sm-6 col-md-8">


<?php 
if (isset($addItemSuccess)) {
if ($addItemSuccess): ?>

<div class="alert alert-success" role="alert">
<strong>Well done!</strong> Your account information was updated successfully.
</div>

<?php else: ?>

<div class="alert alert-danger" role="alert">
<strong>Oh snap!</strong> Your account information couldn't be updated. Here are some possible causes:
<ul><li><strong>You entered wrong information</strong></li>
<li><strong>Something went wrong on our side</strong></li></ul>
</div>

<?php endif; } ?>

<?php 
if (isset($passwordEcho)) {
if ($passwordEcho): ?>

<div class="alert alert-success" role="alert">You got new password.</div>

<?php endif; } ?>

<?php 
if (isset($badpassword)) {
if ($badpassword): ?>

<div class="alert alert-warning" role="alert">You entered wrong current password.</div>

<?php endif; } ?>

<?php 
if (isset($formNotComplete)) {
if ($formNotComplete): ?>

<div class="alert alert-warning" role="alert">All fields except <b>New pasword</b> are reguired. Here are some advices:
  <ul>
    <li><strong>Check if field e-mail addres is correct</strong></li>
    <li><strong>Check if field phone is correct</strong></li>
    <li><strong>Check if you entered the current password</strong></li>
  </ul>
</div>

<?php endif; } ?>

  <ul class="nav nav-pills nav-justified">
    <li role="presentation" class="active"><a href="manage.php?site=setting-acc">Account</a></li>
    <li role="presentation"><a href="manage.php?site=setting-srvc">Services</a></li>
  </ul>
  <h2>Account settings</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <form class="form-horizontal" role="form" method="post" id="addForm">
        <div class="form-group">
          <label for="inputEmail" class="col-sm-2 control-label">Email</label>
          <div class="col-sm-10">
            <input name="email" type="text" class="form-control" id="inputEmail" value="<?php echo($user['email']); ?>">
          </div>
        </div>
        <div class="form-group">
          <label for="inputPhone" class="col-sm-2 control-label">Phone</label>
          <div class="col-sm-10">
            <input name="phone" type="tel" class="form-control" id="inputPhone" value="<?php echo($user['phone']); ?>">
          </div>
        </div>
        <div class="form-group">
          <label for="inputPassword" class="col-sm-2 control-label">New Password</label>
          <div class="col-sm-10">
            <div class="input-group">
              <span class="input-group-addon glyphicon glyphicon-lock"></span>
              <input name="newPass" type="password" class="form-control" id="inputPassword" placeholder="Password">
            </div>
          </div>
        </div>
        <div class="form-group">
          <label for="inputPasswordII" class="col-sm-2 control-label">Current password</label>
          <div class="col-sm-10">
            <div class="input-group">
              <span class="input-group-addon glyphicon glyphicon-lock"></span>
              <input name="curPass" type="password" class="form-control" id="inputPasswordII" placeholder="Password">
            </div>
          </div>
        </div>
        <div class="form-group">
          <div class="col-sm-offset-2 col-sm-10">
            <button name="submit" type="submit" class="btn btn-success"><span class="glyphicon glyphicon-refresh"> Update details</span></button>
          </div>
        </div>
      </form>
    </div>
  </div>
  <h2>Notifications</h2>
  <div class="panel panel-default">
    <div class="panel-body">
    <div id="serviceChangedOn" class="alert alert-success" style="display:none;" role="alert">
      <strong>Well done!</strong> Your notification settings is changed to ON.
    </div>
      <div id="serviceChangedOff" class="alert alert-success" style="display:none;" role="alert">
      <strong>Well done!</strong> Your notification settings is changed to OFF.
    </div>
      <table class="table">
        <tr>
          <td>Service is about to expire</td>
          <td>
            <div class="btn-group input-group" data-toggle="buttons">
              <label class="btn btn-success active">
                <input type="radio" name="options" id="option1" onchange="showDiv(true, this);" checked/>ON</label>
              <label class="btn btn-danger">
                <input type="radio" name="options" id="option3" onchange="showDiv(false, this);" />OFF</label>
            </div>
          </td>
        <tr>
        </tr>
          <td>No space left in database</td>
          <td>
            <div class="btn-group input-group" data-toggle="buttons">
              <label class="btn btn-success active">
                <input type="radio" name="options" id="option1" onchange="showDiv(true, this);"  checked/>ON</label>
              <label class="btn btn-danger">
                <input type="radio" name="options" id="option3" onchange="showDiv(false, this);"/>OFF</label>
            </div>
          </td>
        </tr>
        <tr>
          <td>No space left in mailbox</td>
          <td>
            <div class="btn-group input-group" data-toggle="buttons">
              <label class="btn btn-success active">
                <input type="radio" name="options" id="option1" onchange="showDiv(true, this);" checked/>ON</label>
              <label class="btn btn-danger">
                <input type="radio" name="options" id="option3" onchange="showDiv(false, this);"/>OFF</label>
            </div>
          </td>
        </tr>
      </table>
    </div>
  </div>
</div>

<script>

$(document).ready(function() {
    $('#addForm')
        .bootstrapValidator({
            feedbackIcons: {
                valid: 'glyphicon glyphicon-ok',
                invalid: 'glyphicon glyphicon-remove',
                validating: 'glyphicon glyphicon-refresh'
            },
            fields: {
                email: {
                    validators: {
                        notEmpty: {
                            message: 'The e-mail address is required'
                        },
                        emailAddress: {
                            message: 'The e-mail address is not valid'
                        }
                    }
                },
                phone: {
                    validators: {
                        notEmpty: {
                            message: 'The phone number is required'
                        },
                        phone: {
                            country: 'cz',
                            message: 'The phone number is not valid'
                        }
                    }
                },
                curPass: {
                    validators: {
                        notEmpty: {
                            message: 'The current password is required'
                        }
                    }
                }
            }
        })
       
});

function showDiv(on, trigger)
{
  if(on)
  {
    document.getElementById('serviceChangedOn').style.display = 'block';
    document.getElementById('serviceChangedOff').style.display = 'none';
  }
  else
  {
    document.getElementById('serviceChangedOff').style.display = 'block';
    document.getElementById('serviceChangedOn').style.display = 'none';
  }
}

</script>