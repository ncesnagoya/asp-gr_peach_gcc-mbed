#include <Arduino.h>

#define INTERVAL_GREEN 200

void loop1(){
    digitalWrite(PIN_LED_GREEN, 1);
    delay(INTERVAL_GREEN);
    digitalWrite(PIN_LED_GREEN, 0);
	delay(INTERVAL_GREEN);
}


