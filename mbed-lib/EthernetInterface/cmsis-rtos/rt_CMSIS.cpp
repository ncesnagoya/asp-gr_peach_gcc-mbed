/* mbed includes */
#include "mbed_error.h"
#include "mbed_interface.h"
#include "us_ticker_api.h"

/* lwIP includes. */
#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"

#include "arch/sys_arch.h"

// Cast a CMSIS type to TOPPERS type
#define C2T_ID(id) ((ID)(id)) 
#define C2T_PRI(pri) (TMIN_TPRI-(pri)+3) // TODO: how to decide this mapping?

#define C2T_TMO(ms) (((ms) == osWaitForever) ? TMO_FEVR : (ms)) // millisec -> TMO
#define C2T_RELTIM(ms) ((RELTIM)(ms)) // millisec -> RELTIM

// ==== Thread Management ====

/// Create a thread and add it to Active Threads and set it to state READY

/*
 * Parameters
 * [in]	thread_def	thread definition referenced with osThread.
 * [in] argument pointer that is passed to the thread function as
 * start argument.
 *
 * Returns
 * thread ID for reference by other functions or NULL in case of
 * error.
 * 
 * Note
 * MUST REMAIN UNCHANGED: osThreadCreate shall be consistent in every
 * CMSIS-RTOS.
 *
 * Start a thread function by adding it to the Active Threads list and
 * set it to state READY. The thread function receives the argument
 * pointer as function argument when the function is started. When the
 * priority of the created thread function is higher than the current
 * RUNNING thread, the created thread function starts instantly and
 * becomes the new RUNNING thread.
 *
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument) {
    assert(thread_def->instances == 1);
    assert(thread_def->tpriority >= osPriorityIdle && thread_def->tpriority <= osPriorityRealtime);

    T_CTSK ctsk;
    ctsk.tskatr  = TA_ACT; 
    ctsk.exinf   = (intptr_t)argument;
    ctsk.task    = (TASK)thread_def->pthread;
	//    ctsk.itskpri = C2T_PRI(thread_def->tpriority);
	ctsk.itskpri = thread_def->tpriority;
    ctsk.stksz   = thread_def->stacksize ? thread_def->stacksize : DEFAULT_STACK_SIZE;
	ctsk.stk     = (STK_T*)thread_def->stack_pointer;
	//    ctsk.sstksz  = 0; // 4096/*DEFAULT_SSTKSZ*/; // FIXME: we are system tasks, we do not need sstk (hard coded, should we use DEFAULT_SSTKSZ?, or use a extern symbol to get this value
	//    ctsk.sstk    = NULL; // TODO: should we pass NULL to kernel?

    ER_ID ercd = acre_tsk(&ctsk);
    if (ercd <= 0) {
        return NULL;
    }
    return (osThreadId)ercd;
}

/// Terminate execution of a thread and remove it from ActiveThreads
/*
 * Parameters
 * [in]	thread_id	thread ID obtained by osThreadCreate or osThreadGetId.
 * Returns
 * status code that indicates the execution status of the function.
 * Note
 * MUST REMAIN UNCHANGED: osThreadTerminate shall be consistent in
 * every CMSIS-RTOS. Remove the thread function from the active
 * thread list. If the thread is currently RUNNING the execution will
 * stop.
 * 
 * Note
 * In case that osThreadTerminate terminates the currently running task, the function never returns and other threads that are in the READY state are started.
 * Status and Error Codes
 * - osOK: the specified thread has been successfully terminated.
 * - osErrorParameter: thread_id is incorrect.
 * - osErrorResource: thread_id refers to a thread that is not an
     active thread.
 * - osErrorISR: osThreadTerminate cannot be called from interrupt
     service routines.
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osStatus osThreadTerminate (osThreadId thread_id) {
    ER ercd;
    ID cur_tid;

    ercd = get_tid(&cur_tid);
    if (ercd == E_CTX) {
		return osErrorISR;
	} else if (ercd == E_PAR){
		return osErrorParameter;
	}

    if (cur_tid == C2T_ID(thread_id)) {
		ext_tsk();
	}

    ercd = ter_tsk(C2T_ID(thread_id));
    return osOK;
}

/*
 * Returns
 * thread ID for reference by other functions or NULL in case of
 * error.
 *
 * Note

 * MUST REMAIN UNCHANGED: osThreadGetId shall be consistent in every
 * CMSIS-RTOS. Get the thread ID of the current running thread.
 *
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osThreadId osThreadGetId (void)	 {
    ER ercd;	
    ID cur_tid;
    ercd = get_tid(&cur_tid);
    if (ercd < 0) {
		return NULL;
	}
	return (osThreadId)cur_tid;
}

// ==== Generic Wait Functions ====

/// Wait for Timeout (Time Delay)
/*
 * Parameters
 * [in]	millisec	time delay value
 * Returns
 * status code that indicates the execution status of the function.
 * Wait for a specified time period in millisec.
 *
 * The millisec value specifies the number of timer ticks and is
 * therefore an upper bound. The exact time delay depends on the
 * actual time elapsed since the last timer tick.
 *
 * For a value of 1, the system waits until the next timer tick
 * occurs. That means that the actual time delay may be up to one
 * timer tick less.
 *
 * Status and Error Codes
 * osEventTimeout: the time delay is executed.
 * osErrorISR: osDelay cannot be called from interrupt service routines.
 *
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osStatus osDelay (uint32_t millisec) {
    assert(millisec >= 1);

    ER ercd = dly_tsk(C2T_RELTIM(millisec));
    if (ercd == E_CTX) {
		return osErrorISR; // Not allowed in ISR
	}
    return osEventTimeout;
}

// ==== Mutex Management ====

/// Create and Initialize a Mutex object

/*
 *
 * Parameters
 * [in]	mutex_def	mutex definition referenced with osMutex.
 * Returns
 * mutex ID for reference by other functions or NULL in case of error.
 * 
 * Note
 * MUST REMAIN UNCHANGED: osMutexCreate shall be consistent in every
 * CMSIS-RTOS. Create and initialize a Mutex object.
 *
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osMutexId osMutexCreate (const osMutexDef_t *mutex_def) {
    T_CMTX cmtx;
    cmtx.mtxatr  = TA_NULL;
	cmtx.ceilpri = osPriorityHigh; /* TODO: to be changed to appropriate priority.*/

    ER_ID ercd = acre_mtx(&cmtx);
	if (ercd < 0) {
		return NULL;
	}
    return (osMutexId)ercd;
}

/// Wait until a Mutex becomes available
/*
 * Parameters
 * [in]	mutex_id	mutex ID obtained by osMutexCreate.
 * [in]	millisec	Timout Value or 0 in case of no time-out.
 * Returns
 * status code that indicates the execution status of the function.
 *
 * Note
 * MUST REMAIN UNCHANGED: osMutexWait shall be consistent in every
 * CMSIS-RTOS. Wait until a Mutex becomes available. If no other
 * thread has obtained the Mutex, the function instantly returns and
 * blocks the mutex object.
 *
 * The argument millisec specifies how long the system waits for a
 * mutex. While the system waits the thread that is calling this
 * function is put into the state WAITING. The millisec timeout can
 * have the following values:
 * - when millisec is 0, the function returns instantly.  
 * - when millisec is set to osWaitForever the function will wait for
 *   an infinite time until the mutex becomes available.
 * - all other values specify a time in millisecond for a timeout.
 *
 * Status and Error Codes 
 * - osOK: the mutex has been obtained.
 * - osErrorTimeoutResource: the mutex could not be obtained in the
 *   given time.
 * - osErrorResource: the mutex could not be obtained when no timeout
 *   was specified.
 * - osErrorParameter: the parameter mutex_id is incorrect.
 * - osErrorISR: osMutexWait cannot be called from interrupt service
 *   routines.
 * Note
 * Cannot be called from Interrupt Service Routines.
*/
osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec) {
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!, TMO in Gen3 kernel is microseconds

    ER ercd = tloc_mtx(C2T_ID(mutex_id), C2T_TMO(millisec));

	if (ercd == E_TMOUT) {
		return osErrorTimeoutResource;
	} else if (ercd == E_PAR){
		return osErrorParameter;
	} else if (ercd == E_CTX) {
		return osErrorISR;
	} else if (ercd < 0) {
		return osErrorResource; // This may be not correct implementation.
	}	
    return osOK;
}

/// Release a Mutex that was obtained with osMutexWait
/*
 * Parameters
 * [in]	mutex_id	mutex ID obtained by osMutexCreate.
 * Returns
 * status code that indicates the execution status of the function.
 * Note
 * MUST REMAIN UNCHANGED: osMutexRelease shall be consistent in every
 * CMSIS-RTOS.
 * 
 * Release a Mutex that was obtained with osMutexWait. Other threads
 * that currently wait for the same mutex will be now put into the
 * state READY.
 * 
 * Status and Error Codes
 * - osOK: the mutex has been correctly released.
 * - osErrorResource: the mutex was not obtained before.
 * - osErrorParameter: the parameter mutex_id is incorrect.
 * - osErrorISR: osMutexRelease cannot be called from interrupt
 *   service routines.
 *
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osStatus osMutexRelease (osMutexId mutex_id) {
    ER ercd = unl_mtx(C2T_ID(mutex_id));

	if (ercd == E_PAR){
		return osErrorParameter;
	} else if (ercd == E_CTX) {
		return osErrorISR;
	} else if (ercd < 0) {
		return osErrorResource; // This may be not correct implementation.
	}	
	
    return osOK;
}

/// Delete a Mutex that was created by osMutexCreate
/*
 * Parameters
 * [in]	mutex_id	mutex ID obtained by osMutexCreate.
 * Returns
 * status code that indicates the execution status of the function.
 *
 * Note
 * MUST REMAIN UNCHANGED: osMutexDelete shall be consistent in every
 * CMSIS-RTOS. Delete a Mutex object. The function releases internal
 * memory obtained for Mutex handling. After this call the mutex_id is
 * no longer valid and cannot be used. The Mutex may be created again
 * using the function osMutexCreate.
 *
 * Status and Error Codes
 * - osOK: the mutex object has been deleted.
 * - osErrorISR: osMutexDelete cannot be called from interrupt service
 *   routines.
 * - osErrorResource: all tokens have already been released.
 * - osErrorParameter: the parameter mutex_id is incorrect.
 * 
 * Note
 * Cannot be called from Interrupt Service Routines.
*/
osStatus osMutexDelete (osMutexId mutex_id) {
    ER ercd = del_mtx(C2T_ID(mutex_id));

	if (ercd == E_PAR){
		return osErrorParameter;
	} else if (ercd == E_CTX) {
		return osErrorISR;
	} else if (ercd < 0) {
		return osErrorResource; // This may be not correct implementation.
	}	
    return osOK;
}

// ==== Semaphore Management ====

/// Create and Initialize a Semaphore object
/*
 * Parameters
 * [in]	semaphore_def	semaphore definition referenced with osSemaphore.
 * [in]	count	number of available resources.
 * Returns
 * semaphore ID for reference by other functions or NULL in case of
 * error.
 * 
 * Note
 * MUST REMAIN UNCHANGED: osSemaphoreCreate shall be consistent in
 * every CMSIS-RTOS. Create and initialize a Semaphore object that is
 * used to manage access to shared resources. The parameter count
 * specifies the number of available resources. The count value 1
 * creates a binary semaphore.
 * 
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count) {
    assert(count <= osFeature_Semaphore);

    T_CSEM csem;
    csem.sematr  = TA_NULL;
    csem.isemcnt = count; // NOTE: count in mbed seems to be init value other than a max limit
	//    csem.maxsem  = osFeature_Semaphore; // = 65535 defined in cmsis_os.h
	csem.maxsem  = 1;
    ER_ID ercd = acre_sem(&csem);

	if (ercd < 0) {
		return NULL;
	}
    return (osSemaphoreId)ercd;
}

/// Wait until a Semaphore becomes available
/*

 * Parameters
 * [in]	semaphore_id	semaphore object referenced with osSemaphoreCreate.
 * [in]	millisec	Timout Value or 0 in case of no time-out.
 * Returns
 * number of available tokens, or -1 in case of incorrect parameters.
 * Note
 * MUST REMAIN UNCHANGED: osSemaphoreWait shall be consistent in every
 * CMSIS RTOS. Wait until a Semaphore token becomes available. When
 * no Semaphore token is available, the function waits for the time
 * specified with the parameter millisec.
 * 
 * The argument millisec specifies how long the system waits for a
 * Semaphore token to become available. While the system waits the
 * thread that is calling this function is put into the state
 * WAITING. The millisec timeout can have the following values:
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will wait for
 *   an infinite time until the Semaphore token becomes available.
 * - all other values specify a time in millisecond for a timeout.
 * 
 * The return value indicates the number of available tokens (the
 * semaphore count value). If 0 is returned, then no semaphore was
 * available.
 * 
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
int32_t osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec) {
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!, TMO in Gen3 kernel is microseconds

    ER ercd = twai_sem(C2T_ID(semaphore_id), C2T_TMO(millisec));
    // FIXME: only 0 or osFeature_Semaphore will be returned
    if (ercd == E_TMOUT) {
		return 0;
	} else if (ercd == E_PAR) {
		return -1;
	}
	//    return osFeature_Semaphore;
	return 1;
}

/// Release a Semaphore
/*
 * Parameters
 * [in]	semaphore_id	semaphore object referenced with osSemaphoreCreate.
 * Returns
 * status code that indicates the execution status of the function.
 * 
 * Note
 * MUST REMAIN UNCHANGED: osSemaphoreRelease shall be consistent in
 * every CMSIS-RTOS.  Release a Semaphore token. This increments the
 * count of available semaphore tokens.
 * 
 * Note
 * Interrupt Service Routines can call this function.
 *
 * Status and Error Codes
 * - osOK: the semaphore has been released.
 * - osErrorResource: all tokens have already been released.
 * - osErrorParameter: the parameter semaphore_id is incorrect
 */
osStatus osSemaphoreRelease (osSemaphoreId semaphore_id) {
	ER ercd;
	if (sns_ctx()) {
		ercd = isig_sem(C2T_ID(semaphore_id));
	} else {
		ercd = sig_sem(C2T_ID(semaphore_id));
	}

	if (ercd == E_QOVR) {
		return osErrorResource;
	} else if (ercd < 0) {
		return osErrorParameter;
	}
    return osOK;
}

/// Delete a Semaphore that was created by osSemaphoreCreate
/* 
 * Parameters
 * [in]	semaphore_id	semaphore object referenced with osSemaphoreCreate.
 * Returns
 * status code that indicates the execution status of the function.
 *
 * Note
 * MUST REMAIN UNCHANGED: osSemaphoreDelete shall be consistent in
 * every CMSIS-RTOS. Delete a Semaphore object. The function releases
 * internal memory obtained for Semaphore handling. After this call
 * the semaphore_id is no longer valid and cannot be used. The
 * Semaphore may be created again using the function
 * osSemaphoreCreate.
 *
 * Status and Error Codes
 * - osOK: the semaphore object has been deleted.
 * - osErrorISR: osSemaphoreDelete cannot be called from interrupt
 *   service routines.
 * - osErrorResource: the semaphore object could not be deleted.
 * - osErrorParameter: the parameter semaphore_id is incorrect.
 *
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osStatus osSemaphoreDelete (osSemaphoreId semaphore_id) {
    ER ercd = del_sem(C2T_ID(semaphore_id));

	if (ercd == E_ID || ercd == E_NOEXS){
		return osErrorParameter;
	} else if (ercd == E_CTX) {
		return osErrorISR;
	} else if (ercd < 0) {
		return osErrorResource;
	}	
    return osOK;
}

// ==== Message Queue Management Functions ====
/*
 * Parameters
 * [in]	queue_def	queue definition referenced with osMessageQ.
 * [in] thread_id thread ID (obtained by osThreadCreate or
 *      osThreadGetId) or NULL.
 * Returns
 * message queue ID for reference by other functions or NULL in case
 * of error.
 *
 * Note
 * MUST REMAIN UNCHANGED: osMessageCreate shall be consistent in every
 * CMSIS-RTOS. Create and initialize a message queue. The parameter
 * thread_id registers the receiving thread for a message and is
 * needed for the general osWait function to deliver the message.
 *
 * Note
 * Cannot be called from Interrupt Service Routines.
 */
osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) {
    assert(thread_id == NULL);   

    T_CDTQ cdtq;
    cdtq.dtqatr = TA_NULL; // FIFO
    cdtq.dtqcnt = queue_def->queue_sz;
    cdtq.dtqmb  = queue_def->pool;
	//   assert(cdtq.dtqmb != NULL); // Check: This assert is needed or not.

    ER_ID ercd = acre_dtq(&cdtq);
    if (ercd <= 0) {
        syslog(LOG_ERROR, "%s() called, cnt:%d, mb:0x%p", __FUNCTION__, cdtq.dtqcnt, cdtq.dtqmb);
		return NULL;
    }
    return (osMessageQId)ercd;
}

/*
 * Parameters
 * [in]	queue_id	message queue ID obtained with osMessageCreate.
 * [in]	info	message information.
 * [in]	millisec	Timout Value or 0 in case of no time-out.
 * 
 * Returns
 * status code that indicates the execution status of the function.
 * 
 * Note
 * MUST REMAIN UNCHANGED: osMessagePut shall be consistent in every
 * CMSIS-RTOS.  Put the message info in a message queue specified by
 * queue_id.
 * 
 * When the message queue is full, the system retries for a specified
 * time with millisec. While the system retries the thread that is
 * calling this function is put into the state WAITING. The millisec
 * timeout can have the following values:
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will wait for
 *   an infinite time until a message queue slot becomes available.
 * - all other values specify a time in millisecond for a timeout.
 * 
 * Note
 * The parameter millisec must be 0 for using this function in an ISR.
 * Interrupt Service Routines can call this function.
 *
 * Status and Error Codes
 * - osOK: the message is put into the queue.
 * - osErrorResource: no memory in the queue was available.
 * - osErrorTimeoutResource: no memory in the queue was available
 *   during the given time limit.
 * - osErrorParameter: a parameter is invalid or outside of a
 *   permitted range.
 */
osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!, TMO in Gen3 kernel is microseconds
	ER ercd;
	if (sns_ctx()) {
		ercd = ipsnd_dtq(C2T_ID(queue_id), info);
	} else {
		ercd = tsnd_dtq(C2T_ID(queue_id), info, C2T_TMO(millisec));
	}

	if (ercd == E_TMOUT) {
		return osErrorTimeoutResource;
	} else if (ercd == E_PAR){
		return osErrorParameter;
	} else if (ercd < 0) {
		return osErrorResource; // This may be not correct implementation.
	}	
	return osOK;
}

/*
 * Parameters
 * [in]	queue_id	message queue ID obtained with osMessageCreate.
 * [in]	millisec	Timout Value or 0 in case of no time-out.
 * Returns
 * event information that includes status code.
 * 
 * Note
 * MUST REMAIN UNCHANGED: osMessageGet shall be consistent in every
 * CMSIS-RTOS.  Suspend the execution of the current RUNNING thread
 * until a message arrives. When a message is already in the queue,
 * the function returns instantly with the message information.
 * 
 * The argument millisec specifies how long the system waits for a
 * message to become available. While the system waits the thread that
 * is calling this function is put into the state WAITING. The
 * millisec timeout value can have the following values:
 * - when millisec is 0, the function returns instantly.
 * - when millisec is set to osWaitForever the function will wait for
 *   an infinite time until a message arrives.
 * - all other values specify a time in millisecond for a timeout.
 *
 * Note
 * The parameter millisec must be 0 for using this function in an ISR.
 * Interrupt Service Routines can call this function.
 *
 * Status and Error Codes
 * - osOK: no message is available in the queue and no timeout was
 *   specified.
 * - osEventTimeout: no message has arrived during the given timeout
 *   period.
 * - osEventMessage: message received, value.p contains the pointer to
 *   message.
 * - osErrorParameter: a parameter is invalid or outside of a permitted
 *   range.
 */
os_InRegs osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec) {
    osEvent ret;
    assert(millisec < (1UL << (sizeof(TMO) * 8 - 1)) || millisec == osWaitForever); // FIXME: TMO is not unsigned!, TMO in Gen3 kernel is microseconds

	ER ercd;
	if (sns_ctx()) {
		// No API for receiving a message, that can be called in ISR
		// context in TOPPERS OS. This is not correct implementation.
		ret.status = osErrorParameter;		
	} else {
		ercd = trcv_dtq(C2T_ID(queue_id), (intptr_t*)(&(ret.value.v)), C2T_TMO(millisec));		
	
		if (ercd == E_TMOUT) {
			ret.status = millisec ? osEventTimeout : osOK;
		} else {
			ret.status = osEventMessage;
		}
	}	
    return ret;
}
