/****************************************************************************
 * modules/lte/altcom/api/mbedtls/x509_crt_info.c
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
#include "apicmd_x509_crt_info.h"
#include "apiutil.h"
#include "mbedtls/x509_crt.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define X509_CRT_INFO_REQ_DATALEN (sizeof(struct apicmd_x509_crt_info_s))
#define X509_CRT_INFO_RES_DATALEN (sizeof(struct apicmd_x509_crt_infores_s))

#define X509_CRT_INFO_FAILURE -1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct x509_crt_info_req_s
{
  uint32_t   id;
  size_t     size;
  const char *prefix;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int32_t x509_crt_info_request(FAR struct x509_crt_info_req_s *req,
                                     char *buf)
{
  int32_t                              ret;
  uint16_t                             reslen = 0;
  uint32_t                             buflen = 0;
  FAR struct apicmd_x509_crt_info_s    *cmd = NULL;
  FAR struct apicmd_x509_crt_infores_s *res = NULL;
  uint16_t                             cmdid = 0;

  if (buf == NULL)
    {
      return X509_CRT_INFO_FAILURE;
    }

  cmdid = APICMDID_TLS_X509_CRT_INFO;

  /* Allocate send and response command buffer */

  if (!altcom_mbedtls_alloc_cmdandresbuff(
    (FAR void **)&cmd, cmdid,
    X509_CRT_INFO_REQ_DATALEN,
    (FAR void **)&res, X509_CRT_INFO_RES_DATALEN))
    {
      return X509_CRT_INFO_FAILURE;
    }

  /* Fill the data */

  cmd->crt = htonl(req->id);
  buflen = (req->size <= APICMD_X509_CRT_INFO_RET_BUF_LEN)
    ? req->size : APICMD_X509_CRT_INFO_RET_BUF_LEN;
  cmd->size = htonl(buflen);
  memset(cmd->prefix, '\0', APICMD_X509_CRT_INFO_PREFIX_LEN);
  if (req->prefix != NULL)
    {
      strncpy((char*) cmd->prefix, req->prefix, APICMD_X509_CRT_INFO_PREFIX_LEN-1);
    }

  DBGIF_LOG1_DEBUG("[x509_crt_info]ctx id: %d\n", req->id);
  DBGIF_LOG1_DEBUG("[x509_crt_info]size: %d\n", req->size);
  DBGIF_LOG1_DEBUG("[x509_crt_info]prefix: %s\n", cmd->prefix);

  /* Send command and block until receive a response */

  ret = apicmdgw_send((FAR uint8_t *)cmd, (FAR uint8_t *)res,
                      X509_CRT_INFO_RES_DATALEN, &reslen,
                      ALT_OSAL_TIMEO_FEVR);

  if (ret < 0)
    {
      DBGIF_LOG1_ERROR("apicmdgw_send error: %d\n", ret);
      goto errout_with_cmdfree;
    }

  if (reslen != X509_CRT_INFO_RES_DATALEN)
    {
      DBGIF_LOG1_ERROR("Unexpected response data length: %d\n", reslen);
      goto errout_with_cmdfree;
    }

  ret = ntohl(res->ret_code);
  ret = (ret <= buflen) ? ret : buflen;
  memcpy(buf, res->buf, ret);

  DBGIF_LOG1_DEBUG("[x509_crt_info res]ret: %d\n", ret);

  altcom_mbedtls_free_cmdandresbuff(cmd, res);

  return ret;

errout_with_cmdfree:
  altcom_mbedtls_free_cmdandresbuff(cmd, res);
  return X509_CRT_INFO_FAILURE;
}



/****************************************************************************
 * Public Functions
 ****************************************************************************/

int mbedtls_x509_crt_info(char *buf, size_t size, const char *prefix,
                          const mbedtls_x509_crt *crt)
{
  int32_t                         result;
  struct x509_crt_info_req_s req;

  if (!altcom_isinit())
    {
      DBGIF_LOG_ERROR("Not intialized\n");
      altcom_seterrno(ALTCOM_ENETDOWN);
      return X509_CRT_INFO_FAILURE;
    }

  req.id = crt->id;
  req.size = size;
  req.prefix = prefix;

  result = x509_crt_info_request(&req, buf);

  return result;
}

