/******************************************************************************
 *
 * $Rev: 1674 $
 * $Date: 2012-12-18 11:09:19 +0900#$
 *
 * Description : ITRON support functions for IOIF Asynchronous I/O header file
 *
 * (C) Copyright RENESAS ELECTRONICS EUROPE Ltd 2012 All Rights Reserved
 *****************************************************************************/

/*************************************************************************
 System Includes
*************************************************************************/

#include <r_errno.h>
#include <cmsis_os.h>
#include <aioif.h>
#if(1) /* mbed */
#include "cmsis.h"
#else  /* not mbed */
#include <ipcb.h>
#include <ioif_aio_helper.h>
#endif /* end mbed */
#include <misratypes.h>
#include "bsp_drv_cmn.h"
#if defined (__ICCARM__)
#include <intrinsics.h>
#include "Renesas_RZ_A1.h"
#endif

/*************************************************************************
 OS Resources
*************************************************************************/


/*************************************************************************
 Functions
*************************************************************************/
static void ahf_lock(AHF_S * const ahf);
static void ahf_unlock(AHF_S * const ahf);

static void ahf_lock(AHF_S * const ahf)
{
    if(ahf->flags & AHF_LOCKSEM)
    {
        osMutexWait(ahf->semid, 0);
    }
    else if (ahf->flags & AHF_LOCKINT)
    {
#if defined (__ICCARM__)
        ahf->saved_int_mask = __disable_irq_iar();
#else
        ahf->saved_int_mask = __disable_irq();
#endif
    }
    else
    {
        ;   /* MISRA compliance. */
    }
}

static void ahf_unlock(AHF_S * const ahf)
{
    if(ahf->flags & AHF_LOCKSEM)
    {
        osMutexRelease(ahf->semid);
    }
    else if (ahf->flags & AHF_LOCKINT)
    {
        if (0 == ahf->saved_int_mask)
        {
            __enable_irq();
        }
    }
    else
    {
        ;   /* MISRA compliance. */
    }
}

/***********************************************************************************
Function Name:         ahf_create

Description:    Creates an empty aio control queue pointer.
                Creates a mutex if AHF_CREATESEM bit flag is set.

Parameters:     ahf  - aio queue structure pointer.
                f    - flag indicating that at semaphore is to be created.

Return value:   0 on success.   negative error code on error.
***********************************************************************************/
int32_t ahf_create (AHF_S * const ahf, const uint32_t f)
{
    osMutexDef_t* p_mutex_def;
    uint32_t*     p_mutex_data;
#if defined (__GNUC__)
    int_t was_masked;
#endif/*__GNUC__*/

    if (ahf == NULL)
    {
        return EFAULT;
    }

    ahf->head = NULL;
    ahf->tail = NULL;
    ahf->flags = f;

    /* create the mutex if required */
    if (f & AHF_CREATESEM)
    {
#if defined (__GNUC__)
        /* disable all irq */
        was_masked = __disable_irq();
#endif/*__GNUC__*/
        p_mutex_def = calloc(1, sizeof(osMutexDef_t));
#if defined (__GNUC__)
        if (0 == was_masked)
        {
            /* enable all irq */
            __enable_irq();
        }
#endif/*__GNUC__*/
        if ( NULL == p_mutex_def )
        {
            return ENOMEM;
        }
#if defined (__GNUC__)
        /* disable all irq */
        was_masked = __disable_irq();
#endif/*__GNUC__*/
        p_mutex_data = calloc(3, sizeof(uint32_t));
#if defined (__GNUC__)
        if (0 == was_masked)
        {
            /* enable all irq */
            __enable_irq();
        }
#endif/*__GNUC__*/
        if ( NULL == p_mutex_data )
        {
#if defined (__GNUC__)
            /* disable all irq */
            was_masked = __disable_irq();
#endif/*__GNUC__*/
            free(p_mutex_def);
#if defined (__GNUC__)
            if (0 == was_masked)
            {
                /* enable all irq */
                __enable_irq();
            }
#endif/*__GNUC__*/
            return ENOMEM;
        }
        p_mutex_def->mutex = p_mutex_data;
        ahf->p_cmtx = p_mutex_def;
        ahf->semid = osMutexCreate (p_mutex_def);
        if ( NULL == ahf->semid )
        {
#if defined (__GNUC__)
            /* disable all irq */
            was_masked = __disable_irq();
#endif/*__GNUC__*/
            free(p_mutex_data);
            free(p_mutex_def);
#if defined (__GNUC__)
            if (0 == was_masked)
            {
                /* enable all irq */
                __enable_irq();
            }
#endif/*__GNUC__*/
            return ENOMEM;
        }
    }

    return 0;
}

/***********************************************************************************
Function Name:         ahf_destroy

Description:    Delete aio control block mutex (if it exists).
                Note: This function does not delete the aio control block queue.

Parameters:     ahf  - aio queue structure pointer.

Return value:   void

***********************************************************************************/
void ahf_destroy (AHF_S const * const ahf)
{
#if defined (__GNUC__)
    int_t was_masked;
#endif/*__GNUC__*/

    if (ahf == NULL)
    {
        return; 
    }
     
    if (ahf->flags & AHF_CREATESEM)
    {
        osMutexDelete (ahf->semid);
#if defined (__GNUC__)
        /* disable all irq */
        was_masked = __disable_irq();
#endif/*__GNUC__*/
        free(ahf->p_cmtx->mutex);
        free(ahf->p_cmtx);
#if defined (__GNUC__)
        if (0 == was_masked)
        {
            /* enable all irq */
            __enable_irq();
        }
#endif/*__GNUC__*/
    }
}

/***********************************************************************************
Function Name:         ahf_addtail

Description:    Add an aio control block to the queue.

Parameters:     ahf  - aio queue structure pointer.
                aio  - pointer to queue structure.

Return value:   void

***********************************************************************************/
void ahf_addtail (AHF_S * const ahf, struct aiocb * const aio)
{
    if (ahf == NULL)
    {
        return; 
    }
     
    ahf_lock (ahf);
    if (ahf->tail != NULL)
    {
        ahf->tail->pNext = aio;
    }
    aio->pPrev = ahf->tail;
    aio->pNext = NULL;
    ahf->tail = aio;
    if (ahf->head == NULL)
    {
        /* list was empty */
        ahf->head = aio;
    }

    ahf_unlock (ahf);
}

/***********************************************************************************
Function Name:         ahf_removehead

Description:    Remove an aio control block from the queue.

Parameters:     ahf  - aio queue structure pointer.

Return value:   aio control block, or NULL if queue is empty.

***********************************************************************************/
struct aiocb *ahf_removehead (AHF_S * const ahf)
{
    struct aiocb *aio;
    
    if (ahf == NULL)
    {
        return(NULL);   
    }
     
    ahf_lock (ahf);

    aio = ahf->head;
    if (aio != NULL)
    {
        ahf->head = aio->pNext;
        if (aio->pNext != NULL)
        {
            aio->pNext->pPrev = NULL;
        }
        if (ahf->tail == aio)
        {
            /* the list is now empty */
            ahf->tail = NULL;
        }
    }

    ahf_unlock (ahf);
    return aio;
}

/***********************************************************************************
Function Name:         ahf_peekhead

Description:    Get an aio control block from the queue, but do not remove from the queue.

Parameters:     ahf  - aio queue structure pointer.

Return value:   aio control block, or NULL if queue is empty.

***********************************************************************************/
struct aiocb *ahf_peekhead (AHF_S * const ahf)
{
    struct aiocb *aio;

    ahf_lock (ahf);

    aio = ahf->head;

    ahf_unlock (ahf);
    return aio;
}

/***********************************************************************************
Function Name:         ahf_cancelall

Description:    Empty an aio control block queue.

Parameters:     ahf  - aio queue structure pointer.

Return value:   void

***********************************************************************************/
void ahf_cancelall (AHF_S * const ahf)
{
    struct aiocb *cur, *next;

    ahf_lock (ahf);

    /* cancel all pending requests */
    cur = ahf->head;
    while (cur != NULL)
    {
        next = cur->pNext;
        cur->aio_return = ECANCELED;
        ahf_complete (ahf, cur);
        cur = next;
    }
    /* mark the list as empty */
    ahf->head = NULL;
    ahf->tail = NULL;

    ahf_unlock (ahf);
}

/***********************************************************************************
Function Name:         ahf_complete

Description:    Flag that the operation is complete.
                If required by the sigev_notify variable contents, the calling task is
                notified.
                If a notify was setup, it is signalled.
                
Parameters:     ahf  - aio queue structure pointer.
                aio  - pointer to queue structure.

Return value:   0 on success.   negative error code on error.

***********************************************************************************/
void ahf_complete (AHF_S *ahf, struct aiocb * const aio)
{
#if(1) /* mbed */
#else  /* not mbed */
    int rv;
#endif /* end mbed */
    UNUSED_ARG(ahf);

    if (NULL == aio)
    {
        return;
    }

    /* Flag that the operation is complete */
    /*  This function must be here
        because another task may release AIOCB block
        after the task changed by an iTRON function in the switch statement */
    aio->aio_complete = 1;
    switch (aio->aio_sigevent.sigev_notify)
    {
    case SIGEV_EVENT:
         osSignalSet ((osThreadId)aio->aio_sigevent.sigev_value.sival_int,
             (int32_t)aio->aio_sigevent.sigev_signo);
        break;

    case SIGEV_THREAD:
        if (aio->aio_sigevent.sigev_notify_function)
        {
            (aio->aio_sigevent.sigev_notify_function)
                (aio->aio_sigevent.sigev_value);
        }
        break;

#if(1) /* mbed */
#else  /* not mbed */
    case SIGEV_CALLBACK:
        rv = ipcb_callback ((ipcb_id_t*)aio->aio_sigevent.sigev_signo, &aio->aio_sigevent);
        break;
#endif /* end mbed */

    default:
        /* No notify function */
        break;

    }

    /* Wakeup any suspended tasks */
#if(1) /* mbed */
#else  /* not mbed */
    aio_complete_suspended(aio);
#endif /* end mbed */
}

/***********************************************************************************
Function Name:         ahf_cancel

Description:    Remove an aio control block from the queue.
                Signal completion to the calling task and wake it up.
                If aio is NULL, cancel all.
                
Parameters:     ahf  - aio queue structure pointer.
                aio  - pointer to queue structure.

Return value:   0 on success.   negative error code on error.

***********************************************************************************/
int32_t ahf_cancel (AHF_S * const ahf, struct aiocb * const aio)
{
    struct aiocb *cur;
    int32_t rv = EINVAL;

    if (ahf == NULL)
    {
        return EFAULT;  
    }
     
    /* If aio is NULL, must cancel all. */
    if(NULL == aio)
    {
        ahf_cancelall(ahf);
        rv = 0;
    }
    
    else
    {
        ahf_lock (ahf);

        cur = ahf->head;
        while ((cur != NULL) && (cur != aio))
        {
            cur = cur->pNext;
        }

        if (cur == aio)
        {
            if (aio->pPrev != NULL)
            {
                aio->pPrev->pNext = aio->pNext;
            }

            if (aio->pNext != NULL)
            {
                aio->pNext->pPrev = aio->pPrev;
            }

            if (ahf->head == cur)
            {
                ahf->head = cur->pNext;
            }

            if (ahf->tail == cur)
            {
                ahf->tail = cur->pPrev;
            }

            cur->aio_return = ECANCELED;
            ahf_complete (ahf, aio);
            rv = 0;
        }

        ahf_unlock (ahf);
    }
    return rv;
}
