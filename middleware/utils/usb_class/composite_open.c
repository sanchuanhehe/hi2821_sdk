/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: LiteOS USB composite gets device descriptor
 * Author: Huawei LiteOS Team
 * Create: 2020-11-06
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

#include "gadget/composite_pri.h"
#include "gadget/composite_gadget.h"
#include "securec.h"

typedef struct
{
  device_type type;
  int (*callback)(void *handle);
} dev_init_ops;

static dev_init_ops g_usb_dev_init[] =
{
#ifdef CONFIG_DRIVERS_USB_MASS_STORAGE_GADGET
  { DEV_MASS, usbdev_mass_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_UVC_GADGET
  { DEV_UVC, usbdev_uvc_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_UAC_GADGET
  { DEV_UAC, usbdev_uac_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_CAMERA_GADGET
  { DEV_CAMERA, usbdev_camera_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
  { DEV_SERIAL, usbdev_acm_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_ETHERNET_GADGET
  { DEV_ETHERNET, usbdev_rndis_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_ETH_SER_GADGET
  { DEV_SER_ETH, usbdev_multi_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_DFU_GADGET
  { DEV_DFU, usbdev_dfu_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_HID_GADGET
  { DEV_HID, usbdev_hid_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_UAC_HID_GADGET
  { DEV_UAC_HID, usbdev_uac_hid_initialize },
#endif

#ifdef CONFIG_DRIVERS_USB_CUSTOM_GADGET
  { DEV_CUSTOM, usbdev_test_initialize },
#endif
  { DEV_END, NULL }
};

static void get_dev_desc_var(device_type dtype,
                             struct usbd_string **device_strings,
                             struct usb_devdesc_s **device_desc)
{
  switch (dtype)
    {
#ifdef CONFIG_DRIVERS_USB_MASS_STORAGE_GADGET
    case DEV_MASS:
      mass_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_UVC_GADGET
    case DEV_UVC:
      uvc_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_UAC_GADGET
    case DEV_UAC:
      uac_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_CAMERA_GADGET
    case DEV_CAMERA:
      camera_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
    case DEV_SERIAL:
      acm_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_ETHERNET_GADGET
    case DEV_ETHERNET:
      eth_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_ETH_SER_GADGET
    case DEV_SER_ETH:
      multi_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_DFU_GADGET
    case DEV_DFU:
      dfu_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_HID_GADGET
    case DEV_HID:
      hid_get_device_dec_info(device_strings, device_desc);
      break;
#endif

#ifdef CONFIG_DRIVERS_USB_UAC_HID_GADGET
    case DEV_UAC_HID:
      uac_hid_get_device_dec_info(device_strings, device_desc);
      break;
#endif

    default:
      *device_strings = NULL;
      *device_desc    = NULL;
      usb_err("device type %d error\n", dtype);
      break;
    }
}

uint32_t usbd_set_device_info(device_type dtype,
                              const struct device_string *str_manufacturer,
                              const struct device_string *str_product,
                              const struct device_string *str_serial_number,
                              struct device_id dev_id)
{
  char *buf[STRING_DESC_NUM] = {NULL};
  const struct device_string *str_desc[STRING_DESC_NUM];
  struct usbd_string *device_strings;
  struct usb_devdesc_s *device_desc;
  uint32_t len, flags;
  int i;

  if (str_manufacturer == NULL || str_product == NULL || str_serial_number == NULL)
    {
      usb_err("failed, invalid param!\n");
      return LOS_NOK;
    }

  if (str_manufacturer->str == NULL || str_manufacturer->len == 0 ||
      str_serial_number->str == NULL || str_serial_number->len == 0 ||
      str_product->str == NULL || str_product->len == 0)
    {
      usb_err("failed, str is NULL or len is 0\n");
      return LOS_NOK;
    }

  if (str_manufacturer->len > (STRING_LEN_MAX - STRING_HEAD_LEN) ||
      str_serial_number->len > (STRING_LEN_MAX - STRING_HEAD_LEN) ||
      str_product->len > (STRING_LEN_MAX - STRING_HEAD_LEN))
    {
      usb_err("%s failed, len exceeds maximum limit 253! str_manufacturer->len = %u"
              "str_serial_number->len = %u str_product->len = %u\n", __FUNCTION__,
              str_manufacturer->len, str_serial_number->len, str_product->len);
      return LOS_NOK;
    }

  get_dev_desc_var(dtype, &device_strings, &device_desc);
  if (device_desc == NULL || device_strings == NULL)
    {
      usb_err("device_desc or device_strings is NULL!\n");
      return LOS_NOK;
    }

  USB_SETW(device_desc->vendor, dev_id.vendor_id);
  USB_SETW(device_desc->product, dev_id.product_id);
  USB_SETW(device_desc->device, dev_id.release_num);

  dev_str_desc_free(device_strings);

  str_desc[0] = str_manufacturer;
  str_desc[1] = str_product;
  str_desc[2] = str_serial_number;

  flags = composite_spin_lock_irqsave();

  for (i = 0; i < STRING_DESC_NUM; i++)
    {
      len = str_desc[i]->len + STRING_HEAD_LEN;
      buf[i] = (char *)malloc(len);
      if (buf[i] == NULL)
        {
          usb_err("malloc failed\n");
          goto errout;
        }
      device_strings[i].s = buf[i];

      *buf[i] = (char)len;
      *(buf[i] + 1) = USB_DESC_TYPE_STRING;

      /* Len represents the size of the string */

      (void)memcpy_s(buf[i] + STRING_HEAD_LEN, (size_t)str_desc[i]->len,
                     str_desc[i]->str, (size_t)str_desc[i]->len);
    }

  composite_spin_unlock_irqrestore(flags);
  return LOS_OK;

errout:
  composite_spin_unlock_irqrestore(flags);
  dev_str_desc_free(device_strings);
  return LOS_NOK;
}

static int composite_device_init(struct gadget_device *dev)
{
  device_type type = dev_type_get();

  for (uint32_t i = 0; g_usb_dev_init[i].callback != NULL; i++)
    {
      if (g_usb_dev_init[i].type == type)
        {
          return g_usb_dev_init[i].callback((void *)dev);
        }
    }

  return -1;
}

int composite_attach(struct gadget_device *dev)
{
  struct composite_alloc_s *usb_cdev;
  void *parnet_conext = dev->udc_sc;
  int ret;

  /* Probe and initiate device driver */

  ret = composite_device_init(dev);
  if (ret < 0)
    {
      usb_err("composite device init failed!, ret:%d\n", ret);
      return -1;
    }

  usb_cdev = dev->composite_sc;
  usb_cdev->drvr.dev = &usb_cdev->dev;
  ret = usbd_gadget_attach_driver(parnet_conext, &usb_cdev->drvr.drvr);
  if (ret < 0)
    {
      usb_err("failed, err=%d!\n", ret);
      goto detach;
    }

  return 0; /* Attach success */

detach:
  (void)composite_detach(dev);
  return -1; /* No such device or address */
}

int composite_detach(struct gadget_device *dev)
{
  struct composite_alloc_s *usb_cdev = dev->composite_sc;
  void *parnet_conext = dev->udc_sc;
  int ret;

  ret = usbd_gadget_detach_driver(parnet_conext, &usb_cdev->drvr.drvr);
  if (ret != 0)
    {
      usb_err("failed, err=%d!\n", ret);
      return ret;
    }
  composite_uninitialize(usb_cdev);

  return (0);
}
