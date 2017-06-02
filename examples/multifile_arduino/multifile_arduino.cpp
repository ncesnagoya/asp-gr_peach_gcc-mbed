#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"

#include "arduino_app.h"
#include "arduino_main.h"

/* GR-PEACH Sketch Template V1.00 */
#include <Arduino.h>

#define INTERVAL       100
#define INTERVAL_RED   100

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
	digitalWrite(PIN_LED_RED, 1);
	delay(INTERVAL_RED);
	digitalWrite(PIN_LED_RED, 0);
	delay(INTERVAL_RED);
}

/*
 *  Cyclic Handler
 * 
 *  This handler is called every 10 [ms] as specified in
 *  multitask_arduino.cfg.
 */
void cyclic_handler(intptr_t exinf) {
	irot_rdq(LOOP_PRI); /* change the running loop. */
}
