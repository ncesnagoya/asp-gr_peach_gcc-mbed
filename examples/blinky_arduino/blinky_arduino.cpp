#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"

#include "blinky_arduino.h"

/* GR-PEACH Sketch Template V1.00 */
#include <Arduino.h>

#define INTERVAL 100

extern "C" void *__dso_handle = 0;

/*
 *  初期化タスク
 *
 *  C++の静的グローバルクラスのコンストラクタを呼び出す．
 */
typedef void (*func_ptr)(void);
void init_main_task(intptr_t exinf) {
	extern func_ptr __preinit_array_start[0], __preinit_array_end[0];
	for (func_ptr* func = __preinit_array_start; func != __preinit_array_end; func++) {
		(*func)();
	}	
	
	extern func_ptr __init_array_start[0], __init_array_end[0];
	for (func_ptr* func = __init_array_start; func != __init_array_end; func++) {
		(*func)();
	}

	main(); // call main() at mbed-lib/arduino-lib/core/main.cpp
}

void setup()
{
    pinMode(PIN_LED_RED   , OUTPUT);
    pinMode(PIN_LED_GREEN , OUTPUT);
    pinMode(PIN_LED_BLUE  , OUTPUT);
    pinMode(PIN_LED_USER  , OUTPUT);
	pinMode(PIN_SW        , INPUT);
}

void loop()
{
	while(digitalRead(PIN_SW) == 0){
		digitalWrite(PIN_LED_USER, 1);
        delay(INTERVAL);
        digitalWrite(PIN_LED_USER, 0);
        delay(INTERVAL);
	}
    digitalWrite(PIN_LED_RED, 1);
    delay(INTERVAL);
    digitalWrite(PIN_LED_RED, 0);
    digitalWrite(PIN_LED_GREEN, 1);
    delay(INTERVAL);
    digitalWrite(PIN_LED_GREEN, 0);
    digitalWrite(PIN_LED_BLUE, 1);
    delay(INTERVAL);
    digitalWrite(PIN_LED_BLUE, 0);
}
