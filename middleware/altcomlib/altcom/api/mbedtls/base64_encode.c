/****************************************************************************
 * modules/lte/altcom/api/mbedtls/base64_encode.c
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
#include "apicmd_base64_encode.h"
#include "apicmd_cipher.h"
#include "apiutil.h"
#include "mbedtls/base64.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define BASE64_ENCODE_REQ_DATALEN (sizeof(struct apicmd_base64_encode_s))
#define BASE64_ENCODE_RES_DATALEN (sizeof(struct apicmd_base64_encoderes_s))
#define BASE64_ENCODE_REQ_DATALEN_V4 (APICMD_TLS_CIPHER_CMD_DATA_SIZE + \
                                   sizeof(struct apicmd_base64_encode_v4_s))
#define BASE64_ENCODE_RES_DATALEN_V4 (APICMD_TLS_CIPHER_CMDRES_DATA_SIZE + \
                                   sizeof(struct apicmd_base64_encoderes_v4_s))

#define BASE64_ENCODE_SUCCESS 0
#define BASE64_ENCODE_FAILURE -1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct base64_encode_req_s
{
  const unsigned char *src;
  size_t              slen;
  size_t              dlen;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t base64_encode_request(FAR struct base64_encode_req_s *req,
                                     unsigned char *dst, size_t *olen)
{
  int32_t  ret;
  uint16_t reslen = 0;
  uint32_t out_len = 0;
  FAR void *cmd = NULL;
  FAR void *res = NULL;
  int      protocolver = 0;
  uint16_t reqbuffsize = 0;
  uint16_t resbuffsize = 0;

  /* Set parameter from protocol version */

  protocolver = APICMD_VER_V4;

  if (protocolver == APICMD_VER_V1)
    {
      reqbuffsize = BASE64_ENCODE_REQ_DATALEN;
      resbuffsize = BASE64_ENCODE_RES_DATALEN;
    }
  else if (protocolver == APICMD_VER_V4)
    {
      reqbuffsize = BASE64_ENCODE_REQ_DATALEN_V4;
      resbuffsize = BASE64_ENCODE_RES_DATALEN_V4;
    }
  else
    {
      return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
    }

  /* Allocate send and response command buffer */

  if (!altcom_mbedtls_alloc_cmdandresbuff(
    (FAR void **)&cmd, APICMDID_TLS_CIPHER_CMD,
    reqbuffsize, (FAR void **)&res, resbuffsize))
    {
      return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
    }

  /* Fill the data */
  if (req->slen <= APICMD_BASE64_ENCODE_SRC_LEN)
    {
     if (protocolver == APICMD_VER_V1)
        {
          memcpy(((FAR struct apicmd_base64_encode_s *)cmd)->src,
            req->src, req->slen);
          ((FAR struct apicmd_base64_encode_s *)cmd)->slen = htonl(req->slen);
          ((FAR struct apicmd_base64_encode_s *)cmd)->dlen = htonl(req->dlen);
        }
      else if (protocolver == APICMD_VER_V4)
        {
          ((FAR struct apicmd_ciphercmd_s *)cmd)->subcmd_id =
            htonl(APISUBCMDID_TLS_BASE64_ENCODE);
          memcpy(((FAR struct apicmd_ciphercmd_s *)cmd)->u.base64_encode.src,
            req->src, req->slen);
          ((FAR struct apicmd_ciphercmd_s *)cmd)->u.base64_encode.slen =
            htonl(req->slen);
          ((FAR struct apicmd_ciphercmd_s *)cmd)->u.base64_encode.dlen =
            htonl(req->dlen);
        }
    }
  else
    {
      goto errout_with_cmdfree;
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
      ret = ntohl(((FAR struct apicmd_base64_encoderes_s *)res)->ret_code);

      out_len = ntohl(((FAR struct apicmd_base64_encoderes_s *)res)->olen);
      if (out_len <= req->dlen)
        {
          memcpy(dst, ((FAR struct apicmd_base64_encoderes_s *)res)->dst,
            out_len);
          *olen = out_len;
        }
      else
        {
          DBGIF_LOG1_ERROR("Unexpected output length: %d\n", out_len);
          *olen = 0;
          goto errout_with_cmdfree;
        }
    }
  else if (protocolver == APICMD_VER_V4)
    {
      ret = ntohl(((struct apicmd_ciphercmdres_s *)res)->ret_code);

      if (ntohl(((struct apicmd_ciphercmdres_s *)res)->subcmd_id) !=
        APISUBCMDID_TLS_BASE64_ENCODE)
        {
          DBGIF_LOG1_ERROR("Unexpected sub command id: %d\n",
            ntohl(((struct apicmd_ciphercmdres_s *)res)->subcmd_id));
          goto errout_with_cmdfree;
        }

      out_len = ntohl(((struct apicmd_ciphercmdres_s *)
        res)->u.base64_encoderes.olen);
      if (out_len <= req->dlen)
        {
          memcpy(dst, ((struct apicmd_ciphercmdres_s *)
            res)->u.base64_encoderes.dst, out_len);
          *olen = out_len;
        }
      else
        {
          DBGIF_LOG1_ERROR("Unexpected output length: %d\n", out_len);
          *olen = 0;
          goto errout_with_cmdfree;
        }
    }


  DBGIF_LOG1_DEBUG("[base64_encode res]ret: %d\n", ret);
  DBGIF_LOG1_DEBUG("[base64_encode res]length: %d\n", *olen);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
}



/****************************************************************************
 * Public Functions
 ****************************************************************************/


int mbedtls_base64_encode(unsigned char *dst, size_t dlen, size_t *olen,
                          const unsigned char *src, size_t slen)
{
  int32_t               result;
  size_t                out_len = 0;
  struct base64_encode_req_s req;

  if (dst == NULL || src == NULL || olen == NULL)
    {
      return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
    }

  if (!altcom_isinit())
    {
      DBGIF_LOG_ERROR("Not intialized\n");
      altcom_seterrno(ALTCOM_ENETDOWN);
      return MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
    }

  req.src = src;
  req.slen = slen;
  req.dlen = dlen;

  result = base64_encode_request(&req, dst, &out_len);
  *olen = out_len;

  return result;
}

