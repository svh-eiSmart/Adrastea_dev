/****************************************************************************
 *
 *  (c) copyright 2022 Altair Semiconductor, Ltd. All rights reserved.
 *
 *  This software, in source or object form (the "Software"), is the
 *  property of Altair Semiconductor Ltd. (the "Company") and/or its
 *  licensors, which have all right, title and interest therein, You
 *  may use the Software only in  accordance with the terms of written
 *  license agreement between you and the Company (the "License").
 *  Except as expressly stated in the License, the Company grants no
 *  licenses by implication, estoppel, or otherwise. If you are not
 *  aware of or do not agree to the License terms, you may not use,
 *  copy or modify the Software. You may use the source code of the
 *  Software only for your internal purposes and may not distribute the
 *  source code of the Software, any part thereof, or any derivative work
 *  thereof, to any third party, except pursuant to the Company's prior
 *  written consent.
 *  The Software is the confidential information of the Company.
 *
 ****************************************************************************/

/* Declare extra CLI commands in this file */

#undef MCU_PROJECT_NAME
#define MCU_PROJECT_NAME "MbedtlsConnector"

DECLARE_COMMAND("install-certificates", install_certificates, "install-certificates - setup certificates for this demo")
DECLARE_COMMAND("uninstall-certificates", uninstall_certificates, "uninstall-certificates - delete certificates for this demo")
DECLARE_COMMAND("whereami", do_whereami, "whereami - https://ifconfig.co/{city,ip}")
DECLARE_COMMAND("californium", do_californium,
                "californium - coaps://californium.eclipseprojects.io")
