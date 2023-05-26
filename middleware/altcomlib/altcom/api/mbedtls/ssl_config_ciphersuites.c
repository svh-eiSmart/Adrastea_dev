/****************************************************************************
 * modules/lte/altcom/api/mbedtls/ssl_config_ciphersuites.c
 *
 *   Copyright 2018 Sony Corporation
 *   Copyright 2020, 2021 Sony Semiconductor Solutions Corporation
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

#include <string.h>
#include "dbg_if.h"
#include "altcom_errno.h"
#include "altcom_seterrno.h"
#include "apicmd_config_ciphersuites.h"
#include "apicmd_config.h"
#include "apiutil.h"
#include "mbedtls/ssl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CONFIG_CIPHERSUITES_REQ_DATALEN (sizeof(struct apicmd_config_ciphersuites_s))
#define CONFIG_CIPHERSUITES_RES_DATALEN (sizeof(struct apicmd_config_ciphersuitesres_s))
#define CONFIG_CIPHERSUITES_REQ_DATALEN_V4 (APICMD_TLS_CONFIG_CMD_DATA_SIZE + \
                                           sizeof(struct apicmd_config_ciphersuites_v4_s))
#define CONFIG_CIPHERSUITES_RES_DATALEN_V4 (APICMD_TLS_CONFIG_CMDRES_DATA_SIZE)

#define CONFIG_CIPHERSUITES_SUCCESS 0
#define CONFIG_CIPHERSUITES_FAILURE -1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct config_ciphersuites_req_s
{
  uint32_t id;
  int      *ciphersuites;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t config_ciphersuites_request(FAR struct config_ciphersuites_req_s *req)
{
  int32_t  ret;
  uint16_t reslen = 0;
  int      cnt;
  FAR void *cmd = NULL;
  FAR void *res = NULL;
  int      protocolver = 0;
  uint16_t reqbuffsize = 0;
  uint16_t resbuffsize = 0;

  /* Set parameter from protocol version */

  protocolver = APICMD_VER_V4;

  if (protocolver == APICMD_VER_V1)
    {
      reqbuffsize = CONFIG_CIPHERSUITES_REQ_DATALEN;
      resbuffsize = CONFIG_CIPHERSUITES_RES_DATALEN;
    }
  else if (protocolver == APICMD_VER_V4)
    {
      reqbuffsize = CONFIG_CIPHERSUITES_REQ_DATALEN_V4;
      resbuffsize = CONFIG_CIPHERSUITES_RES_DATALEN_V4;
    }
  else
    {
      return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

  /* Allocate send and response command buffer */

  if (!altcom_mbedtls_alloc_cmdandresbuff(
    (FAR void **)&cmd, APICMDID_TLS_CONFIG_CMD,
    reqbuffsize, (FAR void **)&res, resbuffsize))
    {
      return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

  /* Fill the data */


  if (protocolver == APICMD_VER_V1)
    {
      ((FAR struct apicmd_config_ciphersuites_s *)cmd)->conf = htonl(req->id);
      memset(((FAR struct apicmd_config_ciphersuites_s *)cmd)->ciphersuites,
        0, sizeof(int32_t)*APICMD_CONFIG_CIPHERSUITES_COUNT);

      cnt = 0;
      for (cnt = 0; cnt < APICMD_CONFIG_CIPHERSUITES_COUNT; cnt++)
        {
          if (req->ciphersuites[cnt] == 0)
            {
              break;
            }

          ((FAR struct apicmd_config_ciphersuites_s *)
            cmd)->ciphersuites[cnt] = htonl(req->ciphersuites[cnt]);
        }
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ((FAR struct apicmd_configcmd_s *)cmd)->conf = htonl(req->id);
      ((FAR struct apicmd_configcmd_s *)cmd)->subcmd_id =
        htonl(APISUBCMDID_TLS_CONFIG_CIPHERSUITES);
      memset(((FAR struct apicmd_configcmd_s *)
        cmd)->u.ciphersuites.ciphersuites, 0,
        sizeof(int32_t)*APICMD_CONFIG_CIPHERSUITES_COUNT);

      cnt = 0;
      for (cnt = 0; cnt < APICMD_CONFIG_CIPHERSUITES_COUNT; cnt++)
        {
          if (req->ciphersuites[cnt] == 0)
            {
              break;
            }

          ((FAR struct apicmd_configcmd_s *)
            cmd)->u.ciphersuites.ciphersuites[cnt] =
             htonl(req->ciphersuites[cnt]);
        }
    }

  DBGIF_LOG1_DEBUG("[config_ciphersuites]config id: %d\n", req->id);

  /* Send command and block until receive a response */

  ret = apicmdgw_send((FAR uint8_t *)cmd, (FAR uint8_t *)res,
                      resbuffsize, &reslen,
                      ALT_OSAL_TIMEO_FEVR);

  if (ret < 0)
    {
      DBGIF_LOG1_ERROR("apicmdgw_send error: %d\n", ret);
      goto errout_with_cmdfree;
    }

  if (reslen != resbuffsize)
    {
      DBGIF_LOG1_ERROR("Unexpected response data length: %d\n", reslen);
      goto errout_with_cmdfree;
    }

  if (protocolver == APICMD_VER_V1)
    {
      ret = ntohl(
        ((FAR struct apicmd_config_ciphersuitesres_s *)res)->ret_code);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ret = ntohl(((FAR struct apicmd_configcmdres_s *)res)->ret_code);
    }

  DBGIF_LOG1_DEBUG("[config_ciphersuites res]ret: %d\n", ret);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return MBEDTLS_ERR_SSL_ALLOC_FAILED;
}



/****************************************************************************
 * Public Functions
 ****************************************************************************/

void mbedtls_ssl_conf_ciphersuites(mbedtls_ssl_config *conf, const int *ciphersuites)
{
  int32_t                          result;
  struct config_ciphersuites_req_s req;

  if (!altcom_isinit())
    {
      DBGIF_LOG_ERROR("Not intialized\n");
      altcom_seterrno(ALTCOM_ENETDOWN);
      return;
    }

  req.id = conf->id;
  req.ciphersuites = (int*)ciphersuites;

  result = config_ciphersuites_request(&req);

  if (result != CONFIG_CIPHERSUITES_SUCCESS)
    {
      DBGIF_LOG1_ERROR("%s error.\n", __func__);
    }
}

