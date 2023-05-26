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
#include <stdint.h>
#include <errno.h>
#include <ctype.h>

#include "lte/lte_api.h"
#include "apiutil.h"
#include "apicmd_fwupgradecmd.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define FW_UPGRADE_CMD_DATA_LEN (sizeof(struct apicmd_cmddat_fwupgradecmd_s))
#define FW_UPGRADE_CMD_RESP_LEN (sizeof(struct apicmd_cmddat_fwupgradecmdres_s))

/****************************************************************************
 * Public Functions
 ****************************************************************************/
/**
 * @brief invoke a FW upgrade command
 *
 * @param [in] fw_upgrade_cmd: FW upgrade command
 * @return  On success, LTE_RESULT_OK is returned. On failure,
 * LTE_RESULT_ERR is returned.
 */
lteresult_e lte_fw_upgrade_cmd(lte_fw_upgrade_cmd_t *fw_upgrade_cmd) {
  int32_t ret;
  uint16_t resLen = 0;
  FAR struct apicmd_cmddat_fwupgradecmd_s *cmd;
  FAR struct apicmd_cmddat_fwupgradecmdres_s *res;

  /* Return error if parameter is invalid */
  if (!fw_upgrade_cmd) {
    DBGIF_LOG_ERROR("Input argument is NULL.\n");
    return LTE_RESULT_ERROR;
  }
  if (fw_upgrade_cmd->cmd > LTE_FW_UPGRADE_CMD_CANCELFOTA) {
    DBGIF_LOG_ERROR("Input argument is invalid.\n");
    return LTE_RESULT_ERROR;
  }
  if (fw_upgrade_cmd->confirmation > LTE_FW_UPGRADE_CMD_CONFIRM_CANCEL)
    fw_upgrade_cmd->confirmation = LTE_FW_UPGRADE_CMD_CONFIRM_ACCEPT;  // default value is accept
  if (fw_upgrade_cmd->result > LTE_FW_UPGRADE_RES_UNSUPPORTED_PROTOCOL) {
    fw_upgrade_cmd->result = LTE_FW_UPGRADE_RES_INITIAL_VALUE;  // default value is idle
  }

  /* Check if the library is initialized */
  if (!altcom_isinit()) {
    DBGIF_LOG_ERROR("Not initialized\n");
    return LTE_RESULT_ERROR;
  }

  /* Allocate API command buffer to send */
  if (altcom_generic_alloc_cmdandresbuff((FAR void **)&cmd, APICMDID_FW_UPGRADE_CMD,
                                         FW_UPGRADE_CMD_DATA_LEN, (FAR void **)&res,
                                         FW_UPGRADE_CMD_RESP_LEN)) {
    cmd->cmd = (uint8_t)fw_upgrade_cmd->cmd;
    cmd->confirmation = (uint8_t)fw_upgrade_cmd->confirmation;
    cmd->result = (uint8_t)fw_upgrade_cmd->result;

    /* Send API command to modem */
    ret = apicmdgw_send((FAR uint8_t *)cmd, (FAR uint8_t *)res, FW_UPGRADE_CMD_RESP_LEN, &resLen,
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

  if (FW_UPGRADE_CMD_RESP_LEN != resLen) {
    DBGIF_LOG1_ERROR("Unexpected response data length: %hu\n", resLen);
    ret = LTE_RESULT_ERROR;
    goto errout_with_cmdfree;
  }

  /* Check API return code */
  ret = (int32_t)res->result;

  if (APICMD_FW_UPGRADE_CMD_RES_OK != ret && APICMD_FW_UPGRADE_CMD_RES_ERR != ret) {
    DBGIF_LOG1_ERROR("Unexpected API result: %ld\n", ret);
    ret = LTE_RESULT_ERROR;
    goto errout_with_cmdfree;
  }

errout_with_cmdfree:
  altcom_generic_free_cmdandresbuff(cmd, res);
  return (lteresult_e)ret;
}
