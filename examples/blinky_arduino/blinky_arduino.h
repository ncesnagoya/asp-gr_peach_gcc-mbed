#ifndef _BLINLY_H_
#define _BLINLY_H_

#include "main.h" // Arduino 環境のメイン関数

#ifndef INIT_MAIN_TASK_PRI
#define  INIT_MAIN_TASK_PRI  1
#endif /*  INIT_MAIN_TASK_PRI */

#ifndef INIT_MAIN_TASK_STACK_SIZE
#define INIT_MAIN_TASK_STACK_SIZE 1024
#endif  /* INIT_MAIN_TASK_STACK_SIZE */

#ifndef KMM_SIZE
#define	KMM_SIZE	(INIT_MAIN_TASK_STACK_SIZE * 16)	/* カーネルが割り付ける */
#endif /* KMM_SIZE */						/* メモリ領域のサイズ */


#ifdef __cplusplus
extern "C" {
#endif

extern void init_main_task(intptr_t exinf);

#ifdef __cplusplus
}
#endif

#endif /* _BLINLY_H_ */
