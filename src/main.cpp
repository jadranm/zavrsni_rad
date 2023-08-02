#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


#define LED_INTERNI 2

#define SSID "lmaoo_xd"
#define PASSWORD "marinovic"
#define SERVER_PATH "http://192.168.0.212/co2_projekt/test.php"

// pinovi za GPS
#define GPSSerial Serial2
#define GPS_RX 16
#define GPS_TX 17

//Adafruit_GPS GPS(&GPSSerial);

#define GPSECHO false

// za CO2 senzor
// pinovi za S8
#define S8_RX 13
#define S8_TX 12

byte CO2req[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
byte CO2out[] = {0, 0, 0, 0, 0, 0, 0};

void Alarm(){
	for (int8_t i = 0; i < 3; i++){
		digitalWrite(LED_INTERNI, HIGH);
		delay(500);
		digitalWrite(LED_INTERNI, LOW);
		delay(500);
	}
}

void RequestCO2(){

	while (!Serial1.available()){

		Serial1.write(CO2req, 7);
		delay(50);
	}

	uint8_t timeout = 0;
	while (Serial1.available() < 7){

		timeout++;
		if (timeout > 10){

			while (Serial1.available())
				Serial1.read();
			break;
		}
		delay(50);
	}

	for (uint8_t i = 0; i < 7; i++)
		CO2out[i] = Serial1.read();
}

unsigned long CO2count(){

	int high = CO2out[3];
	int low = CO2out[4];
	unsigned long val = high * 256 + low;
	return val * 1;
}

void StatickaIP(){
	//staticka ip adresa
	IPAddress local_IP(192, 168, 1, 184);
	IPAddress gateway(192, 168, 1, 1);
	IPAddress subnet(255, 255, 0, 0);
	IPAddress primaryDNS(8, 8, 8, 8);
	
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)){
    	Serial.println("greska u konfiguraciji staticke IP adrese");
		Alarm();
	}
}

void connectWiFi() {
	WiFi.mode(WIFI_OFF);
  	delay(1000);
  	
  	WiFi.mode(WIFI_STA);
  
  	WiFi.begin(SSID, PASSWORD);
  	Serial.println("Connecting to WiFi");
  
  	while (WiFi.status() != WL_CONNECTED){
    	delay(500);
    	Serial.print(".");
  	}
    
  	Serial.print("connected to : "); Serial.println(SSID);
  	Serial.print("IP address: "); Serial.println(WiFi.localIP());
}



void setup(){

	pinMode(LED_INTERNI, OUTPUT);

	Serial.begin(115200);
	Serial1.begin(9600, SERIAL_8N1, S8_RX, S8_TX);
	//Serial2.begin(9600,SERIAL_8N1, GPS_RX, GPS_TX); //ova linija sjebe sve

	//staticka ip nije potrebna
	//StatickaIP();

	connectWiFi();
}



void loop(){

	RequestCO2();
	unsigned long CO2 = CO2count();
	delay(2000);

	Serial.println("CO2: " + String(CO2));
	/*
	if (GPS.fix){
		Serial.println(GPS.longitude);
		Serial.println(GPS.latitude);
		Serial.println("");
	}
	else{
		//Serial.println("GPS greska");
	}
	*/

	//za mysql bazu podataka
	if(WiFi.status() != WL_CONNECTED) { 
    	connectWiFi();
  	}

  	String postData = "co2=" + String(CO2);

  	HTTPClient http; 
  	http.begin(SERVER_PATH);
  	http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  	int httpCode = http.POST(postData);
  	String payload = http.getString(); 
  
  	Serial.print("URL : "); Serial.println(SERVER_PATH); 
  	Serial.print("Data: "); Serial.println(postData); 
  	Serial.print("httpCode: "); Serial.println(httpCode); 
  	Serial.print("payload : "); Serial.println(payload); 
  	Serial.println("--------------------------------------------------");
}