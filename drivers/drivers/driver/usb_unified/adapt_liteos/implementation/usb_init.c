/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2023. All rights reserved.
 * Description: Usb Init File
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

#include <los_memory.h>
#include <los_mux.h>
#include <los_atomic.h>
#include "usb_api_pri.h"
#include "phy/usb_phy.h"
#include "usb_os_adapt.h"
#include "implementation/usb_init.h"
#include "implementation/usb_dma_cache.h"
#ifdef CONFIG_DRIVERS_USB_HOST_DRIVER
#include "implementation/global_implementation.h"
#endif

typedef struct usb_info {
	bool is_initialized;
	controller_type ctype;
	device_type dtype;
} usb_info_t;

#define INVALID_TYPE 0xFFFF
#define USB_IDLE_STATUS      0
#define USB_INIT_STATUS      1

static bool uvc_enable = false;
static usb_info_t usb_info = { false, (controller_type)INVALID_TYPE, (device_type)INVALID_TYPE };
uint32_t usb_mtx = INVALID_MUX;
static volatile Atomic usb_init_atomic = USB_IDLE_STATUS;

#ifdef CONFIG_DRIVERS_USB_HOST_DRIVER
static struct module_data *bsd_driver_mode_list[] = {
#ifdef CONFIG_DRIVERS_USB_HOST_EHCI
	/* xxx controller modules */
	/* usb generial controller modules */
	&bsd_usbus_ehci_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_HOST_OHCI
	/* xxx controller modules */
	/* usb generial controller modules */
	&bsd_usbus_ohci_driver_mod,
#endif

#if defined (CONFIG_DRIVERS_USB_HOST_XHCI)
	&bsd_usbus_xhci_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_HOST_DRIVER
	/* xxx driver modules */
	&bsd_uhub_uhub_driver_mod,
	&bsd_uhub_usbus_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_4G_MODEM
	&bsd_cdce_uhub_driver_mod,
	&bsd_u3g_uhub_driver_mod,
#endif

#if defined (CONFIG_DRIVERS_USB_SERIAL) || defined (CONFIG_DRIVERS_USB_4G_MODEM)
	&bsd_u3g_uhub_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_MASS_STORAGE
	&bsd_umass_uhub_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_ETHERNET
	&bsd_axe_uhub_driver_mod,
	&bsd_axge_uhub_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_RNDIS_HOST
	&bsd_urndis_uhub_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_HOST_UVC_CLASS
	&bsd_uvc_uhub_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_HOST_HID
	&bsd_uhid_uhub_driver_mod,
#endif

#ifdef CONFIG_DRIVERS_USB_WIRELESS
	&bsd_usb_linux_uhub_driver_mod,
#endif

	NULL
};
#endif

static void
usbinfo_set(controller_type ctype, device_type dtype)
{
	if ((dtype == DEV_UVC) || (dtype == DEV_CAMERA)) {
		uvc_enable = true;
	}
	usb_info.ctype = ctype;
	usb_info.dtype = dtype;
	usb_info.is_initialized = true;
}

static void
usbinfo_clean(void)
{
	uvc_enable = false;
	usb_info.ctype = (controller_type)INVALID_TYPE;
	usb_info.dtype = (device_type)INVALID_TYPE;
	usb_info.is_initialized = false;
}

device_type
dev_type_get(void)
{
	return (usb_info.dtype);
}

bool
device_is_uvc(void)
{
	return (uvc_enable);
}

static uint32_t
usb_loadonce(void)
{
	dprintf("usb %s\n", fetach_usbversion());

#ifdef CONFIG_DRIVERS_USB
	/* init dma memory */
	uint32_t ret = LOS_MemInit((void *)g_usb_mem_addr_start, g_usb_mem_size);
	if (ret != LOS_OK) {
		dprintf("*** usb init memory error 0x%x!! ***\n\n", ret);
		return (1);
	}

#ifdef CONFIG_DRIVERS_USB_HOST_DRIVER
	/* init quirk */
	usb_quirk_init(NULL);

	for (uint32_t i = 0; bsd_driver_mode_list[i] != NULL; i++) {
		module_register(bsd_driver_mode_list[i]);
	}
	devclass_module_dump();

#ifdef CONFIG_USB_DEBUG
	usb_debug_module_regsiter();
#endif
#endif
#endif

	return (0);
}

static void
usb_unloadonce(void)
{
#ifdef CONFIG_DRIVERS_USB
#ifdef CONFIG_DRIVERS_USB_HOST_DRIVER
#ifdef CONFIG_USB_DEBUG
	usb_debug_module_unregsiter();
#endif

	for (uint32_t i = 0; bsd_driver_mode_list[i] != NULL; i++) {
		module_unregister(bsd_driver_mode_list[i]);
	}
#endif

#ifdef CONFIG_MEM_MUL_POOL
	(void)LOS_MemDeInit((void *)g_usb_mem_addr_start);
#endif

	(void)memset_s((void *)g_usb_mem_addr_start, g_usb_mem_size, 0, g_usb_mem_size);
#endif
}

/*
 * step1: Modify DRIVER_MODULE, register all host driver modules or
 * modify g_usb_dev_init, register callback functions for all device protocols
 * step2: Load host controller(ehci/xhci) or device controlle(dwc2.0/dwc3.0)
 * step3: Load usb host or device protocol
 */
uint32_t
usb_init(controller_type ctype, device_type dtype)
{
	uint32_t ret = LOS_NOK;
	static bool usb_loaded = false;

	usb_phy_init();

	if (LOS_AtomicCmpXchg32bits(&usb_init_atomic, USB_INIT_STATUS, USB_IDLE_STATUS)) {
		return (LOS_NOK);
	}

	/*
	 * After the mutex resource application is successful, it will not be deleted
	 * because LiteOS does not support global static mutexes.
	 */
	if (usb_mtx == INVALID_MUX) {
		if (LOS_MuxCreate(&usb_mtx) != LOS_OK) {
			LOS_AtomicSet(&usb_init_atomic, USB_IDLE_STATUS);
			return (LOS_NOK);
		}
	}
	LOS_AtomicSet(&usb_init_atomic, USB_IDLE_STATUS);
	dprintf("\n******** %s in **********\n", __FUNCTION__);

	if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK) {
		return (LOS_NOK);
	}

	if (usb_info.is_initialized) {
		dprintf("\n duplicate usb_init %s, ctype:%d dtype:%d\n", __FUNCTION__, usb_info.ctype, usb_info.dtype);
		(void)LOS_MuxPost(usb_mtx);
		return (LOS_NOK);
	}
	usbinfo_set(ctype, dtype);

	if (usb_loaded == false) {
		if (usb_loadonce()) {
			goto err;
		}
	}

	if (ctype == HOST) {
#if defined (CONFIG_DRIVERS_USB_HOST_XHCI) || defined (CONFIG_DRIVERS_USB_HOST_EHCI)
		usb_lock_init(&Giant);
#endif
#if defined (CONFIG_DRIVERS_USB_HOST_XHCI)
		ret = (uint32_t)xhci_hcd_init();
#elif defined (CONFIG_DRIVERS_USB_HOST_EHCI)
		ret = (uint32_t)ehci_hcd_init();
#endif
#if defined (CONFIG_DRIVERS_USB_HOST_OHCI) && !defined (CONFIG_DRIVERS_USB_HOST_XHCI)
		ret = (uint32_t)ohci_hcd_init();
#endif
	} else if (ctype == DEVICE) {
		if ((dtype == DEV_START) || (dtype >= DEV_END)) {
			PRINT_ERR("device type is not supported\n");
			goto err;
		}

#if defined (CONFIG_DRIVERS_USB2_DEVICE_CONTROLLER)
		ret = (uint32_t)udc_init();
#elif defined (CONFIG_DRIVERS_USB3_DEVICE_CONTROLLER)
		ret = (uint32_t)udc3_init();
#endif
	} else {
		PRINT_ERR("controller type %d is error\n", ctype);
		goto err;
	}

	if (ret != LOS_OK) {
#if defined (CONFIG_DRIVERS_USB_HOST_XHCI) || defined (CONFIG_DRIVERS_USB_HOST_EHCI)
		if (ctype == HOST) {
			usb_lock_destroy(&Giant);
		}
#endif
		goto err;
	}

	usb_loaded = true;
	(void)LOS_MuxPost(usb_mtx);
	dprintf("******** %s ok**********\n\n", __FUNCTION__);
	return (LOS_OK);

err:
	usbinfo_clean();
	(void)LOS_MuxPost(usb_mtx);
	if (!usb_loaded) {
		usb_unloadonce();
	}
	dprintf("******** %s fail**********\n\n", __FUNCTION__);

	return (LOS_NOK);
}

uint32_t
usb_deinit(void)
{
	uint32_t ret = LOS_NOK;

	dprintf("******** %s in **********\n\n", __FUNCTION__);
	if (LOS_MuxPend(usb_mtx, LOS_WAIT_FOREVER) != LOS_OK) {
		return (LOS_NOK);
	}

	if (usb_info.is_initialized == false) {
		dprintf("******** %s out, no init **********\n\n", __FUNCTION__);
		goto err;
	}

	if (usb_info.ctype == HOST) {
#if defined (CONFIG_DRIVERS_USB_HOST_OHCI) && !defined (CONFIG_DRIVERS_USB_HOST_XHCI)
		ret = (uint32_t)ohci_hcd_exit();
#endif
#if defined (CONFIG_DRIVERS_USB_HOST_XHCI)
		ret = (uint32_t)xhci_hcd_exit();
#elif defined (CONFIG_DRIVERS_USB_HOST_EHCI)
		ret = (uint32_t)ehci_hcd_exit();
#endif
		if (ret) {
			dprintf("******** %s fail, %u **********\n\n", __FUNCTION__, ret);
			goto err;
		}
#if defined (CONFIG_DRIVERS_USB_HOST_XHCI) || defined (CONFIG_DRIVERS_USB_HOST_EHCI)
		usb_lock_destroy(&Giant);
#endif
	} else if (usb_info.ctype == DEVICE) {
#if defined (CONFIG_DRIVERS_USB2_DEVICE_CONTROLLER)
		ret = (uint32_t)udc_exit();
#elif defined (CONFIG_DRIVERS_USB3_DEVICE_CONTROLLER)
		ret = (uint32_t)udc3_exit();
#endif
		if (ret) {
			dprintf("******** %s fail, %u **********\n\n", __FUNCTION__, ret);
			goto err;
		}
	}
	usbinfo_clean();
	dprintf(" ** %s success **\n", __FUNCTION__);
	(void)LOS_MuxPost(usb_mtx);
	return (LOS_OK);

err:
	(void)LOS_MuxPost(usb_mtx);
	return (LOS_NOK);
}

bool
usb_is_devicemode(void)
{
	return (usb_phy_is_device_mode());
}
