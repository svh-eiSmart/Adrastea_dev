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

#ifndef __MODULES_LTE_ALTCOM_INCLUDE_API_LTE_APICMD_SETPSM_H
#define __MODULES_LTE_ALTCOM_INCLUDE_API_LTE_APICMD_SETPSM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "apicmd.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define APICMD_SETPSM_DISABLE (0)
#define APICMD_SETPSM_ENABLE_FORCE_OFF (1)
#define APICMD_SETPSM_ENABLE_FORCE_ON (2)

#define APICMD_SETPSM_TIMER_MIN (1)
#define APICMD_SETPSM_TIMER_MAX (31)

/* Unit value of Requested Active Time */

#define APICMD_SETPSM_RAT_UNIT_2SEC (0)
#define APICMD_SETPSM_RAT_UNIT_1MIN (1)
#define APICMD_SETPSM_RAT_UNIT_6MIN (2)

/* Unit value of extended periodic Tracking Area Update */

#define APICMD_SETPSM_TAU_UNIT_2SEC (0)
#define APICMD_SETPSM_TAU_UNIT_30SEC (1)
#define APICMD_SETPSM_TAU_UNIT_1MIN (2)
#define APICMD_SETPSM_TAU_UNIT_10MIN (3)
#define APICMD_SETPSM_TAU_UNIT_1HOUR (4)
#define APICMD_SETPSM_TAU_UNIT_10HOUR (5)
#define APICMD_SETPSM_TAU_UNIT_320HOUR (6)

#define APICMD_SETPSM_RES_OK (0)
#define APICMD_SETPSM_RES_ERR (1)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* This structure discribes the data structure of the API command */

/* APICMDID_GET_PSM */

begin_packed_struct struct apicmd_cmddat_setpsm_timeval_s {
  uint8_t unit;
  uint8_t time_val;
} end_packed_struct;

begin_packed_struct struct apicmd_cmddat_setpsm_s {
  uint8_t func;
  struct apicmd_cmddat_setpsm_timeval_s at_val;
  struct apicmd_cmddat_setpsm_timeval_s tau_val;
} end_packed_struct;

/* APICMDID_GET_PSM_RES */

begin_packed_struct struct apicmd_cmddat_setpsmres_s { uint8_t result; } end_packed_struct;

#endif /* __MODULES_LTE_ALTCOM_INCLUDE_API_LTE_APICMD_SETPSM_H */
