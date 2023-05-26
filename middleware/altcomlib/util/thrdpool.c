/*  ---------------------------------------------------------------------------

    (c) copyright 2021 Altair Semiconductor, Ltd. All rights reserved.

    This software, in source or object form (the "Software"), is the
    property of Altair Semiconductor Ltd. (the "Company") and/or its
    licensors, which have all right, title and interest therein, You
    may use the Software only in  accordance with the terms of written
    license agreement between you and the Company (the "License").
    Except as expressly stated in the License, the Company grants no
    licenses by implication, estoppel, or otherwise. If you are not
    aware of or do not agree to the License terms, you may not use,
    copy or modify the Software. You may use the source code of the
    Software only for your internal purposes and may not distribute the
    source code of the Software, any part thereof, or any derivative work
    thereof, to any third party, except pursuant to the Company's prior
    written consent.
    The Software is the confidential information of the Company.

   ------------------------------------------------------------------------- */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "alt_osal.h"
#include "thrdpool.h"
#include "dbg_if.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define THRDPOOL_THRDNAME_MAX_LEN (24)
#define THRDPOOL_CONDWAIT(handle, timeout)     \
  do {                                         \
    alt_osal_wait_semaphore(&handle, timeout); \
  } while (0)
#define THRDPOOL_CONDSIGNAL(handle)   \
  do {                                \
    alt_osal_post_semaphore(&handle); \
  } while (0)
#define THRDPOOL_INIT_BIN_SEM_SET(param) \
  {                                      \
    (param).initial_count = 0;           \
    (param).max_count = 1;               \
  }
/****************************************************************************
 * Private Types
 ****************************************************************************/

enum thrdpool_thrdstate_e { THRDPOOL_WAITING = 0, THRDPOOL_RUNNABLE, THRDPOOL_TERMINATING };

struct thrdpool_queelements_s {
  CODE thrdpool_jobif_t job;
  FAR void *arg;
};

struct thrdpool_share_s {
  alt_osal_queue_handle quehandle;
  alt_osal_thread_cond_handle delwaitcond;
  alt_osal_mutex_handle delwaitcondmtx;
};

struct thrdpool_info_s {
  FAR struct thrdpool_share_s *share;
  alt_osal_task_handle thrdhandle;
  enum thrdpool_thrdstate_e state;
};

struct thrdpool_datatable_s {
  FAR struct thrdpool_s thrdpoolif;
  uint16_t maxthrdnum;
  uint16_t maxquenum;
  struct thrdpool_share_s share;
  FAR struct thrdpool_info_s *thrdinfolist;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int32_t thrdpool_runjob(FAR struct thrdpool_s *thiz, CODE thrdpool_jobif_t job,
                               FAR void *arg);
static uint32_t thrdpool_getfreethrds(FAR struct thrdpool_s *thiz);
static void thrdpool_thrdmain(FAR void *arg);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: thrdpool_runjob
 *
 * Description:
 *   Enqueues the processing that the thread does.
 *
 * Input Parameters:
 *   thiz  struct thrdpool_s pointer(i.e. instance of threadpool).
 *   job   Pointer to the processing function conforming to the job_if.
 *   arg   argument of @job.
 *
 * Returned Value:
 *   If the process succeeds, it returns 0.
 *   Otherwise errno is returned.
 *
 ****************************************************************************/

static int32_t thrdpool_runjob(FAR struct thrdpool_s *thiz, CODE thrdpool_jobif_t job,
                               FAR void *arg) {
  FAR struct thrdpool_datatable_s *table = NULL;
  struct thrdpool_queelements_s element;

  if (!thiz || !job) {
    DBGIF_LOG_ERROR("Incorrect argument.\n");
    return -EINVAL;
  }

  table = (FAR struct thrdpool_datatable_s *)thiz;
  element.job = job;
  element.arg = arg;

  alt_osal_send_mqueue(&table->share.quehandle, (FAR int8_t *)&element,
                       sizeof(struct thrdpool_queelements_s), ALT_OSAL_TIMEO_FEVR);

  return 0;
}

/****************************************************************************
 * Name: thrdpool_getfreethrds
 *
 * Description:
 *   Get number of available threads.
 *
 * Input Parameters:
 *   thiz  struct thrdpool_s pointer(i.e. instance of threadpool).
 *
 * Returned Value:
 *   Number of available threads.
 *
 ****************************************************************************/

static uint32_t thrdpool_getfreethrds(FAR struct thrdpool_s *thiz) {
  FAR struct thrdpool_datatable_s *table = NULL;
  uint16_t num = 0;
  uint16_t count = 0;

  if (!thiz) {
    DBGIF_LOG_ERROR("thiz pointer is NULL.\n");
    return 0;
  }

  table = (FAR struct thrdpool_datatable_s *)thiz;
  for (num = 0; num < table->maxthrdnum; num++) {
    if (table->thrdinfolist[num].state == THRDPOOL_WAITING) {
      count++;
    }
  }

  return count;
}

/****************************************************************************
 * Name: thrdpool_thrdmain
 *
 * Description:
 *   The main loop of the thread to create.
 *
 * Input Parameters:
 *   arg  Information for the thread to operate.(i.e. struct thrdpool_info_s)
 *
 * Returned Value:
 *   None.
 *
 ****************************************************************************/

static void thrdpool_thrdmain(FAR void *arg) {
  FAR struct thrdpool_info_s *info = (FAR struct thrdpool_info_s *)arg;
  struct thrdpool_queelements_s recvbuf;

  while (1) {
    recvbuf.job = NULL;
    recvbuf.arg = NULL;
    if (alt_osal_recv_mqueue(&info->share->quehandle, (FAR int8_t *)&recvbuf,
                             sizeof(struct thrdpool_queelements_s), ALT_OSAL_TIMEO_FEVR) < 0) {
      DBGIF_LOG_ERROR("Receive failed from queue.\n");
      continue;
    }

    if (recvbuf.job == NULL) {
      /* Receive delete packet. */
      alt_osal_lock_mutex(&info->share->delwaitcondmtx, ALT_OSAL_TIMEO_FEVR);
      break;
    }

    info->state = THRDPOOL_RUNNABLE;

    /* Perform actual processing. */

    recvbuf.job(recvbuf.arg);

    info->state = THRDPOOL_WAITING;
  }

  info->state = THRDPOOL_TERMINATING;
  alt_osal_signal_thread_cond(&info->share->delwaitcond);
  alt_osal_unlock_mutex(&info->share->delwaitcondmtx);
  alt_osal_delete_task(ALT_OSAL_OWN_TASK);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: thrdpool_create
 *
 * Description:
 *   Create an object of threadpool and get the instance.
 *
 * Input Parameters:
 *   set  Threadpool create setting.
 *
 * Returned Value:
 *   struct thrdpool_s pointer(i.e. instance of threadpool).
 *
 ****************************************************************************/

FAR struct thrdpool_s *thrdpool_create(FAR struct thrdpool_set_s *set) {
  static uint16_t thrdcount = 0;
  FAR struct thrdpool_datatable_s *table = NULL;
  uint16_t num = 0;
  alt_osal_task_attribute thread_param;
  alt_osal_queue_attribute que_param;
  char thrdname[THRDPOOL_THRDNAME_MAX_LEN];

  if (!set || set->maxthrdnum <= 0 || set->maxquenum <= 0) {
    DBGIF_LOG_ERROR("Incorrect argument.\n");
    errno = EINVAL;
    goto errout;
  }

  /* Create data table. */

  table = (FAR struct thrdpool_datatable_s *)ALT_OSAL_MALLOC(sizeof(struct thrdpool_datatable_s));
  if (!table) {
    DBGIF_LOG_ERROR("thrdpool_s table allocate failed.\n");
    errno = ENOMEM;
    goto errout;
  }

  memset(table, 0, sizeof(*table));
  table->maxthrdnum = set->maxthrdnum;

  /* Set interface. */

  table->thrdpoolif.runjob = thrdpool_runjob;
  table->thrdpoolif.getfreethrds = thrdpool_getfreethrds;

  /* Create queue. */
  memset((void *)&que_param, 0, sizeof(alt_osal_queue_attribute));
  que_param.numof_queue = set->maxquenum;
  que_param.queue_size = sizeof(struct thrdpool_queelements_s);
  if (alt_osal_create_mqueue(&table->share.quehandle, &que_param) < 0) {
    DBGIF_LOG_ERROR("Queue create failed.\n");
    goto errout_with_tablefree;
  }

  if (alt_osal_create_thread_cond_mutex(&table->share.delwaitcond, &table->share.delwaitcondmtx) <
      0) {
    DBGIF_LOG_ERROR("alt_osal_create_thread_cond_mutex failed.\n");
    goto errout_with_quedelete;
  }

  /* Create threads data. */

  memset((void *)&thread_param, 0, sizeof(alt_osal_task_attribute));
  thread_param.function = thrdpool_thrdmain;
  thread_param.name = (FAR char *)thrdname;
  thread_param.priority = set->thrdpriority;
  thread_param.stack_size = set->thrdstacksize;
  table->thrdinfolist = (FAR struct thrdpool_info_s *)ALT_OSAL_MALLOC(
      sizeof(struct thrdpool_info_s) * table->maxthrdnum);
  if (!table->thrdinfolist) {
    DBGIF_LOG_ERROR("thrdinfolist create failed.\n");
    goto errout_with_semdelete;
  }

  /* Create threads */

  for (num = 0; num < table->maxthrdnum; num++) {
    table->thrdinfolist[num].share = &table->share;
    table->thrdinfolist[num].state = THRDPOOL_WAITING;
    thread_param.arg = (FAR void *)&table->thrdinfolist[num];
    snprintf(thrdname, sizeof(thrdname), "thrdpool_no%02d", (int)(++thrdcount));
    if (alt_osal_create_task(&table->thrdinfolist[num].thrdhandle, &thread_param) < 0) {
      DBGIF_LOG1_ERROR("thread create failed. number:%d\n", num);
      goto errout_with_thrddelete;
    }
  }

  return (FAR struct thrdpool_s *)table;

errout_with_thrddelete:
  for (; 0 < num; num--) {
    alt_osal_delete_task(&table->thrdinfolist[num - 1].thrdhandle);
  }

  ALT_OSAL_FREE(table->thrdinfolist);
errout_with_semdelete:
  alt_osal_delete_thread_cond_mutex(&table->share.delwaitcond, &table->share.delwaitcondmtx);
errout_with_quedelete:
  alt_osal_delete_mqueue(&table->share.quehandle);
errout_with_tablefree:
  ALT_OSAL_FREE(table);
  errno = ENOMEM;
errout:
  return NULL;
}

/****************************************************************************
 * Name: thrdpool_delete
 *
 * Description:
 *   Delete object of threadpool.
 *
 * Input Parameters:
 *   thiz  struct thrdpool_s pointer(i.e. instance of threadpool).
 *
 * Returned Value:
 *   If the process succeeds, it returns 0.
 *   Otherwise errno is returned.
 *
 ****************************************************************************/

int32_t thrdpool_delete(FAR struct thrdpool_s *thiz) {
  FAR struct thrdpool_datatable_s *table = NULL;
  uint16_t num = 0;
  struct thrdpool_queelements_s element;
  int32_t ret = 0;

  if (!thiz) {
    DBGIF_LOG_ERROR("thiz pointer is NULL.\n");
    return -EINVAL;
  }

  table = (FAR struct thrdpool_datatable_s *)thiz;
  element.job = NULL;

  for (num = 0; num < table->maxthrdnum; num++) {
    /* Send delete request to thread main */
    ret = alt_osal_send_mqueue(&table->share.quehandle, (FAR int8_t *)&element,
                               sizeof(struct thrdpool_queelements_s), ALT_OSAL_TIMEO_FEVR);
    DBGIF_ASSERT(0 == ret, "Queue send failed.");
  }

  alt_osal_lock_mutex(&table->share.delwaitcondmtx, ALT_OSAL_TIMEO_FEVR);
  do {
    for (num = 0; num < table->maxthrdnum; num++) {
      if (THRDPOOL_TERMINATING != table->thrdinfolist[num].state) {
        break;
      }
    }

    if (num == table->maxthrdnum) {
      break;
    }
  } while (0 == (ret = alt_osal_thread_cond_wait(&table->share.delwaitcond,
                                                 &table->share.delwaitcondmtx)));

  alt_osal_unlock_mutex(&table->share.delwaitcondmtx);
  DBGIF_LOG_DEBUG("All thread delete success.\n");
  alt_osal_delete_thread_cond_mutex(&table->share.delwaitcond, &table->share.delwaitcondmtx);
  alt_osal_delete_mqueue(&table->share.quehandle);
  ALT_OSAL_FREE(table->thrdinfolist);
  ALT_OSAL_FREE(table);
  return 0;
}
