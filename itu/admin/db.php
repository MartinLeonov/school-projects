<?php
$servername = "localhost";
$username = "root";
$password = "isa2014";
$dbname = "ITU";

// Create connection
$conn = new mysqli($servername, $username, $password);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$conn->select_db($dbname);

?> 