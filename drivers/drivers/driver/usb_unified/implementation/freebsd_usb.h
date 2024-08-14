/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Usb Head File
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef _FREEBSD_USB_H
#define	_FREEBSD_USB_H

#include "core/usb_endian.h"
#include "core/usb_freebsd_loader.h"
#include "core/usb.h"
#include "core/linux_usb.h"
#include "core/usbdi.h"
#include "core/usbdi_util.h"
#include "core/usb_core.h"
#include "core/usb_busdma.h"
#include "core/usb_process.h"
#include "core/usb_transfer.h"
#include "core/usb_device.h"
#include "core/usb_util.h"
#include "core/usb_debug.h"
#include "core/usb_request.h"
#include "core/usb_dynamic.h"
#include "core/usb_hub.h"
#include "core/usb_ioctl.h"
#include "core/usb_controller.h"
#include "core/usb_bus.h"

#if USB_HAVE_UGEN
#include "core/usb_dev.h"
#endif

#include "core/usb_dev.h"
#include "core/usb_debug.h"
#include "core/usbhid.h"
#include "quirk/usb_quirk.h"
#include "core/usb_mbuf.h"
#include "core/usb_cdc.h"
#include "core/serial.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FREEBSD_USB_H */

