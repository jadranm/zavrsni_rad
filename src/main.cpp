#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

hw_timer_t *Timer0_Cfg = NULL;

#define SERIAL_BRZINA 115200
#define LED_INTERNI 2

hw_timer_t *Timer0_Cfg = NULL;

#define SSID ""
#define PASSWORD ""
#define SERVER_PATH "http://192.168.0.212/co2_projekt/test.php"


// pinovi za GPS
HardwareSerial neogps(1);	//uart port 1
TinyGPSPlus gps;
#define GPS_RX 16
#define GPS_TX 17
float longitude, latitude;
//sa rx16 tx17 radi

// za CO2 senzor
// pinovi za S8
#define S8_RX 13
#define S8_TX 12

byte CO2req[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
byte CO2out[] = {0, 0, 0, 0, 0, 0, 0};

int counter= 0;
int *CO2_p;

void IRAM_ATTR Timer0_ISR(){

	String postData = "co2=" + String(*CO2_p);

  	HTTPClient http; 
  	http.begin(SERVER_PATH);
  	http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  	int httpCode = http.POST(postData);
  	String payload = http.getString(); 
	counter++;
}


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

void SpajanjeNaWIFI() {
	WiFi.mode(WIFI_OFF);
  	delay(1000);
  	
  	WiFi.mode(WIFI_STA);
  
  	WiFi.begin(SSID, PASSWORD);
  	Serial.println("Spajanje na WiFi");
  
  	while (WiFi.status() != WL_CONNECTED){
    	delay(500);
    	Serial.print(".");
  	}
    Serial.print("Spojeno na: ");
  	Serial.print("Ime mreže : "); Serial.println(SSID);
  	Serial.print("IP address: "); Serial.println(WiFi.localIP());
}

bool upis_flag = false;

void IRAM_ATTR Timer0_ISR(){
	upis_flag = true;
}

void setup(){

	pinMode(LED_INTERNI, OUTPUT);

	Serial.begin(SERIAL_BRZINA);
	Serial1.begin(9600, SERIAL_8N1, S8_RX, S8_TX);
	neogps.begin(9600,SERIAL_8N1, GPS_RX, GPS_TX);
	
	//konfigurirano na 30 sekundi
	Timer0_Cfg = timerBegin(0, 64000, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 37500, true);
    timerAlarmEnable(Timer0_Cfg);
	

Timer0_Cfg = timerBegin(0, 64000, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 37500, true);
    timerAlarmEnable(Timer0_Cfg);

	//staticka ip nije potrebna
	//StatickaIP();

	//SpajanjeNaWIFI();
}



void loop(){
	
	RequestCO2();
	unsigned long CO2 = CO2count();
	delay(2000);
	
	//za mysql bazu podataka
	if(WiFi.status() != WL_CONNECTED) { 
    	SpajanjeNaWIFI();
  	}
	
	
	//za tinyGPS
	boolean newData = false;
  	for (unsigned long start = millis(); millis() - start < 1000;) {
		while (neogps.available()) {
			if (gps.encode(neogps.read())) {
				newData = true;
			}
		}
  	}
	

	if (gps.location.isValid() == 1) {

			latitude = gps.location.lat();
			longitude = gps.location.lng();

			//Serial.println(gps.location.lat(), 6);
			//Serial.println(gps.location.lng(), 6);
			
			delay(5000);
		
	}else{
		Serial.println("no fix"); 
	}
	
	if(newData == true) {   
    	Serial.println("No Data");
    	newData = false;
    	Serial.println(gps.satellites.value());
  	}
  	if(newData == false){
    	Serial.println("No Data");
	}


	if (upis_flag == true && gps.location.isValid()){
		
		String postData = "co2=" + String(CO2);

		HTTPClient http; 
		http.begin(SERVER_PATH);
		http.addHeader("Content-Type", "application/x-www-form-urlencoded");
	
		int httpCode = http.POST(postData);
		String payload = http.getString(); 
		
		Serial.println("CO2: " + String(CO2));
		Serial.print("geografska duzina: "); Serial.println(latitude, 6);
		Serial.print("geografska sirina: "); Serial.println(longitude, 6);
		Serial.print("URL: "); Serial.println(SERVER_PATH); 
		//Serial.print("Data: "); Serial.println(postData); 
		Serial.print("httpCode: "); Serial.println(httpCode);
		Serial.print("payload: "); Serial.println(payload); 
		Serial.println("--------------------------------------------------");

		upis_flag = false;
	}
	

}