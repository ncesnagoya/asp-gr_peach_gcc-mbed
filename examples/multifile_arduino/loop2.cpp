#include <Arduino.h>

#define INTERVAL_BLUE  150

void loop2(){
    digitalWrite(PIN_LED_BLUE, 1);
    delay(INTERVAL_BLUE);
	digitalWrite(PIN_LED_BLUE, 0);
	delay(INTERVAL_BLUE);
}

