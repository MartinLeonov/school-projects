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
  if(!isset($_POST['addType']) || strlen(trim($_POST['addLogin'])) < 2 ||
    strlen(trim($_POST['addPassword'])) < 4 || strlen(trim($_POST['addName'])) < 2)
    $formNotComplete = true;

  if(!$formNotComplete)
  {
    $password = sha1((trim($_POST['addPassword'])));
    $login = trim($_POST['addLogin']); 
    $name = trim($_POST['addName']);
    $type = $_POST['addType'];
    $db_server = $name . ".sql.".$_SESSION['domain'];


    $addItemSuccess = addDatabase($webhosting, $name, $login, $password, $type, $db_server);
  }

  if((isset($addItemSuccess) &&!$addItemSuccess) || $formNotComplete) 
  {
    $POST_login = isset($_POST['addLogin']) ? $_POST['addLogin'] : "";
    $POST_type = isset($_POST['addType']) ? $_POST['addType'] : "";
    $POST_name = isset($_POST['addName']) ? $_POST['addName'] : "";
    $addError = true;
  }

}

if(isset($_POST['delSubmit'])){
  require('db.php');

  $name = ($_POST['delName']);

  $delItemSuccess = deleteDatabase($webhosting, $name);
}

?>

<div class="col-xs-12 col-sm-6 col-md-8">

<?php 
if (isset($addItemSuccess)) {
if ($addItemSuccess): ?>

<div class="alert alert-success" role="alert"><strong>Well done!</strong> Database "<?php echo($name); ?>" was created successfully.</div>

<?php else: ?>

<div class="alert alert-danger" role="alert"><strong>Oh snap!</strong> Database couldn't be added. Here are some possible causes:
<ul><li><strong>Database with the same name already exists</strong></li></ul>
</div>

<?php endif; } ?>

<?php 
if (isset($formNotComplete)) {
if ($formNotComplete): ?>

<div class="alert alert-warning" role="alert">All fields are reguired. Here are some advices:
  <ul>
  <?php if ($POST_type !== ""): ?>
    <li><strong>Check if field name or login have at least 2 characters</strong></li>
    <li><strong>Check if field password have at least 4 characters</strong></li>
  <?php else: ?>
    <li><strong>Check if you choosed type of new database</strong></li>
  <?php endif; ?>
  </ul>
</div>

<?php endif; } ?>


<?php 
if (isset($delItemSuccess)) {
if ($delItemSuccess): ?>

<div class="alert alert-success" role="alert"><strong>Well done!</strong> Database "<?php echo($name); ?>" was deleted successfully.</div>

<?php else: ?>

<div class="alert alert-danger" role="alert"><strong>Oh snap!</strong> Database "<?php echo($name); ?>" couldn't be deleted.</div>

<?php endif; } ?>

  <h2>New Database</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <form class="form-horizontal" role="form" method="post" id="addForm">
        <div class="form-group">
          <label for="inputName" class="col-sm-2 control-label">Name</label>
          <div class="col-sm-10">
            <input name="addName" type="text" class="form-control" id="inputName" placeholder="New database name"
            <?php if (isset($addError) and $addError) echo('value="'.$POST_name.'"'); ?> >
          </div>
        </div>
        <div class="form-group">
          <label class="col-sm-2 control-label">Type</label>
          <div class="col-sm-10">
            <div class="btn-group input-group btn-group-justified" data-toggle="buttons">
              <label class="btn btn-primary <?php if (isset($addError) and $addError and $POST_type === "0") echo('active'); ?>  ">
                <input type="radio" name="addType" value="0" <?php if (isset($addError) and $addError and $POST_type === "0") echo('checked'); ?> />MySQL</label>
              <label class="btn btn-primary <?php if (isset($addError) and $addError and $POST_type === "1") echo('active'); ?>  ">
                <input type="radio" name="addType" value="1" <?php if (isset($addError) and $addError and $POST_type === "1") echo('checked'); ?> />PostgreSQL</label>
              <label class="btn btn-primary <?php if (isset($addError) and $addError and $POST_type === "2") echo('active'); ?>  ">
                <input type="radio" name="addType" value="2" <?php if (isset($addError) and $addError and $POST_type === "2") echo('checked'); ?> />MariaDB</label>
            </div>
          </div>
        </div>
        <div class="form-group">
          <label for="inputLogin" class="col-sm-2 control-label">Login</label>
          <div class="col-sm-10">
            <input name="addLogin" type="text" class="form-control" id="inputLogin" placeholder="Database login"
            <?php if (isset($addError) and $addError) echo('value="'.$POST_login.'"'); ?> >
          </div>
        </div>
        <div class="form-group">
          <label for="inputPassword" class="col-sm-2 control-label">Password</label>
          <div class="col-sm-10">
            <div class="input-group">
              <span class="input-group-addon glyphicon glyphicon-lock"></span>
              <input name="addPassword" type="password" class="form-control" id="inputPassword" placeholder="Password">
            </div>
          </div>
        </div>
        <div class="form-group">
          <div class="col-sm-offset-2 col-sm-10">
            <button name="addSubmit" type="submit" class="btn btn-success"><span class="glyphicon glyphicon-tasks"> Create database</span></button>
          </div>
        </div>
      </form>
    </div>
  </div>
  <h2>Manage existing databases</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <table class="table table-bordered">
        <tr>
          <td>Name</td>
          <td>Login</td>
          <td>Type</td>
          <td>Usage</td>
          <td></td>
          <td></td>
        </tr>
        <?php 
              $dbs = getDatabases($webhosting);

              foreach($dbs as $db)
              {
        ?>
        <tr>
          <td><strong><?php echo($db['name']); ?></strong></td>
          <td><?php echo($db['login']); ?></td>
          <td><?php echo($DbTypeDict[$db['type']]); ?></td>
          <td>
            <div class="progress">
              <div class="progress-bar" role="progressbar" aria-valuenow="95" aria-valuemin="0" aria-valuemax="100" style="width: 95%;">
                80 MB
              </div>
            </div>
          </td>
          <td>
            <button type="button" class="btn btn-info">
              <span class="glyphicon glyphicon-pencil" aria-hidden="true"></span> Adminer
            </button>
          </td>
          <td>
            <form class="form-horizontal" role="form" method="post">
              <input name="delName" type="hidden" value="<?php echo($db['name']); ?>"/>
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
                addName: {
                    validators: {
                        notEmpty: {
                            message: 'The name is required'
                        },
                        stringLength: {
                            message: 'name have to contain at least 2 characters',
                            min: 2
                        }
                    }
                },
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
                addPassword: {
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
                addType: {
                    feedbackIcons: false,
                    validators: {
                        notEmpty: {
                            message: 'The type name is required'
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
