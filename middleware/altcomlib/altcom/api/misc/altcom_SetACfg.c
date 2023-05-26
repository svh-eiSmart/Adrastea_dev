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
#include <string.h>
#include <stdbool.h>

#include "dbg_if.h"
#include "apicmd.h"
#include "altcom_misc.h"
#include "buffpoolwrapper.h"
#include "apiutil.h"
#include "altcom_cc.h"
#include "apicmd_SetACfg.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define SETACFG_DATA_LEN (sizeof(struct apicmd_setacfg_s))
#define SETACFG_RESP_LEN (sizeof(struct apicmd_setacfgres_s))

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief Set configuration parameter through %SETACFG command.
 *
 * @param [in] cfgName: The configuration name in "Config.Section.Option" format. It must to be a
 * NULL-Terminated string, and length should less than @ref MISC_MAX_CFGNAME_LEN.
 * @param [inout] cfgValue: The configuration value to be set to cfgName. Please note that the
 * string length should never larger than @ref MISC_MAX_CFGVALUE_LEN. It must to be a
 * NULL-Terminated string.
 *
 * @return
 * Error Code                | Reason/Cause
 * --------------------------| --------------------------
 * MISC_SUCCESS              | Operation succeeded
 * MISC_FAILURE              | General failure
 */

Misc_Error_e altcom_SetACfg(const char *cfgName, char *cfgValue) {
  uint16_t cfgNameLen, cfgValueLen = 0;

  /* Return error if parameter is invalid */
  if (!cfgName) {
    DBGIF_LOG_ERROR("Input argument is NULL.\n");
    return MISC_FAILURE;
  }

  cfgNameLen = strlen(cfgName);
  if (0 == cfgNameLen || MISC_MAX_CFGNAME_LEN - 1 < cfgNameLen) {
    DBGIF_LOG1_ERROR("Invalid cfgName length %hu\n", cfgNameLen);
    return MISC_FAILURE;
  }

  if (cfgValue) {
    cfgValueLen = strlen(cfgValue);
    if (MISC_MAX_CFGNAME_LEN - 1 < cfgValueLen) {
      DBGIF_LOG1_ERROR("Invalid cfgValue length %hu\n", cfgValueLen);
      return MISC_FAILURE;
    }
  }

  /* Check if the library is initialized */
  if (!altcom_isinit()) {
    DBGIF_LOG_ERROR("Not intialized\n");
    return MISC_FAILURE;
  }

  int32_t ret;
  uint16_t resLen = 0;
  FAR struct apicmd_setacfg_s *cmd = NULL;
  FAR struct apicmd_setacfgres_s *res = NULL;
  Misc_Error_e result = MISC_FAILURE;

  /* Allocate API command buffer to send */
  if (altcom_generic_alloc_cmdandresbuff((FAR void **)&cmd, APICMDID_MISC_SETACFG, SETACFG_DATA_LEN,
                                         (FAR void **)&res, SETACFG_RESP_LEN)) {
    memcpy(cmd->cfgName, cfgName, (size_t)cfgNameLen);
    cmd->cfgName[cfgNameLen] = 0;
    if (cfgValueLen) {
      memcpy(cmd->cfgValue, cfgValue, (size_t)cfgValueLen);
      cmd->cfgValue[cfgValueLen] = 0;
    }

    /* Send API command to modem */
    ret = apicmdgw_send((FAR uint8_t *)cmd, (FAR uint8_t *)res, SETACFG_RESP_LEN, &resLen,
                        ALT_OSAL_TIMEO_FEVR);
  } else {
    DBGIF_LOG_ERROR("Failed to allocate command buffer.\n");
    return MISC_FAILURE;
  }

  if (0 > ret) {
    DBGIF_LOG1_ERROR("apicmdgw_send error: %ld\n", ret);
    result = MISC_FAILURE;
    goto errout_with_cmdfree;
  }

  if (SETACFG_RESP_LEN != resLen) {
    DBGIF_LOG1_ERROR("Unexpected response data length: %hu\n", resLen);
    result = MISC_FAILURE;
    goto errout_with_cmdfree;
  }

  /* Check API return code */
  result = (Misc_Error_e)res->result;

errout_with_cmdfree:
  altcom_generic_free_cmdandresbuff(cmd, res);
  return result;
}
