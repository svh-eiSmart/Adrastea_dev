/*  ---------------------------------------------------------------------------

    (c) copyright 2022 Altair Semiconductor, Ltd. All rights reserved.

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

#ifndef _CERTIFICATES_H_
#define _CERTIFICATES_H_

#include <stdint.h>

extern char *GlobalSign_Root_CA_name;
extern char *GlobalSign_Root_CA;

extern char *californium_cert_name;
extern char *californium_cert;
extern char *californium_key_name;
extern char *californium_key;
extern char *californium_root_name;
extern char *californium_root;

int setup_cafile(const char *credName, uint8_t *credData, uint16_t credLen);

#endif /* _CERTIFICATES_H_ */
