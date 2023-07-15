#include <Arduino.h>
#include <SoftwareSerial.h>
#include "s8_uart.h"

#define LED_INTEGRIRANI 2
#define DEBUG_BAUDRATE 115200

#define S8_RX_PIN 16
#define S8_TX_PIN 17


SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);


S8_UART *sensor_S8;
S8_sensor sensor;

void setup()
{

	pinMode(LED_INTEGRIRANI, OUTPUT);

	// startup sekvenca
	delay(1000);

	for (uint8_t i = 0; i < 3; i++)
	{
		digitalWrite(LED_INTEGRIRANI, HIGH);
		delay(500);
		digitalWrite(LED_INTEGRIRANI, LOW);
		delay(500);
	}


	Serial.begin(DEBUG_BAUDRATE);

	// Wait port is open or timeout
	int i = 0;
	while (!Serial && i < 50){
		delay(10);
		i++;
	}

	
	Serial.println("serial radi");

	// Initialize S8 sensor
	S8_serial.begin(S8_BAUDRATE);
	sensor_S8 = new S8_UART(S8_serial);
}



void loop(){

  	sensor.co2 = sensor_S8->get_co2();
  	printf("CO2 value = %d ppm\n", sensor.co2);
	Serial.printf("/*%u*/\n", sensor.co2);

	delay(500);

}
