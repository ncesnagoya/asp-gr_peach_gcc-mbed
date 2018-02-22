#ifndef _SSLCLIENT_H_
#define _SSLCLIENT_H_

#ifndef SSL_MAIN_TASK_PRI
#define SSL_MAIN_TASK_PRI  4
#endif /* SSL_MAIN_TASK_PRI */

#ifndef SSL_MAIN_TASK_STACK_SIZE
#define SSL_MAIN_TASK_STACK_SIZE 1024 *  24
#endif  /* SSL_MAIN_TASK_STACK_SIZE */

#ifndef KMM_SIZE
#define	KMM_SIZE	(1024 * 32)	/* カーネルが割り付ける */
#endif /* KMM_SIZE */						/* メモリ領域のサイズ */

#ifdef __cplusplus
extern "C" {
#endif

extern void	cyclic_handler(intptr_t exinf);
extern void sslClient_main(intptr_t exinf);

#ifdef __cplusplus
}
#endif

#endif /* _HTTPSAMPLE_H_ */
