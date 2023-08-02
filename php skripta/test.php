<?php
/*
	~\xampp\htdocs\co2_projekt

    ova skripta radi lol
	dodati backup tablicu i kada se priitsne gumb da se trenutno mjerenje konvertira u csv i izbrise
*/

$hostname = "localhost"; 
$username = "root"; 
$password = ""; 
$database = "sensor_db"; 

$conn = mysqli_connect($hostname, $username, $password, $database);

if (!$conn) { 
	die("konekcija je neuspješna" . mysqli_connect_error()); 
} 

echo "konekcija sa bazom je ok<br>"; 

if(isset($_POST["co2"])){

	$t = $_POST["co2"];
    //$t = 2223;

	$sql = "INSERT INTO co2_tablica (co2) VALUES (".$t.")"; 

	if (mysqli_query($conn, $sql)) { 
		echo "\nnova vrijednost je uspjesno dodana"; 
	} else { 
		echo "Error: " . $sql . "<br>" . mysqli_error($conn); 
	}
}

?>