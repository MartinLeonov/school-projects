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
  if(!isset($_POST['addType']) || strlen(trim($_POST['addName'])) < 2
    || !filter_var(trim($_POST['addIp']), FILTER_VALIDATE_IP))
    $formNotComplete = true;

  if(!$formNotComplete)
  {
    $ip = trim($_POST['addIp']); 
    $name = trim($_POST['addName']);
    $type = $_POST['addType'];
    $ttl = $_POST['addTTL'];

    $addItemSuccess = addDNSRecord($_SESSION['domain'], $name, $ip, $type, $ttl);
  }

  if((isset($addItemSuccess) &&!$addItemSuccess) || $formNotComplete) 
  {
    $POST_type = isset($_POST['addType']) ? $_POST['addType'] : "";
    $POST_ip = isset($_POST['addIp']) ? $_POST['addIp'] : "";
    $POST_name = isset($_POST['addName']) ? $_POST['addName'] : "";
    $POST_ttl = isset($_POST['addTTL']) ? $_POST['addTTL'] : "";
    $addError = true;
  }

}

if(isset($_POST['delSubmit'])){
  require('db.php');

  $name = ($_POST['delName']);

  $delItemSuccess = deleteDNSrecord($name, $_SESSION['domain']);
}

?>

<div class="col-xs-12 col-sm-6 col-md-8">


<?php 
if (isset($addItemSuccess)) {
if ($addItemSuccess): ?>

<div class="alert alert-success" role="alert"><strong>Well done!</strong> DNS record "<?php echo($name); ?>" was created successfully.</div>

<?php else: ?>

<div class="alert alert-danger" role="alert"><strong>Oh snap!</strong> DNS record couldn't be added. Here are some possible causes:
<ul><li><strong>DNS record with the same name already exists</strong></li></ul>
</div>

<?php endif; } ?>

<?php 
if (isset($formNotComplete)) {
if ($formNotComplete): ?>

<div class="alert alert-warning" role="alert">All fields are reguired. Here are some advices:
  <ul>
  <?php if ($POST_type !== ""): ?>
    <li><strong>Check if field name have at least 2 characters</strong></li>
    <li><strong>Check if field ip address is valid</strong></li>
  <?php else: ?>
    <li><strong>Check if you choosed type of dns record</strong></li>
  <?php endif; ?>
  </ul>
</div>

<?php endif; } ?>
<?php 
if (isset($delItemSuccess)) {
if ($delItemSuccess): ?>

<div class="alert alert-success" role="alert"><strong>Well done!</strong> DNS record "<?php echo($name); ?>" was deleted successfully.</div>

<?php else: ?>

<div class="alert alert-danger" role="alert"><strong>Oh snap!</strong> DNS record "<?php echo($name); ?>" couldn't be deleted.</div>

<?php endif; } ?>

  <h2>Add dns record to "<?php echo($_SESSION['domain']); ?>" domain</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <form class="form-horizontal" role="form" id="addForm" method="post">
        <div class="form-group">
          <label for="inputName" class="col-sm-2 control-label">Name</label>
          <div class="col-sm-10">
            <input name="addName" type="text" class="form-control" id="inputName" placeholder="Domain name"
            <?php if (isset($addError) and $addError) echo('value="'.$POST_name.'"'); ?> >
          </div>
        </div>
        <div class="form-group">
          <label class="col-sm-2 control-label">Type</label>
          <div class="col-sm-10">
            <div class="btn-group input-group btn-group-justified" data-toggle="buttons">
              <label class="btn btn-primary <?php if (isset($addError) and $addError and $POST_type === "0") echo('active'); ?>">
                <input type="radio" name="addType" value="0" <?php if (isset($addError) and $addError and $POST_type === "0") echo('checked'); ?> />Primary</label>
              <label class="btn btn-primary <?php if (isset($addError) and $addError and $POST_type === "1") echo('active'); ?>">
                <input type="radio" name="addType" value="1" <?php if (isset($addError) and $addError and $POST_type === "1") echo('checked'); ?> />Secondary</label>
            </div>
          </div>
        </div>
        <div class="form-group">
          <label for="inputPrimaryIp" class="col-sm-2 control-label">Primary IP</label>
          <div class="col-sm-10">
            <input name="addIp" type="text" class="form-control" id="inputPrimaryIp" placeholder="Primary IP"
            <?php if (isset($addError) and $addError) echo('value="'.$POST_ip.'"'); ?> >
          </div>
        </div>
                <div class="form-group">
          <label for="inputPrimaryIp" class="col-sm-2 control-label">TTL<br \>Default: 1800</label>
          <div class="col-sm-10">
            <input name="addTTL" type="number" class="form-control" placeholder="Time to live"
            <?php if (isset($addError) and $addError) echo('value="'.$POST_ttl.'"'); ?> >

          </div>
        </div>
        <div class="form-group">
          <div class="col-sm-offset-2 col-sm-10">
            <button name="addSubmit" type="submit" class="btn btn-success"><span class="glyphicon glyphicon-plus-sign"> Add DNS record</span></button>
          </div>
        </div>
      </form>
    </div>
  </div>
  <h2>DNS records</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <table class="table table-bordered">
        <tr>
          <td>Name</td>
          <td>Status</td>
          <td>Type</td>
          <td>TTL</td>
          <td></td>
        </tr>
        <?php 
              $records = getDNSRecords($_SESSION['domain']);

              foreach($records as $dns)
              {
        ?>
        <tr>
          <td><strong><?php echo($dns['name']); ?></strong></td>
          <td><?php echo($StatusDict[$dns['status']]); ?></td>
          <td><strong><?php echo($DnsTypeDict[$dns['type']]); ?></strong></td>
          <td><?php echo($dns['TTL']); ?></td>
          <td>
            <form class="form-horizontal" role="form" method="post">
              <input type="hidden" name="delName" value="<?php echo($dns['name']); ?>"/>
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
                addIp: {
                    validators: {
                        notEmpty: {
                            message: 'The IP address is required'
                        },
                        ip: {
                            ipv4: true,
                            message: 'The IP address is not valid'
                        }
                    }
                },
                addType: {
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