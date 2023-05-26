/****************************************************************************
 * modules/lte/altcom/api/mbedtls/ssl_read.c
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
#include "apicmd_ssl_read.h"
#include "apicmd_ssl.h"
#include "apiutil.h"
#include "mbedtls/ssl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SSL_READ_REQ_DATALEN (sizeof(struct apicmd_ssl_read_s))
#define SSL_READ_RES_DATALEN (sizeof(struct apicmd_ssl_readres_s))
#define SSL_READ_REQ_DATALEN_V4 (APICMD_TLS_SSL_CMD_DATA_SIZE + \
                                sizeof(struct apicmd_ssl_read_v4_s))
#define SSL_READ_RES_DATALEN_V4 (APICMD_TLS_SSL_CMDRES_DATA_SIZE + \
                                sizeof(struct apicmd_ssl_readres_v4_s))

#define SSL_READ_SUCCESS 0
#define SSL_READ_FAILURE -1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct ssl_read_req_s
{
  uint32_t      id;
  unsigned char *buf;
  size_t        len;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t ssl_read_request(FAR struct ssl_read_req_s *req,
                                unsigned char *buf)
{
  int32_t  ret;
  size_t   req_buf_len = 0;
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
      reqbuffsize = SSL_READ_REQ_DATALEN;
      resbuffsize = SSL_READ_RES_DATALEN;
    }
  else if (protocolver == APICMD_VER_V4)
    {
      reqbuffsize = SSL_READ_REQ_DATALEN_V4;
      resbuffsize = SSL_READ_RES_DATALEN_V4;
    }
  else
    {
      return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

  /* Allocate send and response command buffer */

  if (!altcom_mbedtls_alloc_cmdandresbuff(
    (FAR void **)&cmd, APICMDID_TLS_SSL_CMD, reqbuffsize,
    (FAR void **)&res, resbuffsize))
    {
      return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

  /* Fill the data */

  if (protocolver == APICMD_VER_V1)
    {
      ((FAR struct apicmd_ssl_read_s *)cmd)->ssl = htonl(req->id);

      req_buf_len = (req->len <= APICMD_SSL_READ_BUF_LEN)
        ? req->len : APICMD_SSL_READ_BUF_LEN;
      ((FAR struct apicmd_ssl_read_s *)cmd)->len = htonl(req_buf_len);
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ((FAR struct apicmd_sslcmd_s *)cmd)->ssl = htonl(req->id);
      ((FAR struct apicmd_sslcmd_s *)cmd)->subcmd_id =
        htonl(APISUBCMDID_TLS_SSL_READ);

      req_buf_len = (req->len <= APICMD_SSL_READ_BUF_LEN)
        ? req->len : APICMD_SSL_READ_BUF_LEN;
      ((FAR struct apicmd_sslcmd_s *)cmd)->u.read.len = htonl(req_buf_len);
    }

  DBGIF_LOG1_DEBUG("[ssl_read]ctx id: %d\n", req->id);
  DBGIF_LOG1_DEBUG("[ssl_read]read len: %d\n", req_buf_len);

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
      ret = ntohl(((FAR struct apicmd_ssl_readres_s *)res)->ret_code);
      if (ret <= 0)
        {
          /* Nothing to do */
        }
      else if ((0 < ret) && (ret <= req_buf_len))
        {
          memcpy(buf, ((FAR struct apicmd_ssl_readres_s *)res)->buf, ret);
        }
      else
        {
          DBGIF_LOG1_ERROR("Unexpected buffer length: %d\n", ret);
          goto errout_with_cmdfree;
        }
    }
  else if (protocolver == APICMD_VER_V4)
    {
      if (ntohl(((FAR struct apicmd_sslcmdres_s *)res)->subcmd_id) !=
        APISUBCMDID_TLS_SSL_READ)
        {
          DBGIF_LOG1_ERROR("Unexpected sub command id: %d\n",
            ntohl(((FAR struct apicmd_sslcmdres_s *)res)->subcmd_id));
          goto errout_with_cmdfree;
        }

      ret = ntohl(((FAR struct apicmd_sslcmdres_s *)res)->ret_code);
      if (ret <= 0)
        {
          /* Nothing to do */
        }
      else if ((0 < ret) && (ret <= req_buf_len))
        {
          memcpy(buf, ((FAR struct apicmd_sslcmdres_s *)res)->u.readres.buf,
            ret);
        }
      else
        {
          DBGIF_LOG1_ERROR("Unexpected buffer length: %d\n", ret);
          goto errout_with_cmdfree;
        }
    }

  DBGIF_LOG1_DEBUG("[ssl_read res]ret: %d\n", ret);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
}



/****************************************************************************
 * Public Functions
 ****************************************************************************/

int mbedtls_ssl_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len)
{
  int32_t               result;
  struct ssl_read_req_s req;

  if (!altcom_isinit())
    {
      DBGIF_LOG_ERROR("Not intialized\n");
      altcom_seterrno(ALTCOM_ENETDOWN);
      return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

  req.id = ssl->id;
  req.len = len;

  result = ssl_read_request(&req, buf);

  return result;
}

