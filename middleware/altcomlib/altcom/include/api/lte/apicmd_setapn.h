/****************************************************************************
 * modules/lte/altcom/include/api/lte/apicmd_setapn.h
 *
 *   Copyright 2018 Sony Semiconductor Solutions Corporation
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

#ifndef __MODULES_LTE_ALTCOM_INCLUDE_API_LTE_APICMD_SETAPN_H
#define __MODULES_LTE_ALTCOM_INCLUDE_API_LTE_APICMD_SETAPN_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "apicmd.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define APICMD_SETAPN_SESSIONID_MIN (1)
#define APICMD_SETAPN_SESSIONID_MAX (5)

#define APICMD_SETAPN_IPTYPE_IP (0)
#define APICMD_SETAPN_IPTYPE_IPV6 (1)
#define APICMD_SETAPN_IPTYPE_IPV4V6 (2)

#define APICMD_SETAPN_AUTHTYPE_NONE (0)
#define APICMD_SETAPN_AUTHTYPE_PAP (1)
#define APICMD_SETAPN_AUTHTYPE_CHAP (2)

#define APICMD_SETAPN_APN_LEN (101)
#define APICMD_SETAPN_USERNAME_LEN (64)
#define APICMD_SETAPN_PASS_LEN (32)

#define APICMD_SETAPN_RES_OK (0)
#define APICMD_SETAPN_RES_ERR (1)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* This structure discribes the data structure of the API command */

begin_packed_struct struct apicmd_cmddat_setapn_s {
  uint8_t session_id;
  uint8_t apn[APICMD_SETAPN_APN_LEN];
  uint8_t ip_type;
  uint8_t auth_type;
  uint8_t user_name[APICMD_SETAPN_USERNAME_LEN];
  uint8_t password[APICMD_SETAPN_PASS_LEN];
} end_packed_struct;

begin_packed_struct struct apicmd_cmddat_setapnv2_s {
  uint8_t session_id;
  uint8_t apn[APICMD_SETAPN_APN_LEN];
  uint8_t ip_type;
  uint8_t auth_type;
  uint8_t user_name[APICMD_SETAPN_USERNAME_LEN];
  uint8_t password[APICMD_SETAPN_PASS_LEN];
  uint8_t hostname[APICMD_SETAPN_APN_LEN];
  uint8_t ipv4addralloc;
  uint8_t pcscf_discovery;
  uint8_t nslpi;
} end_packed_struct;

begin_packed_struct struct apicmd_cmddat_setapnres_s { uint8_t result; } end_packed_struct;

#endif /* __MODULES_LTE_ALTCOM_INCLUDE_API_LTE_APICMD_SETAPN_H */
