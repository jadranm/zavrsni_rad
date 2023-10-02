<?php
/*
	~\xampp\htdocs\co2_projekt
*/

$hostname = "localhost"; 
$username = "root"; 
$password = ""; 
$database = "sensor_db";


$conn = mysqli_connect($hostname, $username, $password, $database);

if (!$conn) { 
	die("konekcija je neuspješna" . mysqli_connect_error()); 
} 

echo "konekcija sa bazom je OK<br>";

if(isset($_POST["latitude"]) && isset($_POST["longitude"]) && isset($_POST["co2"])) {	
	
	
	$lat = $_POST["latitude"];
	$long = $_POST["longitude"];
	$co2 = $_POST["co2"];

	//test vrijednosti
	/*
	$lat = 33;
	$long = 666;
	$co2 =323400;
	*/
	
	$sql1 = "INSERT INTO co2_tablica (latitude, longitude, co2) VALUES (".$lat.",".$long.",".$co2.")";



	if (mysqli_query($conn, $sql1)) { 
		echo "\nnova vrijednost je uspjesno dodana"; 
	} else { 
		echo "Error: " . $sql1 . "<br>" . mysqli_error($conn); 
	}


}

?>