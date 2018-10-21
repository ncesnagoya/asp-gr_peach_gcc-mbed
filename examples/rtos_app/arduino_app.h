#ifndef _ARDUINO_APP_H_
#define _ARDUINO_APP_H_

#define ADDITIONAL_LOOP_NUM 0 /* number of additional loops */

/*
 *  Definition of priority and stack size
 *
 *  Define priority and stack size for each task used in this
 *  application. These definitions are refered from system the
 *  confguration file (.cfg).
 */
#ifndef MAIN_TASK_PRI
#define MAIN_TASK_PRI  4
#endif /* MAIN_TASK_PRI */

#ifndef MAIN_TASK_STACK_SIZE
#define MAIN_TASK_STACK_SIZE 1024
#endif  /* MAIN_TASK_STACK_SIZE */

/*
 *  Definition of memory size for RTOS
 *
 *  Define size of memory which used by RTOS at runtime.
 */

#ifndef KMM_SIZE
#define	KMM_SIZE	(MAIN_TASK_STACK_SIZE * 16)
#endif /* KMM_SIZE */

/*
 *  Declaration of handlers and tasks 
 * 
 *  When you define a new handler/task in .cfg, add its prototype
 *  declaration here.
 */
#ifdef __cplusplus
extern "C" {
#endif

extern void	cyclic_handler(intptr_t exinf);
extern void	button_handler(void);
extern void main_task(intptr_t exinf);
extern void blink_task(intptr_t exinf);

#ifdef __cplusplus
}
#endif

#endif /* _ARDUINO_APP_H_ */
