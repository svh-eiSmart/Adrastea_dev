/****************************************************************************
 * modules/lte/altcom/api/mbedtls/ssl_hostname.c
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

#include <string.h>
#include "dbg_if.h"
#include "altcom_errno.h"
#include "altcom_seterrno.h"
#include "apicmd_ssl_hostname.h"
#include "apicmd_ssl.h"
#include "apiutil.h"
#include "mbedtls/ssl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SSL_HOSTNAME_REQ_DATALEN (sizeof(struct apicmd_ssl_hostname_s))
#define SSL_HOSTNAME_RES_DATALEN (sizeof(struct apicmd_ssl_hostnameres_s))
#define SSL_HOSTNAME_REQ_DATALEN_V4 (APICMD_TLS_SSL_CMD_DATA_SIZE + \
                                    sizeof(struct apicmd_ssl_hostname_v4_s))
#define SSL_HOSTNAME_RES_DATALEN_V4 (APICMD_TLS_SSL_CMDRES_DATA_SIZE)

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct ssl_hostname_req_s
{
  uint32_t   id;
  const char *hostname;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t ssl_hostname_request(FAR struct ssl_hostname_req_s *req)
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
      reqbuffsize = SSL_HOSTNAME_REQ_DATALEN;
      resbuffsize = SSL_HOSTNAME_RES_DATALEN;
    }
  else if (protocolver == APICMD_VER_V4)
    {
      reqbuffsize = SSL_HOSTNAME_REQ_DATALEN_V4;
      resbuffsize = SSL_HOSTNAME_RES_DATALEN_V4;
    }
  else
    {
      return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

  /* Allocate send and response command buffer */

  if (!altcom_mbedtls_alloc_cmdandresbuff(
    (FAR void **)&cmd, APICMDID_TLS_SSL_CMD,
    reqbuffsize, (FAR void **)&res, resbuffsize))
    {
      return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

  /* Fill the data */

  if (protocolver == APICMD_VER_V1)
    {
      ((FAR struct apicmd_ssl_hostname_s *)cmd)->ssl = htonl(req->id);
      memset(((FAR struct apicmd_ssl_hostname_s *)cmd)->hostname, '\0',
        APICMD_SSL_HOSTNAME_LEN);
      if (req->hostname != NULL)
        {
          size_t hostname_len = strnlen(req->hostname,
            APICMD_SSL_HOSTNAME_LEN-1);
          strncpy((char*)((FAR struct apicmd_ssl_hostname_s *)cmd)->hostname,
            req->hostname, hostname_len);
        }

      DBGIF_LOG1_DEBUG("[ssl_hostname]id: %d\n", req->id);
      DBGIF_LOG1_DEBUG("[ssl_hostname]hostname: %s\n",
        ((FAR struct apicmd_ssl_hostname_s *)cmd)->hostname);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ((FAR struct apicmd_sslcmd_s *)cmd)->ssl = htonl(req->id);
      ((FAR struct apicmd_sslcmd_s *)cmd)->subcmd_id =
      htonl(APISUBCMDID_TLS_SSL_HOSTNAME);
      memset(((FAR struct apicmd_sslcmd_s *)cmd)->u.hostname.hostname, '\0',
        APICMD_SSL_HOSTNAME_LEN);
      if (req->hostname != NULL)
        {
          size_t hostname_len = strnlen(req->hostname,
            APICMD_SSL_HOSTNAME_LEN-1);
          strncpy((char*)((FAR struct apicmd_sslcmd_s *)
            cmd)->u.hostname.hostname, req->hostname, hostname_len);
        }

      DBGIF_LOG1_DEBUG("[ssl_hostname]id: %d\n", req->id);
      DBGIF_LOG1_DEBUG("[ssl_hostname]hostname: %s\n",
        ((FAR struct apicmd_sslcmd_s *)cmd)->u.hostname.hostname);
    }


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
      ret = ntohl(((FAR struct apicmd_ssl_hostnameres_s *)res)->ret_code);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      if (ntohl(((FAR struct apicmd_sslcmdres_s *)res)->subcmd_id) !=
          APISUBCMDID_TLS_SSL_HOSTNAME)
        {
          DBGIF_LOG1_ERROR("Unexpected sub command id: %d\n",
            ntohl(((FAR struct apicmd_sslcmdres_s *)res)->subcmd_id));
          goto errout_with_cmdfree;
        }
      ret = ntohl(((FAR struct apicmd_sslcmdres_s *)res)->ret_code);
    }

  DBGIF_LOG1_DEBUG("[ssl_hostname res]ret: %d\n", ret);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
}



/****************************************************************************
 * Public Functions
 ****************************************************************************/

int mbedtls_ssl_set_hostname(mbedtls_ssl_context *ssl, const char *hostname)
{
  int32_t                   result;
  struct ssl_hostname_req_s req;

  if (!altcom_isinit())
    {
      DBGIF_LOG_ERROR("Not intialized\n");
      altcom_seterrno(ALTCOM_ENETDOWN);
      return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

  req.id = ssl->id;
  req.hostname = hostname;

  result = ssl_hostname_request(&req);

  return result;
}

