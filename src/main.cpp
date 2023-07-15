#include<Arduino.h>

#define LED_INTEGRIRANI 2

void setup() {

	pinMode(LED_INTEGRIRANI, OUTPUT);

	//startup sekvenca
	delay(1000);
	
	for (uint8_t i = 0; i < 3; i++){
		digitalWrite(LED_INTEGRIRANI, HIGH);
		delay(500);
		digitalWrite(LED_INTEGRIRANI, LOW);
		delay(500);
	}
	
}

void loop() {
	
}
