/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS USB DMA CACHE Implementation
 * Author: Huawei LiteOS Team
 * Create: 2023-01-05
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

#include <los_typedef.h>
#include <los_base.h>
#include <asm/dma.h>
#include "usb_os_adapt.h"

#if defined CONFIG_DRIVERS_USB_HOST_EHCI || defined CONFIG_DRIVERS_USB2_DEVICE_CONTROLLER
#include USB_HEADER_PATH
#endif

#if defined CONFIG_DRIVERS_USB_HOST_XHCI || defined CONFIG_DRIVERS_USB3_DEVICE_CONTROLLER
#include USB3_HEADER_PATH
#endif

void
usb_dma_cache_invalid(void *addr, unsigned int size)
{
	UINTPTR align = USB_CACHE_ALIGN_SIZE;
	UINTPTR start = (UINTPTR)addr & ~(align - 1);
	UINTPTR end = (UINTPTR)addr + size;

	end = ALIGN(end, USB_CACHE_ALIGN_SIZE);
	USB_DMA_CACHE_INV(start, end);
}

void
usb_dma_cache_flush(void *addr, unsigned int size)
{
	UINTPTR align = USB_CACHE_ALIGN_SIZE;
	UINTPTR start = (UINTPTR)addr & ~(align - 1);
	UINTPTR end = (UINTPTR)addr + size;

	end = ALIGN(end, USB_CACHE_ALIGN_SIZE);
	USB_DMA_CACHE_CLR(start, end);
}
