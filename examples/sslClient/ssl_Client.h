#ifndef _HTTPSAMPLE_H_
#define _HTTPSAMPLE_H_

#ifndef HTTP_MAIN_TASK_PRI
#define HTTP_MAIN_TASK_PRI  4
#endif /* HTTP_MAIN_TASK_PRI */

#ifndef HTTP_MAIN_TASK_STACK_SIZE
#define HTTP_MAIN_TASK_STACK_SIZE 1024 *  24
#endif  /* HTTP_MAIN_TASK_STACK_SIZE */

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
