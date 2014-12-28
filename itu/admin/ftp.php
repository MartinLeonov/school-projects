<?php
if(!isset($_SESSION['domain']))
{
  exit();
}

$webhosting = getWebHosting($_SESSION['id']);

if(isset($_POST['addSubmit'])){
  require('db.php');

  $addError = false;

  $formNotComplete = false;
  if(strlen(trim($_POST['addEmail'])) < 2 || strlen(trim($_POST['addLogin'])) < 2 
    || !filter_var(trim($_POST['addEmail']), FILTER_VALIDATE_EMAIL))
    $formNotComplete = true;

  if(!$formNotComplete)
  {
    $pass = generatePassword();
    $password = sha1($pass);
    $login = trim($_POST['addLogin']); 
    $path = trim($_POST['addPath']);
    $mail = $_POST['addEmail'];
    $ftp_server = "ftp.".$_SESSION['domain'];

    if($path == "")
      $path = "\\home";

    $addItemSuccess = addFtpAccount($webhosting, $login, $password, $path, $ftp_server);
  }

  if((isset($addItemSuccess) &&!$addItemSuccess) || $formNotComplete)
  {
    $POST_login = isset($_POST['addLogin']) ? $_POST['addLogin'] : "";
    $POST_path = isset($_POST['addPath']) ? $_POST['addPath'] : "";
    $POST_mail = isset($_POST['addEmail']) ? $_POST['addEmail'] : "";
    $addError = true;
  }
  else
  {
    mail($mail,"Registrovani FTP uctu", "Byl vam vytvoren FTP ucet na serveru: " . $ftp_server . 
      " s loginem " . $login . " a s heslem " . $pass . " ");

  }
}

if(isset($_POST['delSubmit'])){
  require('db.php');

  $login = ($_POST['delLogin']);

  $delItemSuccess = deleteFtpAccount($webhosting, $login);
}

if(isset($_POST['acSubmit']) or isset($_POST['deSubmit'])){
  require('db.php');

  $login = ($_POST['changeLogin']);
  $status = isset($_POST['acSubmit']) ? 0 : 1;

  $changeItemSuccess = changeFtpAccount($webhosting, $login, $status);

}

?>
        <div class="col-xs-12 col-sm-6 col-md-8">

<?php 
if (isset($addItemSuccess)) {
if ($addItemSuccess): ?>

<div class="alert alert-success" role="alert">
<strong>Well done!</strong>FTP account "<?php echo($login); ?>" was added successfully and password was send to "<?php echo($mail); ?>".
</div>

<?php else: ?>

<div class="alert alert-danger" role="alert">
<strong>Oh snap!</strong> FTP account couldn't be added. Here are some possible causes:
<ul><li><strong>FTP account with the same name already exists</strong></li></ul>
</div>

<?php endif; } ?>

<?php
if (isset($changeItemSuccess)) {
if ($changeItemSuccess): ?>

<div class="alert alert-success" role="alert">
<strong>Well done!</strong> FTP account "<?php echo($login); ?>" was edited successfully to "<?php echo($StatusDict[$status]); ?>".
</div>

<?php else: ?>

<div class="alert alert-danger" role="alert">
<strong>Oh snap!</strong> FTP account couldn't be edited. <?php echo($login); ?>. Here are some possible causes:
<ul><li>FTP account no longer exists</li></ul>
</div>

<?php endif; } ?>

<?php 
if (isset($delItemSuccess)) {
if ($delItemSuccess): ?>

<div class="alert alert-success" role="alert"><strong>Well done!</strong> FTP account "<?php echo($login); ?>" was deleted successfully.</div>

<?php else: ?>

<div class="alert alert-danger" role="alert"><strong>Oh snap!</strong> FTP account "<?php echo($login); ?>" couldn't be deleted.</div>

<?php endif; } ?>

<?php 
if (isset($formNotComplete)) {
if ($formNotComplete): ?>

<div class="alert alert-warning" role="alert">All fields except <b>Path</b> are reguired. Here are some advices:
  <ul>
    <li><strong>Check if field e-mail addres is correct</strong></li>
    <li><strong>Check if field login have at least 2 characters</strong></li>
    <li><strong>Check if field password have at least 4 characters</strong></li>
  </ul>
</div>

<?php endif; } ?>

          <h2>New FTP account</h2>
          <div class="panel panel-default">
  <div class="panel-body">
          <form class="form-horizontal" role="form" method="post" id="addForm">
            <div class="form-group">
              <label for="inputName" class="col-sm-2 control-label">Login name</label>
              <div class="col-sm-10">
                <input name="addLogin" type="text" class="form-control" id="inputName" placeholder="FTP account name"
                <?php if (isset($addError) and $addError) echo('value="'.$POST_login.'"'); ?> >
              </div>
            </div>
            <div class="form-group">
              <label for="inputPath" class="col-sm-2 control-label">Path</label>
              <div class="col-sm-10">
                <input name="addPath" type="text" class="form-control" id="inputPath" placeholder="Path to folder"
                <?php if (isset($addError) and $addError) echo('value="'.$POST_path.'"'); ?> >
              </div>
            </div>
            <div class="form-group">
              <label for="inputEmail" class="col-sm-2 control-label">Email</label>
              <div class="col-sm-10">
                <input name="addEmail" type="email" class="form-control" id="inputEmail" placeholder="Users email"
                <?php if (isset($addError) and $addError) echo('value="'.$POST_mail.'"'); ?> >
              </div>
            </div>
            <div class="form-group">
              <div class="col-sm-offset-2 col-sm-10">
                <button name="addSubmit" type="submit" class="btn btn-success"><span class="glyphicon glyphicon-user"> Add account</span></button>
              </div>
            </div>
          </form>
          </div>
          </div>
          <h2>Manage Existing accounts</h2>
                    <div class="panel panel-default">
  <div class="panel-body">
            <table class="table table-bordered">
              <tr>
                <td>Login</td>
                <td>Status</td>
                <td></td>
                <td></td>
              </tr>
              <?php 
              $accounts = getFtpAccounts($webhosting);

              foreach($accounts as $account)
              {
              ?>

              <tr>
                <td><strong><?php echo($account['login']); ?></strong></td>
                <td><?php echo($StatusDict[$account['status']]); ?></td>
                <?php if ($account['status'] == 0):  ?>
                  <td>
                    <form class="form-horizontal" role="form" method="post">
                      <input name="changeLogin" type="hidden" value="<?php echo($account['login']); ?>"/>
                      <button name="deSubmit" type="submit" class="btn btn-warning"><span class="glyphicon  glyphicon-minus" aria-hidden="true"></span> Deactivate</button>
                    </form>
                  </td>
                <?php else: ?>
                  <td>
                    <form class="form-horizontal" role="form" method="post">
                      <input name="changeLogin" type="hidden" value="<?php echo($account['login']); ?>"/>
                      <button name="acSubmit" type="submit" class="btn btn-success"><span class="glyphicon  glyphicon-plus" aria-hidden="true"></span> Activate</button></td>
                    </form>
                <?php endif; ?>
                <td>
                  <form class="form-horizontal" role="form" method="post">
                    <input name="delLogin" type="hidden" value="<?php echo($account['login']); ?>"/>
                    <button name="delSubmit" type="submit" class="btn btn-danger"><span class="glyphicon  glyphicon-remove" aria-hidden="true"></span> Delete</button></td>
                  </form>
              </tr>

              <?php
              }
              ?>
            
            </table>
            </div>
            </div>
            <h2>FTP details</h2>
                      <div class="panel panel-default">
  <div class="panel-body">
              <table class="table">
                <tr>
                  <td><strong>Hostname</strong></td>
                  <td><?php echo($_SESSION['domain']);?></td>
                </tr>
                <tr>
                  <td><strong>FTP server</strong></td>
                  <td>ftp.<?php echo($_SESSION['domain']);?></td>
                </tr>
                <tr>
                  <td><strong>Port</strong></td>
                  <td>21</td>
                </tr>
              </table>
            </div>
            </div>
          <h2>WebFTP</h2>
                                <div class="panel panel-default">
  <div class="panel-body">
          <button type="button" class="btn btn-link"><span class="glyphicon glyphicon-new-window"> Go to WebFTP</span></button>
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
                addLogin: {
                    validators: {
                        notEmpty: {
                            message: 'The login is required'
                        },
                        stringLength: {
                            message: 'Login have to contain at least 2 characters',
                            min: 2
                        }
                    }
                },
                addEmail: {
                    validators: {
                        notEmpty: {
                            message: 'The e-mail address is required'
                        },
                        emailAddress: {
                            message: 'The e-mail address is not valid'
                        }
                    }
                },
            }
        })
        .on('click', 'button[data-toggle]', function() {
            var $target = $($(this).attr('data-toggle'));
            $target.toggle();
            if (!$target.is(':visible')) {
                // Enable the submit buttons in case additional fields are not valid
                $('#togglingForm').data('bootstrapValidator').disableSubmitButtons(false);
            }
        });
});
</script>