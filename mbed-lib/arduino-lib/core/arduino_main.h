#ifndef _ARDUINO_MAIN_H_
#define _ARDUINO_MAIN_H_

#ifndef ARDUINO_MAIN_TASK_PRI
#define  ARDUINO_MAIN_TASK_PRI  2
#endif /*  ARDUINO_MAIN_TASK_PRI */

#ifndef ARDUINO_MAIN_TASK_STACK_SIZE
#define ARDUINO_MAIN_TASK_STACK_SIZE 1024
#endif  /* ARDUINO_MAIN_TASK_STACK_SIZE */

#ifdef __cplusplus
extern "C" {
#endif

extern void arduino_main_task(intptr_t exinf);

#ifdef __cplusplus
}
#endif 

#endif /* _ARDUINO_MAIN_H_ */

