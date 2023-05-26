/****************************************************************************
 * modules/lte/altcom/api/mbedtls/ssl_conf_max_frag_len.c
 *
 *   Copyright 2018 Sony Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Sony Semiconductor Solutions Corporation nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "dbg_if.h"
#include "altcom_errno.h"
#include "altcom_seterrno.h"
#include "apicmd_config_max_frag_len.h"
#include "apicmd_config.h"
#include "apiutil.h"
#include "mbedtls/ssl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CONFIG_MAX_FRAG_LEN_REQ_DATALEN (APICMD_TLS_CONFIG_CMD_DATA_SIZE + sizeof(struct apicmd_config_max_frag_len_s))
#define CONFIG_MAX_FRAG_LEN_RES_DATALEN (APICMD_TLS_CONFIG_CMDRES_DATA_SIZE)

#define CONFIG_MAX_FRAG_LEN_SUCCESS 0
#define CONFIG_MAX_FRAG_LEN_FAILURE -1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct config_max_frag_len_req_s {
  uint32_t id;
  uint8_t mfl_code;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t config_max_frag_len_request(FAR struct config_max_frag_len_req_s *req) {
  int32_t ret;
  uint16_t reslen = 0;
  FAR struct apicmd_config_max_frag_len_s *cmd = NULL;
  FAR struct apicmd_config_max_frag_lenres_s *res = NULL;

  /* Allocate send and response command buffer */

  if (!altcom_mbedtls_alloc_cmdandresbuff((FAR void **)&cmd, APICMDID_TLS_CONFIG_CMD,
                                          CONFIG_MAX_FRAG_LEN_REQ_DATALEN, (FAR void **)&res,
                                          CONFIG_MAX_FRAG_LEN_RES_DATALEN)) {
    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
  }

  /* Fill the data */

  ((FAR struct apicmd_configcmd_s *)cmd)->conf = htonl(req->id);
  ((FAR struct apicmd_configcmd_s *)cmd)->subcmd_id =
    htonl(APISUBCMDID_TLS_CONFIG_MAX_FRAG_LEN);
  ((FAR struct apicmd_configcmd_s *)cmd)->u.max_frag_len.mfl_code =
    (int32_t) htonl(req->mfl_code);

  DBGIF_LOG1_DEBUG("[config_max_frag_len]config id: %d\n", req->id);
  DBGIF_LOG1_DEBUG("[config_max_frag_len]mfl_code: %d\n", req->mfl_code);

  /* Send command and block until receive a response */

  ret = apicmdgw_send((FAR uint8_t *)cmd, (FAR uint8_t *)res, CONFIG_MAX_FRAG_LEN_RES_DATALEN, &reslen,
                      ALT_OSAL_TIMEO_FEVR);

  if (ret < 0) {
    DBGIF_LOG1_ERROR("apicmdgw_send error: %d\n", ret);
    goto errout_with_cmdfree;
  }

  if (reslen != CONFIG_MAX_FRAG_LEN_RES_DATALEN) {
    DBGIF_LOG1_ERROR("Unexpected response data length: %d\n", reslen);
    goto errout_with_cmdfree;
  }

  ret = ntohl(((FAR struct apicmd_configcmdres_s *)res)->ret_code);

  DBGIF_LOG1_DEBUG("[config_max_frag_len res]ret: %d\n", ret);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return CONFIG_MAX_FRAG_LEN_FAILURE;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int mbedtls_ssl_conf_max_frag_len( mbedtls_ssl_config *conf, unsigned char mfl_code ) {
  int32_t result;
  struct config_max_frag_len_req_s req;

  if (!altcom_isinit()) {
    DBGIF_LOG_ERROR("Not intialized\n");
    altcom_seterrno(ALTCOM_ENETDOWN);
    return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
  }

  req.id = conf->id;
  req.mfl_code = mfl_code;

  result = config_max_frag_len_request(&req);

  if (result != CONFIG_MAX_FRAG_LEN_SUCCESS) {
    DBGIF_LOG_ERROR("%s error.\n");
  }

  return result;
}

