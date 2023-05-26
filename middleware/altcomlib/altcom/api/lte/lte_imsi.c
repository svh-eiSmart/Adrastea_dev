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

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "lte/lte_api.h"
#include "apiutil.h"
#include "apicmd_imsi.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GETIMSI_DATA_LEN (0)
#define GETIMSI_RESP_LEN (sizeof(struct apicmd_cmddat_getimsires_s))

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief lte_get_imsi() get International Mobile Subscriber Identity(IMSI).
 *
 * @param[inout] errcause: Error cause.
 * It is set only if the result is not successful.
 * See @ref lteerrcause
 * @param[inout] imsi: A character string indicating IMSI.
 * It is terminated with '\0'; Note that is buffer should have sufficient length not less than @ref
 * LTE_MAX_IMSI_LEN.
 *
 * @return On success, LTE_RESULT_OK is returned. On failure,
 * LTE_RESULT_ERR is returned.
 */

lteresult_e lte_get_imsi(lteerrcause_e *errcause, int8_t *imsi) {
  int32_t ret;
  uint16_t resLen = 0;
  FAR void *cmd;
  FAR struct apicmd_cmddat_getimsires_s *res;

  /* Return error if parameter is invalid */
  if (!errcause || !imsi) {
    DBGIF_LOG_ERROR("Input argument is NULL.\n");
    return LTE_RESULT_ERROR;
  }

  /* Check if the library is initialized */
  if (!altcom_isinit()) {
    DBGIF_LOG_ERROR("Not intialized\n");
    return LTE_RESULT_ERROR;
  }

  /* Allocate API command buffer to send */
  if (altcom_generic_alloc_cmdandresbuff((FAR void **)&cmd, APICMDID_GET_IMSI, GETIMSI_DATA_LEN,
                                         (FAR void **)&res, GETIMSI_RESP_LEN)) {
    /* Send API command to modem */
    ret = apicmdgw_send((FAR uint8_t *)cmd, (FAR uint8_t *)res, GETIMSI_RESP_LEN, &resLen,
                        ALT_OSAL_TIMEO_FEVR);
  } else {
    DBGIF_LOG_ERROR("Failed to allocate command buffer.\n");
    return LTE_RESULT_ERROR;
  }

  if (0 > ret) {
    DBGIF_LOG1_ERROR("apicmdgw_send error: %ld\n", ret);
    ret = LTE_RESULT_ERROR;
    goto errout_with_cmdfree;
  }

  if (GETIMSI_RESP_LEN != resLen) {
    DBGIF_LOG1_ERROR("Unexpected response data length: %hu\n", resLen);
    ret = LTE_RESULT_ERROR;
    goto errout_with_cmdfree;
  }

  /* Check API return code */
  ret = (int32_t)res->result;
  if (APICMD_IMSI_RES_OK == ret) {
    strncpy((char *)imsi, (char *)res->imsi, LTE_MAX_IMSI_LEN);
  } else {
    DBGIF_LOG1_ERROR("Unexpected API result: %ld\n", ret);
    *errcause = (lteerrcause_e)res->errcause;
    ret = LTE_RESULT_ERROR;
    goto errout_with_cmdfree;
  }

errout_with_cmdfree:
  altcom_generic_free_cmdandresbuff(cmd, res);
  return (lteresult_e)ret;
}
