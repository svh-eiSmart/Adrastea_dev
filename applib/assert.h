/*  ---------------------------------------------------------------------------

    (c) copyright 2021 Altair Semiconductor, Ltd. All rights reserved.

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
#ifndef ASSERT_H
#define ASSERT_H

#include "cmsis_compiler.h"

#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
__NO_RETURN void portASSERT(const char *, int, const char *, const char *);
#define assert(x) ((x) ? (void)0 : portASSERT(__FILE__, __LINE__, NULL, #x))
#endif

#endif /* ASSERT_H */
