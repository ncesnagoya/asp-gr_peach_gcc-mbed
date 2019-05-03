#ifndef _QUICCLIENT_H_
#define _QUICCLIENT_H_

#ifndef QUIC_MAIN_TASK_PRI
#define QUIC_MAIN_TASK_PRI  4
#endif /* SSL_MAIN_TASK_PRI */

#ifndef QUIC_MAIN_TASK_STACK_SIZE
#define QUIC_MAIN_TASK_STACK_SIZE 1024 *  24
#endif  /* SSL_MAIN_TASK_STACK_SIZE */

#ifndef KMM_SIZE
#define	KMM_SIZE	(1024 * 32)	/* カーネルが割り付ける */
#endif /* KMM_SIZE */						/* メモリ領域のサイズ */

#ifdef __cplusplus
extern "C" {
#endif

extern void	cyclic_handler(intptr_t exinf);
extern void quicClient_main(intptr_t exinf);
//extern void cyclic_time(intptr_t exinf);
//extern void tsk_time(intptr_t exinf);

#ifdef __cplusplus
}
#endif

#endif /* _QUICCLIENT_H_ */
