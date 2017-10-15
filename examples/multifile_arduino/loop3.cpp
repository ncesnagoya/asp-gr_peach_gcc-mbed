#include <Arduino.h>

#define INTERVAL_USER       100

void loop3(){
	while(digitalRead(PIN_SW) == 0){
		digitalWrite(PIN_LED_USER, 1);
        delay(INTERVAL_USER);
        digitalWrite(PIN_LED_USER, 0);
        delay(INTERVAL_USER);
	}
}
