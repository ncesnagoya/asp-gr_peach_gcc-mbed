#ifndef _CAMTANK_H_
#define _CAMTANK_H_

#define TASKPRI_MAIN		4
#define TASKPRI_TIMELAPSE	4

#define TASKSTACKSIZE		(1024)

#ifndef KMM_SIZE
#define	KMM_SIZE	(TASKSTACKSIZE * 32)	/* カーネルが割り付ける */
#endif /* KMM_SIZE */						/* メモリ領域のサイズ */

#ifdef __cplusplus
extern "C" {
#endif

extern void task_main(intptr_t exinf);
extern void task_timelapse(intptr_t exinf);

#ifdef __cplusplus
}
#endif

#endif /* _CAMTANK_H_ */
