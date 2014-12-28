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
  if(!isset($_POST['type']) || strlen(trim($_POST['alias'])) < 2 ||
    strlen(trim($_POST['password'])) < 4)
    $formNotComplete = true;

  if(!$formNotComplete)
  {
    $alias = ($_POST['alias']); $password = sha1(($_POST['password']));
    $type = ($_POST['type']);

    $addItemSuccess = addEmailAdress($webhosting, $alias, $password, $type);
  }

  if((isset($addItemSuccess) &&!$addItemSuccess) || $formNotComplete)
  {
    $POST_alias = isset($_POST['alias']) ? $_POST['alias'] : "";
    $POST_type = isset($_POST['type']) ? $_POST['type'] : "";
    $addError = true;
  }
}

if(isset($_POST['delSubmit'])){
  require('db.php');

  $alias = ($_POST['delAlias']);

  $delItemSuccess = deleteEmailAdress($webhosting, $alias);
}


if(isset($_POST['editSubmit'])){
  require('db.php');

  $old_alias = ($_POST['editOldAlias']);
  $alias = ($_POST['editAlias']); $password = sha1(($_POST['editPassword']));
  $type = ($_POST['editType']);

  if(!isset($_POST['editType']) || strlen(trim($_POST['editAlias'])) < 2 ||
    strlen(trim($_POST['editPassword'])) < 4)
    $editItemSuccess = false;
  else
    $editItemSuccess = updateEmailAdress($webhosting, $old_alias, $alias, $password, $type);
}

?>

<div class="col-xs-12 col-sm-6 col-md-8" ng-controller="EmailController">
<?php 
if (isset($addItemSuccess)) {
if ($addItemSuccess): ?>

<div class="alert alert-success" role="alert">
<strong>Well done!</strong> E-mail address "<?php echo($alias . "@" . $_SESSION['domain']); ?>" was added successfully.
</div>

<?php else: ?>

<div class="alert alert-danger" role="alert"><strong>Oh snap!</strong> E-mail address couldn't be added. Here are some possible causes:
<ul><li><strong>E-mail address with same alias already exists</strong></li></ul>
</div>

<?php endif; } ?>

<?php 
if (isset($delItemSuccess)) {
if ($delItemSuccess): ?>

<div class="alert alert-success" role="alert">
<strong>Well done!</strong> E-mail address "<?php echo($alias . "@" . $_SESSION['domain']); ?>" was deleted successfully.
</div>

<?php else: ?>

<div class="alert alert-danger" role="alert">
<strong>Oh snap!</strong> E-mail address "<?php echo($alias . "@" . $_SESSION['domain']); ?>" couldn't be deleted.
</div>

<?php endif; } ?>

<?php 
if (isset($formNotComplete)) {
if ($formNotComplete): ?>

<div class="alert alert-warning" role="alert">All fields are reguired. Here are some advices:
  <ul>
  <?php if ($POST_type !== ""): ?>
    <li><strong>Check if field address have at least 2 characters</strong></li>
    <li><strong>Check if field password have at least 4 characters</strong></li>
  <?php else: ?>
    <li><strong>Check if you choosed type of new e-mail address</strong></li>
  <?php endif; ?>
  </ul>
</div>

<?php endif; } ?>

<?php 
if (isset($editItemSuccess)) {
if ($editItemSuccess): ?>

  <?php if ($old_alias == $alias): ?>
    <div class="alert alert-success" role="alert">
      <strong>Well done!</strong>E-mail address "<?php echo($alias . "@" . $_SESSION['domain']); ?>" was edited successfully.
    </div>
  <?php else: ?>
    <div class="alert alert-success" role="alert">
      <strong>Well done!</strong>E-mail address "<?php echo($old_alias . "@" . $_SESSION['domain']); ?>" was edited successfully to "<?php echo($alias . "@" . $_SESSION['domain']); ?>"
    </div>
  <?php endif;  ?>
<?php else: ?>

<div class="alert alert-danger" role="alert">
<strong>Oh snap!</strong> E-mail address "<?php echo($alias . "@" . $_SESSION['domain']); ?>" couldn't be edited. Here are some adiveces:
<ul>
    <li>Check if field address have at least 2 characters</li>
    <li>Check if field password have at least 4 characters</li>
    <li>Check if you choosed type of new e-mail address</li>
</ul>
</div>

<?php endif; } ?>
  <h2>New Mailbox</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <form class="form-horizontal" role="form" method="post" id="addForm">
        <div class="form-group">
          <label for="inputAddress" class="col-sm-2 control-label">Address</label>
          <div class="col-sm-10 emailInputGroup">
            <div class="input-group">
              <input data-bv-remote-delay="10000" name="alias" type="text" class="form-control" id="inputAddress" placeholder="Mailbox address"
              <?php if (isset($addError) and !$addError) echo('value="'.$POST_alias.'"'); ?> >
              <span class="input-group-addon">@<?php echo($_SESSION['domain']);?></span>
            </div>
          </div>
        </div>
        <div class="form-group">
          <label class="col-sm-2 control-label">Type</label>
          <div class="col-sm-10 typelInputGroup">
            <div class="btn-group input-group btn-group-justified" data-toggle="buttons">
              <label class="btn btn-primary <?php if (isset($addItemSuccess) and !$addItemSuccess and $POST_type === "0") echo('active'); ?> ">
                <input type="radio" name="type" value="0" <?php if (isset($addError) and $addError and $POST_type === "0") echo('checked'); ?>/>POP3</label>
              <label class="btn btn-primary <?php if (isset($addItemSuccess) and !$addItemSuccess and $POST_type === "1") echo('active'); ?> ">
                <input type="radio" name="type" value="1" <?php if (isset($addError) and $addError and $POST_type === "1") echo('checked'); ?>/>IMAP</label>
              <label class="btn btn-primary <?php if (isset($addItemSuccess) and !$addItemSuccess and $POST_type === "2") echo('active'); ?> ">
                <input type="radio" name="type" value="2" <?php if (isset($addError) and $addError and $POST_type === "2") echo('checked'); ?>/>POP3 & IMAP</label>
            </div>
          </div>
        </div>
        <div class="form-group">
          <label for="inputPassword" class="col-sm-2 control-label">Password</label>
          <div class="col-sm-10">
            <div class="input-group">
              <span class="input-group-addon glyphicon glyphicon-lock"></span>
              <input name="password" type="password" class="form-control" id="inputPassword" placeholder="Password">
            </div>
          </div>
        </div>
        <div class="form-group">
          <div class="col-sm-offset-2 col-sm-10">
            <button name="addSubmit" type="submit" class="btn btn-success"><span class="glyphicon glyphicon-inbox"> Create mailbox</span></button>
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
          <td>Alias</td>
          <td>Type</td>
          <td>Usage</td>
          <td></td>
          <td></td>
        </tr>
        <?php 
          $emails = getEmailAdress($webhosting);

          foreach($emails as $email)
          {
              ?>
              <tr>
                <td><strong><?php echo($email['alias']); ?></strong></td>
                <td><?php echo($EmailTypeDict[$email['type']]); ?></td>
                <td>
                  <div class="progress">
                    <div class="progress-bar" role="progressbar" aria-valuenow="<?php echo($email['used']); ?>" 
                         aria-valuemin="0" aria-valuemax="<?php echo($email['size']); ?>" 
                         style="width: <?php echo($email['used'] * 100 / $email['size']); ?>%;">
                      <?php echo($email['used']); ?> MB
                    </div>
                  </div>
                </td>
                <td>
                  <button type="button" class="btn btn-warning" 
                    ng-click="mailbox.doClick('<?php echo($email['alias']); ?>', '<?php echo($email['type']); ?>')">
                    <span class="glyphicon glyphicon-pencil" aria-hidden="true"></span> Edit
                  </button>
                </td>
                <td>
                  <form class="form-horizontal" role="form" method="post">
                    <input name="delAlias" type="hidden" value="<?php echo($email['alias']); ?>"/>
                    <button name="delSubmit" type="submit" class="btn btn-danger">
                      <span class="glyphicon glyphicon-remove" aria-hidden="true"></span> Delete
                    </button>
                  </form>
                </td>
              </tr>


              <?php
          }
        ?>
      </table>
    </div>
  </div>
  <h2 ng-show="mailbox.edit" id="edit">Edit mailbox</h2>
  <div ng-show="mailbox.edit" class="panel panel-default">
    <div class="panel-body">
      <form class="form-horizontal" role="form" method="post" id="editForm">
        <input type="hidden" name="editOldAlias" value="{{mailbox.address}}"/>
          <div class="form-group">
          <label for="inputAddress" class="col-sm-2 control-label">Address</label>
          <div class="col-sm-10 emailInputGroup">
            <div class="input-group">
              <input name="editAlias" type="text" class="form-control" id="inputAddress" value="{{mailbox.address}}">
              <span class="input-group-addon">@<?php echo($_SESSION['domain']);?></span>
            </div>
          </div>
        </div>
        <div class="form-group">
          <label for="inputPath" class="col-sm-2 control-label">Type</label>
          <div class="col-sm-10">
            <div class="btn-group input-group btn-group-justified" data-toggle="buttons">
              <label class="btn btn-primary">
                <input type="radio" name="editType" value="0" />POP3</label>
              <label class="btn btn-primary">
                <input type="radio" name="editType" value="1" />IMAP</label>
              <label class="btn btn-primary">
                <input type="radio" name="editType" value="2" />POP3 & IMAP</label>
            </div>
          </div>
        </div>
        <div class="form-group">
          <label for="inputPassword" class="col-sm-2 control-label">Password</label>
          <div class="col-sm-10">
            <div class="input-group">
              <span class="input-group-addon glyphicon glyphicon-lock"></span>
              <input name="editPassword" type="password" class="form-control" id="inputPassword" placeholder="Password">
            </div>
          </div>
        </div>
        <div class="form-group">
          <div class="col-sm-offset-2 col-sm-10">
            <button name="editSubmit" type="submit" class="btn btn-success"><span class="glyphicon glyphicon-pencil"> Edit mailbox</span></button>
          </div>
        </div>
      </form>
    </div>
  </div>
</div>
<style type="text/css">
#editForm .emailInputGroup .form-control-feedback,
#addForm .emailInputGroup .form-control-feedback {
    top: 0;
    right: 105px;
}
</style>
<script src="js/angular.min.js"></script>
<script src="js/app.js"></script>
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
                alias: {
                    validators: {
                        notEmpty: {
                            message: 'The alias is required'
                        }
                    }
                },
                password: {
                    validators: {
                        notEmpty: {
                            message: 'The password is required'
                        },
                        stringLength: {
                            message: 'Password have to contain at least 4 characters',
                            min: 4
                        }
                    }
                },
                type: {
                    feedbackIcons: false,
                    validators: {
                        notEmpty: {
                            message: 'The type is required'
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
        $('#editForm')
        .bootstrapValidator({
            feedbackIcons: {
                valid: 'glyphicon glyphicon-ok',
                invalid: 'glyphicon glyphicon-remove',
                validating: 'glyphicon glyphicon-refresh'
            },
            fields: {
                editAlias: {
                    validators: {
                        notEmpty: {
                            message: 'The alias is required'
                        }
                    }
                },
                editPassword: {
                    validators: {
                        notEmpty: {
                            message: 'The password is required'
                        },
                        stringLength: {
                            message: 'Password have to contain at least 4 characters',
                            min: 4
                        }
                    }
                },
                editType: {
                    feedbackIcons: false,
                    validators: {
                        notEmpty: {
                            message: 'The type is required'
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

