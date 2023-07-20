#include<Arduino.h>
#include<SPI.h>
#include<Wire.h>
#include<Adafruit_GPS.h>

#define LED_INTERNI 2

//pinovi za GPS
#define GPSSerial Serial2
#define GPS_RX 3
#define GPS_TX 1

Adafruit_GPS GPS(&GPSSerial);

#define GPSECHO false


//za CO2 senzor
//pinovi za S8
#define S8_RX 13
#define S8_TX 12

byte CO2req[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
byte CO2out[] = {0, 0, 0, 0, 0, 0, 0};

void RequestCO2(){

	while (!Serial1.available()){

    	Serial1.write(CO2req, 7);
    	delay(50);
  	}

  	uint8_t timeout = 0;
  	while (Serial1.available() < 7 ){

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
	return val * 1; // S8 = 1. K-30 3% = 3, K-33 ICB = 10
}


void setup(){
  
	pinMode(LED_INTERNI, OUTPUT);

    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, S8_RX, S8_TX);
	Serial2.begin(9600,SERIAL_8N1, GPS_RX, GPS_TX); //valjda je dobro

}


void loop() {

	RequestCO2();
  	unsigned long CO2 = CO2count();
  	delay(2000);

  	Serial.println("CO2: " + String(CO2));

	if (GPS.fix){
		Serial.println(GPS.longitude);
		Serial.println(GPS.latitude);	
	}else{
		Serial.println("GPS greska");
	}
	
	
}