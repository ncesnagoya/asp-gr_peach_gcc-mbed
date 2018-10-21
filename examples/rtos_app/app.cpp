#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"

/* GR-PEACH Sketch Template V1.00 */
#include <Arduino.h>

/*
#include "EthernetInterface.h"
#include "HTTPServer.h"
#include "mbed_rpc.h"
#include "RomRamFileSystem.h"
#include "file_table.h"        //Binary data of web pages
#include "i2c_setting.h"
*/

#include "syssvc/logtask.h"
#include "arduino_app.h"

/*
 *  Error check of service calls
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

bool_t led_user_state = true;

/*
 *  Interrupt handler
 */
void button_hander(void)
{
	/* add your code here */
	syslog(LOG_NOTICE, "USER_BUTTON0 was pushed.");
    digitalWrite(LED_USER, led_user_state);
	led_user_state = !led_user_state;
	
   	SVC_PERROR(iwup_tsk(BLINK_TASK));
}

/*
 *  Main task
 */
void
main_task(intptr_t exinf) {
	/* configure syslog */
	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));	
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/* Initialization */
    pinMode(LED_USER, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
	pinMode(LED_RED, OUTPUT);

	attachInterrupt(USER_BUTTON0, button_hander, FALLING); // ok
	//	attachInterrupt(0, PIN_USER_BUTTON0, button_hander, CHANGE); //ok	
	sta_cyc(CYCHDR1);	
}

/*
 *  LED blink task
 */
#define NUM_OF_COUNT 3
uint8_t count = 0;
void
blink_task(intptr_t exinf) {
	while (1) {
		SVC_PERROR(slp_tsk());
		while (count < NUM_OF_COUNT) {
			digitalWrite(LED_RED, 1);
			SVC_PERROR(dly_tsk(250));
			digitalWrite(LED_RED, 0);
			SVC_PERROR(dly_tsk(250));
			count++;
		}
		count = 0;			
	}
}


/*
 *  Cyclic handler
 */
bool_t led_blue_state = LOW;
void cyclic_handler(intptr_t exinf)
{
	/* add your code here */
    digitalWrite(LED_BLUE, led_blue_state);
	led_blue_state = !led_blue_state;	
}



#define NUM_OF_COUNT 3
void
blink_task(intptr_t exinf) {
	while (1) {
		digitalWrite(LED_RED, state);
		SVC_PERROR(dly_tsk(2000));
	}
}




uint8_t count = 0;
void
blink_task(intptr_t exinf) {
	count = 0;
	while (count < NUM_OF_COUNT) {
		digitalWrite(LED_RED, 1);
		SVC_PERROR(dly_tsk(250));
		digitalWrite(LED_RED, 0);
		SVC_PERROR(dly_tsk(250));
		count++;
	}
}
