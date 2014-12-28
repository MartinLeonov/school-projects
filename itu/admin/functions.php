<?php
abstract class State {
	const active = 0;
	const inactive = 1;
	const waiting = 2;
	const rejected = 3;
	const expired = 4;

}

abstract class EmailType {
	const pop3 = 0;
	const imap = 1;
	const pop3imap = 2;
	const deactivated = 3;
}

$EmailTypeDict = array (
	0 => 'POP3',
	1 => 'IMAP',
	2 => 'POP3 & IMAP',
	3 => 'DEACTIVATED',
);

abstract class DbType {
	const mysql = 0;
	const postgresql = 1;
	const mariadb = 2;
	const unkown = 3;
}

$DbTypeDict = array (
	0 => 'MySQL',
	1 => 'PostgreSQL',
	2 => 'MariaDB',
	3 => 'Unkown',
);

abstract class DnsType {
	const primary = 0;
	const secondary = 1;
}

$DnsTypeDict = array (
	0 => 'primary',
	1 => 'secondary',
);

$StatusDict = array (
	0 => 'active',
	1 => 'inactive',
);


function getDefaultDomain($id)
{
	require('db.php');
	$qz = "SELECT name, tld, state FROM DOMAIN WHERE client='".$id."' LIMIT 1" ;
	$result = mysqli_query($conn,$qz);

	if ($result && $result->num_rows != 0){
		$row = $result->fetch_assoc();
		return $row; 
	}

	return null;
}

function getDomains($id)
{
	require('db.php');
	$qz = "SELECT name, tld, state, fullname FROM DOMAIN WHERE client='".$id."'" ;
	$result = mysqli_query($conn,$qz);

	
	if ($result && $result->num_rows != 0){
		return resultToArray($result); 
	}

	return array();
}

function getWebHosting($id)
{
	require('db.php');
	$qz = "SELECT * FROM WEBHOSTING WHERE client='".$id."' LIMIT 1" ;
	$result = mysqli_query($conn,$qz);

	if ($result && $result->num_rows != 0){
		$row = $result->fetch_assoc();
		return $row['id']; 
	}

	return null;
}

function getUser($id)
{
	if($id == null)
		return null;

	require('db.php');
	$qz = "SELECT email, phone FROM CLIENT WHERE id='".$id."' LIMIT 1" ;
	$result = mysqli_query($conn,$qz);

	if ($result && $result->num_rows != 0){
		return $result->fetch_assoc();
	}

	return null;
}



function updateUserInfo($id, $email, $phone)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "UPDATE CLIENT SET email = '".$email."', phone = '".$phone."' WHERE id='".$id."'";
	return $conn->query($qz);

}

function updateUserPassword($id, $newpass)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "UPDATE CLIENT SET password = UNHEX('".$newpass."') WHERE id='".$id."' ";
	return $conn->query($qz);

}

function getDNSRecords($name)
{
	if($name == null)
		return null;

	require('db.php');
	$qz = "SELECT name, type, status, TTL FROM DNSRECORD WHERE domain='".$name."' " ;
	$result = mysqli_query($conn,$qz);

	if ($result && $result->num_rows != 0){
		return resultToArray($result); 
	}

	return array();
}

function deleteDNSRecord($name, $domain)
{
	if($name == null)
		return false;

	require('db.php');
	$qz = "DELETE FROM `DNSRECORD` WHERE domain='".$domain."' AND name='".$name."' ";
	return $conn->query($qz);
}

function addDNSRecord($domain, $name, $ipv4, $type, $ttl)
{
	if($domain == null)
		return false;

	require('db.php');
	$qz = "INSERT INTO `DNSRECORD` (name, TTL, type, IPv4, IPv6, domain, status)
			VALUES ('".$name."', '".$TTL."', '".$type."', '".$ipv4."', '', '".$domain."', '0')";
	return $conn->query($qz);
}

function getEmailAdress($id)
{
	if($id == null)
		return null;

	require('db.php');
	$qz = "SELECT alias, type, used, size FROM MAILBOX WHERE webhosting='".$id."' " ;
	$result = mysqli_query($conn,$qz);

	if ($result && $result->num_rows != 0){
		return resultToArray($result); 
	}

	return array();
}

function addEmailAdress($id, $alias, $password, $type)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "INSERT INTO MAILBOX (alias, type, password, webhosting) VALUES ('".$alias."', '".$type."', '".$password."', '".$id."')";
	return $conn->query($qz);
}


function addDatabase($id, $name, $login, $password, $type, $server)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "INSERT INTO `DATABASE` (name, login, password, type, server, webhosting) 
		   VALUES ('".$name."', '".$login."', '".$password."', '".$type."', '".$server."', '".$id."')";

	return $conn->query($qz);
}

function getDatabases($id)
{
	if($id == null)
		return null;

	require('db.php');
	$qz = "SELECT name, login, type, server FROM `DATABASE` WHERE webhosting='".$id."' " ;
	$result = mysqli_query($conn,$qz);

	if ($result && $result->num_rows != 0){
		return resultToArray($result); 
	}

	return array();
}

function deleteDatabase($id, $name)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "DELETE FROM `DATABASE` WHERE webhosting='".$id."' AND name='".$name."' ";
	return $conn->query($qz);
}

function addFtpAccount($id, $login, $password, $directory, $server)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "INSERT INTO FTPACCOUNT (login, password, directory, status, server, webhosting) 
		   VALUES ('".$login."', '".$password."', '".$directory."', '0', '".$server."', '".$id."')";
	return $conn->query($qz);
}

function getFtpAccounts($id)
{
	if($id == null)
		return null;

	require('db.php');
	$qz = "SELECT login, status FROM FTPACCOUNT WHERE webhosting='".$id."' " ;
	$result = mysqli_query($conn,$qz);

	if ($result && $result->num_rows != 0){
		return resultToArray($result); 
	}

	return array();
}

function deleteFtpAccount($id, $login)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "DELETE FROM FTPACCOUNT WHERE webhosting='".$id."' AND login='".$login."' ";
	return $conn->query($qz);
}

function changeFtpAccount($id, $login, $status)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "UPDATE FTPACCOUNT SET status = '".$status."' WHERE webhosting='".$id."' AND login='".$login."' ";
	return $conn->query($qz);

}

function updateEmailAdress($id, $old_alias, $alias, $password, $type)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "UPDATE MAILBOX SET alias = '".$alias."', password = '".$password."', type = '".$type."' WHERE webhosting='".$id."' AND alias='".$old_alias."' ";
	return $conn->query($qz);

}

function deleteEmailAdress($id, $alias)
{
	if($id == null)
		return false;

	require('db.php');
	$qz = "DELETE FROM MAILBOX WHERE webhosting='".$id."' AND alias='".$alias."' ";
	return $conn->query($qz);
}


function resultToArray($result)
{
	$rows = array();
	while($row = $result->fetch_assoc()) {
		$rows[] = $row;
	}
	return $rows;
}

function generatePassword($length = 8) {
    $chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    $count = mb_strlen($chars);

    for ($i = 0, $result = ''; $i < $length; $i++) {
        $index = rand(0, $count - 1);
        $result .= mb_substr($chars, $index, 1);
    }

    return $result;
}
?>