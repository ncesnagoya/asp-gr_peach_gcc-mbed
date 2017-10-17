#ifndef _WEBCAMERA_H_
#define _WEBCAMERA_H_

#ifndef WEBCAMERA_MAIN_TASK_PRI
#define WEBCAMERA_MAIN_TASK_PRI  4
#endif /* WEBCAMERA_MAIN_TASK_PRI */

#ifndef WEBCAMERA_MAIN_TASK_STACK_SIZE
#define WEBCAMERA_MAIN_TASK_STACK_SIZE 1024 * 5
#endif  /* WEBCAMERA_MAIN_TASK_STACK_SIZE */

#ifndef KMM_SIZE
#define	KMM_SIZE	(WEBCAMERA_MAIN_TASK_STACK_SIZE * 16)	/* カーネルが割り付ける */
#endif /* KMM_SIZE */						/* メモリ領域のサイズ */

#ifdef __cplusplus
extern "C" {
#endif

extern void	cyclic_handler(intptr_t exinf);
extern void webcamera_main_task(intptr_t exinf);

#ifdef __cplusplus
}
#endif

#endif /* _WEBCAMERA_H_ */
