/****************************************************************************
 * modules/lte/altcom/api/mbedtls/ssl_verify_result.c
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
#include "apicmd_ssl_verify_result.h"
#include "apicmd_ssl.h"
#include "apiutil.h"
#include "mbedtls/ssl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SSL_VERIFY_RESULT_REQ_DATALEN (sizeof(struct apicmd_ssl_verify_result_s))
#define SSL_VERIFY_RESULT_RES_DATALEN (sizeof(struct apicmd_ssl_verify_resultres_s))
#define SSL_VERIFY_RESULT_REQ_DATALEN_V4 (APICMD_TLS_SSL_CMD_DATA_SIZE)
#define SSL_VERIFY_RESULT_RES_DATALEN_V4 (APICMD_TLS_SSL_CMDRES_DATA_SIZE)

#define SSL_VERIFY_RESULT_FAILURE 0xFFFFFFFF

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct ssl_verify_result_req_s
{
  uint32_t id;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t ssl_verify_result_request(FAR struct ssl_verify_result_req_s *req)
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
      reqbuffsize = SSL_VERIFY_RESULT_REQ_DATALEN;
      resbuffsize = SSL_VERIFY_RESULT_RES_DATALEN;
    }
  else if (protocolver == APICMD_VER_V4)
    {
      reqbuffsize = SSL_VERIFY_RESULT_REQ_DATALEN_V4;
      resbuffsize = SSL_VERIFY_RESULT_RES_DATALEN_V4;
    }
  else
    {
      return SSL_VERIFY_RESULT_FAILURE;
    }

  /* Allocate send and response command buffer */

  if (!altcom_mbedtls_alloc_cmdandresbuff(
    (FAR void **)&cmd, APICMDID_TLS_SSL_CMD,
    reqbuffsize, (FAR void **)&res, resbuffsize))
    {
      return SSL_VERIFY_RESULT_FAILURE;
    }

  /* Fill the data */

  if (protocolver == APICMD_VER_V1)
    {
      ((FAR struct apicmd_ssl_verify_result_s *)cmd)->ssl = htonl(req->id);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ((FAR struct apicmd_sslcmd_s *)cmd)->ssl = htonl(req->id);
      ((FAR struct apicmd_sslcmd_s *)cmd)->subcmd_id =
        htonl(APISUBCMDID_TLS_SSL_VERIFY_RESULT);
    }

  DBGIF_LOG1_DEBUG("[ssl_verify_result]ctx id: %d\n", req->id);

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
        ((FAR struct apicmd_ssl_verify_resultres_s *)res)->ret_code);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      if (ntohl(((FAR struct apicmd_sslcmdres_s *)res)->subcmd_id) !=
        APISUBCMDID_TLS_SSL_VERIFY_RESULT)
        {
          DBGIF_LOG1_ERROR("Unexpected sub command id: %d\n",
            ntohl(((FAR struct apicmd_sslcmdres_s *)res)->subcmd_id));
          goto errout_with_cmdfree;
        }

      ret = ntohl(((FAR struct apicmd_sslcmdres_s *)res)->ret_code);
    }

  DBGIF_LOG1_DEBUG("[ssl_verify_result res]ret: %d\n", ret);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return SSL_VERIFY_RESULT_FAILURE;
}



/****************************************************************************
 * Public Functions
 ****************************************************************************/

uint32_t mbedtls_ssl_get_verify_result( const mbedtls_ssl_context *ssl )
{
  int32_t                        result;
  struct ssl_verify_result_req_s req;

  if (!altcom_isinit())
    {
      DBGIF_LOG_ERROR("Not intialized\n");
      altcom_seterrno(ALTCOM_ENETDOWN);
      return SSL_VERIFY_RESULT_FAILURE;
    }

  req.id = ssl->id;

  result = ssl_verify_result_request(&req);

  return result;
}

