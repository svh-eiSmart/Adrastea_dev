/****************************************************************************
 * modules/lte/altcom/api/mbedtls/ssl_config_defaults.c
 *
 *   Copyright 2018 Sony Corporation
 *   Copyright 2020 Sony Semiconductor Solutions Corporation
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
#include "apicmd_config_defaults.h"
#include "apicmd_config.h"
#include "apiutil.h"
#include "mbedtls/ssl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CONFIG_DEFAULTS_REQ_DATALEN (sizeof(struct apicmd_config_defaults_s))
#define CONFIG_DEFAULTS_RES_DATALEN (sizeof(struct apicmd_config_defaultsres_s))
#define CONFIG_DEFAULTS_REQ_DATALEN_V4 (APICMD_TLS_CONFIG_CMD_DATA_SIZE + \
                                       sizeof(struct apicmd_config_defaults_v4_s))
#define CONFIG_DEFAULTS_RES_DATALEN_V4 (APICMD_TLS_CONFIG_CMDRES_DATA_SIZE)

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct config_defaults_req_s
{
  uint32_t id;
  int32_t  endpoint;
  int32_t  transport;
  int32_t  preset;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t config_defaults_request(FAR struct config_defaults_req_s *req)
{
  int32_t  ret;
  uint16_t reslen = 0;
  FAR void *cmd = NULL;
  FAR void *res = NULL;
  int      protocolver = 0;
  uint16_t reqbuffsize = 0;
  uint16_t resbuffsize = 0;

  /* Set parameter from protocol version */

  protocolver = APICMD_VER_V4;

  if (protocolver == APICMD_VER_V1)
    {
      reqbuffsize = CONFIG_DEFAULTS_REQ_DATALEN;
      resbuffsize = CONFIG_DEFAULTS_RES_DATALEN;
    }
  else if (protocolver == APICMD_VER_V4)
    {
      reqbuffsize = CONFIG_DEFAULTS_REQ_DATALEN_V4;
      resbuffsize = CONFIG_DEFAULTS_RES_DATALEN_V4;
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
      ((FAR struct apicmd_config_defaults_s *)cmd)->conf = htonl(req->id);
      ((FAR struct apicmd_config_defaults_s *)cmd)->endpoint =
        (int32_t) htonl(req->endpoint);
      ((FAR struct apicmd_config_defaults_s *)cmd)->transport =
        (int32_t) htonl(req->transport);
      ((FAR struct apicmd_config_defaults_s *)cmd)->preset =
        (int32_t) htonl(req->preset);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ((FAR struct apicmd_configcmd_s *)cmd)->conf = htonl(req->id);
      ((FAR struct apicmd_configcmd_s *)cmd)->subcmd_id =
        htonl(APISUBCMDID_TLS_CONFIG_DEFAULTS);
      ((FAR struct apicmd_configcmd_s *)cmd)->u.defaults.endpoint =
        (int32_t) htonl(req->endpoint);
      ((FAR struct apicmd_configcmd_s *)cmd)->u.defaults.transport =
        (int32_t) htonl(req->transport);
      ((FAR struct apicmd_configcmd_s *)cmd)->u.defaults.preset =
        (int32_t) htonl(req->preset);
    }

  DBGIF_LOG1_DEBUG("[config_defaults]config id: %d\n", req->id);
  DBGIF_LOG1_DEBUG("[config_defaults]endpoint: %d\n", req->endpoint);
  DBGIF_LOG1_DEBUG("[config_defaults]transport: %d\n", req->transport);
  DBGIF_LOG1_DEBUG("[config_defaults]preset: %d\n", req->preset);

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
      ret = ntohl(((FAR struct apicmd_config_defaultsres_s *)res)->ret_code);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ret = ntohl(((FAR struct apicmd_configcmdres_s *)res)->ret_code);
    }

  DBGIF_LOG1_DEBUG("[config_defaults res]ret: %d\n", ret);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return MBEDTLS_ERR_SSL_ALLOC_FAILED;
}



/****************************************************************************
 * Public Functions
 ****************************************************************************/

int mbedtls_ssl_config_defaults(mbedtls_ssl_config *conf,
                                int endpoint, int transport, int preset)
{
  int32_t                      result;
  struct config_defaults_req_s req;

  if (!altcom_isinit())
    {
      DBGIF_LOG_ERROR("Not intialized\n");
      altcom_seterrno(ALTCOM_ENETDOWN);
      return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

  req.id = conf->id;
  req.endpoint = endpoint;
  req.transport = transport;
  req.preset = preset;

  result = config_defaults_request(&req);

  return result;
}

