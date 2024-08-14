/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2023. All rights reserved.
 * Description: LiteOS USB Driver HID Protocol
 * Author: Huawei LiteOS Team
 * Create: 2019-10-24
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

#include "gadget/f_hid_pri.h"
#include "gadget/usbd_hid.h"
#include "implementation/usb_list.h"
#ifdef CONFIG_DRIVERS_USB2_DEVICE_CONTROLLER
#include "controller/usb_device/dwc_otg_pcd.h"
#endif

static int usbclass_hid_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_hid_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);
static int usbclass_hid_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen);
static void usbclass_hid_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev);

struct hid_protocol_desc
{
  uint8_t *report_desc[HID_REPORT_MAP_NUM];
  size_t report_len[HID_REPORT_MAP_NUM];
  uint8_t report_protocol[HID_REPORT_MAP_NUM];
  uint8_t report_num;
};

struct usb_hid_desc
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bcdHID[2];
  uint8_t bCountryCode;
  uint8_t bNumDescriptors;
  struct
    {
      uint8_t bDescriptorType;
      uint8_t wDescriptorLength[2];
    } descrs[1];
} __attribute__ ((packed));

#define USB_DESC_TYPE_HID   0x21
#define USB_HID_REPORT      0x22

#define USB_GET_REPORT      0x01
#define USB_GET_IDLE        0x02
#define USB_GET_PROTOCOL    0x03
#define USB_REQ_GET_HIDDESC 0x06
#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
#define USB_SET_REPORT      0x09
#endif
#define USB_SET_IDLE        0x0a
#define USB_SET_PROTOCOL    0x0b

SPIN_LOCK_INIT(g_hid_report_spinlock);

static struct hid_protocol_desc g_fhid_report_map = {0};

/* USB driver operations */

static const struct usbdevclass_driverops_s g_hid_driverops =
{
  usbclass_hid_bind,
  usbclass_hid_unbind,
  usbclass_hid_setup,
  usbclass_hid_disconnect,
  NULL,
  NULL
};

#define HID_STR_LANG 4
static const char g_fhid_str_lang[HID_STR_LANG] =
{
  HID_STR_LANG,
  USB_DESC_TYPE_STRING,
  0x09, 0x04
};

#define HID_STR_IDX_INTERFACE 28
static const char g_fhid_str_interface[HID_STR_IDX_INTERFACE] =
{
  HID_STR_IDX_INTERFACE,
  USB_DESC_TYPE_STRING,
  'H', 0, 'I', 0, 'D', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0,
  'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

static struct usbd_string g_fhid_device_strings[6] =
{
  { 0, g_fhid_str_lang },
  { 1, NULL },
  { 2, NULL },
  { 3, NULL },
  { 4, g_fhid_str_interface },
  USBD_DEVICE_STRINGS_END
};

static struct usb_devdesc_s g_fhid_device_desc =
{
  .len          = sizeof(struct usb_devdesc_s),
  .type         = USB_DESC_TYPE_DEVICE, /* Constant for device descriptor */
  HSETW(.usb, USB_VERSION_BCD),         /* USB version required: 2.0 */
  .classid      = 0x00,                 /* Miscellaneous Device Class */
  .subclass     = 0x00,                 /* Common Class */
  .protocol     = 0x00,                 /* Interface Association Descriptor */
  .mxpacketsize = USB_EP_MPS,           /* Control Endpoint packet size */
  HSETW(.device, 0x0100),               /* Device release code */
  .imfgr        = 1,                    /* Manufacturer name, string index */
  .iproduct     = 2,                    /* Product name, string index */
  .serno        = 3,                    /* Used */
  .nconfigs     = 1                     /* One Configuration */
};

static struct usb_cfgdesc_s g_fhid_config_desc =
{
  .len         = sizeof(struct usb_cfgdesc_s),
  .type        = USB_DESC_TYPE_CONFIG,
  HSETW(.totallen, 0), /* Size of all descriptors, set later */
  .ninterfaces = 0x1,  /* Number of Interfaces */
  .cfgvalue    = 0x1,  /* ID of this configuration */
  .icfg        = 0x0,  /* Index of string descriptor */
  .attr        = 0xa0, /* Bus-powered and remote wakeup */
  .mxpower     = 0x32  /* Maximum power consumption from the bus */
};

static struct usb_ifdesc_s g_fhid_intf_desc =
{
  .len      = sizeof(struct usb_ifdesc_s),
  .type     = USB_DESC_TYPE_INTERFACE,
  .ifno     = 0,    /* Index number of this interface */
  .alt      = 0,    /* Index of this settings */
  .neps     = 1,    /* Number of endpoint */
  .classid  = 0x03, /* bInterfaceClass: HID */
  .subclass = 1,    /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  .protocol = 0,    /* bInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  .iif      = 0     /* Index of string descriptor */
};

static struct usb_hid_desc g_fhid_desc =
{
  .bLength          = sizeof(struct usb_hid_desc),
  .bDescriptorType  = USB_DESC_TYPE_HID, /* HID type is 0x21 */
  HSETW(.bcdHID, 0x0110),                /* bcdHID: HID Class Spec release number HID 1.1 */
  .bCountryCode     = 0x00,              /* bCountryCode: Hardware target country */
  .bNumDescriptors  = 0x01,              /* bNumDescriptors: Number of HID class descriptors to follow */
  {
    {
      .bDescriptorType = 0x22,           /* bDescriptorType */
    }
  }
};

static struct usb_epdesc_s g_fhid_in_ep_desc =
{
  .len      = sizeof(struct usb_epdesc_s),
  .type     = USB_DESC_TYPE_ENDPOINT,
  .addr     = USB_DIR_IN | 0x01,
  .attr     = 0x03,                       /* bmAttributes = 00000011b */
  HSETW(.mxpacketsize, HID_IN_DATA_SIZE), /* wMaxPacketSize = 64 */
  .interval = 1                           /* bInterval = 125us */
};

/* fhid desc array includes:
 * 1. config_desc (for all report map)
 * 2. iface_desc、hid_desc、in ep_desc (report map 0)
 * 3. iface_desc、hid_desc、in ep_desc (report map 1)
 * ...
 * n+1. iface_desc、hid_desc、in ep_desc (report map n-1)
 *
 * g_fhid_desc_array is like this:
 *
 * config_desc  iface_desc(0)  hid_desc(0)  ep_desc(0)  iface_desc(1)  hid_desc(1)  ep_desc(1) ... NULL
 *                  |                          |           |                           |
 *                  |_ _ _  report map 0  _ _ _|           |_ _ _  report map 1  _ _ _ |
 *
 * Total Length : 2 + (4 - 1) * n
 */

#define HID_SINGLE_PROTOCOL_DESC_NUM 4
#define HID_DESC_ARRAY_MAX_NUM (2 + (HID_SINGLE_PROTOCOL_DESC_NUM - 1) * HID_REPORT_MAP_NUM)
static const uint8_t *g_fhid_desc_array[HID_DESC_ARRAY_MAX_NUM] =
{
  (const uint8_t *)&g_fhid_config_desc,
  (const uint8_t *)&g_fhid_intf_desc,
  (const uint8_t *)&g_fhid_desc,
  (const uint8_t *)&g_fhid_in_ep_desc,
  NULL,
};

static int hid_report_descriptor_set(const uint8_t *report_desc, size_t report_desc_len,
                                     uint8_t protocol, uint8_t report_max_num)
{
  uint8_t *report_buf;
  uint8_t index = g_fhid_report_map.report_num;

  if (report_desc == NULL || report_desc_len == 0)
    {
      usb_err("failed, report_desc is NULL or report_desc_len is 0\n");
      return -1;
    }

  if (index >= report_max_num)
    {
      usb_err("report descriptor already configured!\n");
      return -1;
    }

  report_buf = (uint8_t *)malloc(report_desc_len);
  if (report_buf == NULL)
    {
      usb_err("malloc failed\n");
      return -1;
    }

  (void)memcpy_s(report_buf, report_desc_len, report_desc, report_desc_len);
  g_fhid_report_map.report_desc[index] = report_buf;
  g_fhid_report_map.report_len[index] = report_desc_len;
  g_fhid_report_map.report_protocol[index] = protocol;
  g_fhid_report_map.report_num++;

  return (int)index;
}

static void hid_report_descriptor_clear(void)
{
  for (uint8_t i = 0; i < g_fhid_report_map.report_num; i++)
    {
      if (g_fhid_report_map.report_desc[i] != NULL)
        {
          free(g_fhid_report_map.report_desc[i]);
          g_fhid_report_map.report_desc[i] = NULL;
        }
    }
    g_fhid_report_map.report_num = 0;
}

#ifdef CONFIG_DRIVERS_USB_HID_FUNC_INTERFACE
int hid_add_report_descriptor(const uint8_t *report_desc, size_t report_desc_len, uint8_t protocol)
{
  uint32_t flags;
  int ret;

  LOS_SpinLockSave(&g_hid_report_spinlock, &flags);
  ret = hid_report_descriptor_set(report_desc, report_desc_len, protocol, HID_REPORT_MAP_NUM);
  LOS_SpinUnlockRestore(&g_hid_report_spinlock, flags);
  return ret;
}
#else /* CONFIG_DRIVERS_USB_HID_VFS_INTERFACE */
int hid_report_descriptor_info(const void *report_desc, size_t report_desc_len)
{
  uint32_t flags;
  int ret;

  LOS_SpinLockSave(&g_hid_report_spinlock, &flags);
  ret = hid_report_descriptor_set(report_desc, report_desc_len, 0, 1);
  LOS_SpinUnlockRestore(&g_hid_report_spinlock, flags);
  return ret;
}
#endif /* CONFIG_DRIVERS_USB_HID_FUNC_INTERFACE */

#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
static void fhid_output_request_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct hid_data_ctl *hid_data;
  errno_t ret;
  uint32_t ret_event;
  uint32_t flags;

  if (ep == NULL || req == NULL || req->priv == NULL)
    {
      usb_err("Illegal request or ep!\n");
      return;
    }

  if (req->result != 0)
    {
      return;
    }

  hid_data = (struct hid_data_ctl *)req->priv;

  LOS_SpinLockSave(&hid_data->hid_lock, &flags);

  /* Save private data of read request.
   * Interrupt endpoint: the actual length of the OUT report data is req->xfrd.
   * Control endpoint: the actual length of the OUT report data is hid_data->out_report_len.
   */

  ret = memcpy_s(hid_data->read_buf, HID_OUT_DATA_SIZE, req->buf, hid_data->out_report_len);
  if (ret != EOK)
    {
      LOS_SpinUnlockRestore(&hid_data->hid_lock, flags);
      usb_err("memcpy fail!\n");
      return;
    }
  hid_data->read_len = hid_data->out_report_len;
  LOS_SpinUnlockRestore(&hid_data->hid_lock, flags);

  ret_event = LOS_EventWrite(&hid_data->read_event, USB_HID_READ_EVENT);
  if (ret_event != LOS_OK)
    {
      usb_err("write event failed!\r\n");
    }
}
#endif

static void fhid_input_req_complete(struct usbdev_ep_s *ep, struct usbdev_req_s *req)
{
  struct hid_data_ctl *hid_data = (struct hid_data_ctl *)ep->priv;
  uint32_t flags;
  (void)req;

  LOS_SpinLockSave(&hid_data->hid_lock, &flags);

  hid_data->write_head = (hid_data->write_head + 1) % HID_WRITE_BUF_NUM;
  hid_data->write_busy = 0;

  if (hid_data->write_head == hid_data->write_tail)
    {
      LOS_SpinUnlockRestore(&hid_data->hid_lock, flags);
      return;
    }
  hid_submit_data(hid_data);

  LOS_SpinUnlockRestore(&hid_data->hid_lock, flags);
}

static void fhid_descriptor_array_deinit(void)
{
  for (uint8_t i = HID_SINGLE_PROTOCOL_DESC_NUM; i < HID_DESC_ARRAY_MAX_NUM; i++)
    {
      if (g_fhid_desc_array[i] == NULL)
        {
          break;
        }
      free((void *)g_fhid_desc_array[i]);
      g_fhid_desc_array[i] = NULL;
    }
  g_fhid_config_desc.ninterfaces = 1;
}

/* g_fhid_desc_array: the descriptor in the first report map uses static memory, while the descriptor
 * in the subsequent report map uses dynamic memory, whose content is copied from the first report map,
 * and then modify the ifno of the iface_desc and the addr of the ep_desc.
 */

static int fhid_descriptor_array_init(void)
{
  if (g_fhid_report_map.report_desc[0] == NULL)
    {
      return -1;
    }
  g_fhid_intf_desc.protocol = g_fhid_report_map.report_protocol[0];
  USB_SETW(g_fhid_desc.descrs[0].wDescriptorLength, g_fhid_report_map.report_len[0]);

#if (HID_REPORT_MAP_NUM > 1)
  uint8_t i = 1;
  uint8_t desc_index = HID_SINGLE_PROTOCOL_DESC_NUM;
  struct usb_ifdesc_s *iface_desc;
  struct usb_hid_desc *hid_desc;
  struct usb_epdesc_s *ep_desc;

  for (; i < g_fhid_report_map.report_num; i++)
    {
      iface_desc = malloc(sizeof(struct usb_ifdesc_s));
      if (iface_desc == NULL)
        {
          goto desc_err;
        }
      (void)memcpy_s(iface_desc, sizeof(struct usb_ifdesc_s), &g_fhid_intf_desc, sizeof(struct usb_ifdesc_s));
      iface_desc->protocol = g_fhid_report_map.report_protocol[i];
      iface_desc->ifno = g_fhid_intf_desc.ifno + i;
      g_fhid_desc_array[desc_index++] = (const uint8_t *)iface_desc;

      hid_desc = malloc(sizeof(struct usb_hid_desc));
      if (hid_desc == NULL)
        {
           goto desc_err;
        }
      (void)memcpy_s(hid_desc, sizeof(struct usb_hid_desc), &g_fhid_desc, sizeof(struct usb_hid_desc));
      USB_SETW(hid_desc->descrs[0].wDescriptorLength, g_fhid_report_map.report_len[i]);
      g_fhid_desc_array[desc_index++] = (const uint8_t *)hid_desc;

      ep_desc = malloc(sizeof(struct usb_epdesc_s));
      if (ep_desc == NULL)
        {
          goto desc_err;
        }
      (void)memcpy_s(ep_desc, sizeof(struct usb_epdesc_s), &g_fhid_in_ep_desc, sizeof(struct usb_epdesc_s));
      ep_desc->addr += i;
      g_fhid_desc_array[desc_index++] = (const uint8_t *)ep_desc;

      g_fhid_config_desc.ninterfaces++;
    }

  return 0;

desc_err:
  fhid_descriptor_array_deinit();
  return -1;
#else
  return 0;
#endif
}

static int hid_ep_data_init(struct hid_dev_s *hid, uint8_t index, struct usbdev_s *dev, struct usbdev_devinfo_s *devinfo)
{
  struct hid_data_ctl *hid_data = &hid->hid_data[index];
  struct usb_epdesc_s *ep_desc;
  struct usbdev_ep_s *ep;
  uint8_t desc_index = (HID_SINGLE_PROTOCOL_DESC_NUM - 1) * (1 + index);

  ep_desc = (struct usb_epdesc_s *)g_fhid_desc_array[desc_index];
  if (ep_desc == NULL)
    {
      return 0;
    }

  /* Initialize the interrupt input endpoint */

  ep = DEV_ALLOCEP(dev, ep_desc->addr, ep_desc);
  if (ep == NULL)
    {
      return -1;
    }

  (void)memset_s(&hid_data->inputreq, sizeof(struct usbdev_req_s), 0, sizeof(struct usbdev_req_s));
  hid_data->inputreq.callback = fhid_input_req_complete;
  hid_data->inputreq.priv     = (void *)hid_data;
  hid_data->inputreq.buf      = NULL;
  ep->priv                    = (void *)hid_data;
  ep->handle_req              = &hid_data->inputreq;
  hid_data->in_ep             = ep;
  devinfo->epno[index]        = ep->eplog;
  devinfo->nendpoints++;

  LOS_SpinInit(&hid_data->hid_lock);

  /* When the abnormal branch returns, the applied resources are released in usbclass_hid_unbind */

  hid_data->write_busy = 1;
  hid_data->write_head = 0;
  hid_data->write_tail = 0;

#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
  hid_data->read_len = 0;
  hid_data->read_buf = malloc(HID_OUT_DATA_SIZE);
  if (hid_data->read_buf == NULL)
    {
      return -1;
    }
  (void)LOS_EventInit(&hid_data->read_event);
  hid_data->event_flag = true;
#endif

  return 0;
}

static void hid_ep_data_deinit(struct hid_dev_s *hid, uint8_t index, struct usbdev_s *dev)
{
  struct hid_data_ctl *hid_data = &hid->hid_data[index];

#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
  /* Destroy read event */

  if (hid_data->event_flag == true)
    {
      hid_data->event_flag = false;
      (void)LOS_EventDestroy(&hid_data->read_event);
    }

  if (hid_data->read_buf != NULL)
    {
      free(hid_data->read_buf);
      hid_data->read_buf = NULL;
    }
#endif

  if (hid_data->in_ep != NULL)
    {
      DEV_FREEEP(dev, hid_data->in_ep);
      hid_data->in_ep = NULL;
    }
}

static int usbclass_hid_bind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct hid_driver_s *drvr;
  struct composite_dev_s *cdev;
  struct hid_dev_s *hid;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  int ret = -1;

  if (driver == NULL || dev == NULL)
    {
      return ret;
    }

  cdev = dev->ep0->priv;
  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;
  if (hid == NULL)
    {
      return ret;
    }

  devobj = usbclass_devobj_get(cdev, DEV_HID);
  if (devobj == NULL)
    {
      return ret;
    }

  devinfo = &devobj->compdesc.devinfo;

  hid->connected = 0;

  for (uint8_t index = 0; index < HID_REPORT_MAP_NUM; index++)
    {
      ret = hid_ep_data_init(hid, index, dev, devinfo);
      if (ret != 0)
        {
          goto errout;
        }
    }

  /* Registered character device */

  ret = hid_fops_init(hid);
  if (ret != 0)
    {
      goto errout;
    }

  return ret;
errout:
  (void)usbclass_hid_unbind(driver, dev);
  return ret;
}

static int usbclass_hid_unbind(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct composite_dev_s *cdev;
  struct composite_devobj_s *devobj;
  struct usbdev_devinfo_s *devinfo;
  struct hid_driver_s *drvr;
  struct hid_dev_s *hid;
  uint8_t index;

  if (driver == NULL || dev == NULL)
    {
      return -1;
    }

  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;

#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
  /* If there is a read data task waiting, must write an event to notify the task to exit,
   * otherwise it will never be unbind.
   */

  struct hid_data_ctl *hid_data;

  for (index = 0; index < HID_REPORT_MAP_NUM; index++)
    {
      hid_data = &hid->hid_data[index];
      if (hid_data->event_flag == true && !LOS_ListEmpty(&hid_data->read_event.stEventList))
        {
          (void)LOS_EventWrite(&hid_data->read_event, USB_HID_EXIT_EVENT);
        }
    }
#endif

  if (hid_is_running())
    {
      usb_err("HID device is running! \n");
      return -1;
    }

  (void)hid_fops_deinit(hid);

  usbclass_hid_disconnect(driver, dev);

  for (index = 0; index < HID_REPORT_MAP_NUM; index++)
    {
      hid_ep_data_deinit(hid, index, dev);
    }

  cdev = dev->ep0->priv;
  devobj = usbclass_devobj_get(cdev, DEV_HID);
  if (devobj == NULL)
    {
      return -1;
    }
  devinfo = &devobj->compdesc.devinfo;
  (void)memset_s(devinfo, sizeof(struct usbdev_devinfo_s), 0, sizeof(struct usbdev_devinfo_s));

  return 0;
}

static void usbclass_hid_set_endpoint(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct hid_driver_s *drvr;
  struct hid_dev_s *hid;
  struct hid_data_ctl *hid_data;
  struct usb_epdesc_s *ep_desc;
  int ret;
  uint8_t index;
  uint8_t desc_index;

  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;

  for (index = 0; index < HID_REPORT_MAP_NUM; index++)
    {
      hid_data = &hid->hid_data[index];
      if (hid_data->in_ep_enabled == true)
        {
          (void)EP_DISABLE(hid_data->in_ep);
          hid_data->in_ep_enabled = false;
        }

      desc_index = (HID_SINGLE_PROTOCOL_DESC_NUM - 1) * (1 + index);
      ep_desc = (struct usb_epdesc_s *)g_fhid_desc_array[desc_index];
      if (ep_desc == NULL)
        {
          break;
        }
      ret = EP_CONFIGURE(hid_data->in_ep, ep_desc, 0);
      if (ret < 0)
        {
          usb_err("Config interrupt in_ep failed!\n");
          goto errout;
        }
      hid_data->in_ep_enabled = true;

      hid_data->idle_flag = 0;
      hid_data->report_flag = 0;
#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
      (void)LOS_EventClear(&hid_data->read_event, ~(USB_HID_EXIT_EVENT | USB_HID_READ_EVENT));
#endif
    }

  if (hid->connected == 0)
    {
      hid->connected = 1;
    }

  return;

errout:
  usbclass_hid_disconnect(driver, dev);
}

static void usbclass_hid_get_report(struct usbdev_s *dev, uint16_t len, uint16_t index)
{
  struct usbdev_req_s *req = dev->ep0->handle_req;
  errno_t ret;

  ret = memcpy_s(req->buf, USB_COMP_EP0_BUFSIZ,
                 g_fhid_report_map.report_desc[index], g_fhid_report_map.report_len[index]);
  if (ret != EOK)
    {
      usb_err("memcpy fail\n");
      return;
    }

  req->len = MIN(len, g_fhid_report_map.report_len[index]);
}

static int usbclass_hid_setup(struct usbdevclass_driver_s *driver, struct usbdev_s *dev,
                              const struct usb_ctrlreq_s *ctrl, uint8_t *dataout, size_t outlen)
{
  uint8_t req_type;
  uint8_t transfer_flag = 0;
  uint16_t value;
  uint16_t len;
  uint16_t index;
  struct hid_dev_s *hid;
  struct hid_driver_s *drvr;
  struct usbdev_req_s *req;
  struct hid_data_ctl *hid_data;
  int ret;
  int new_req = 0;
  uint32_t flags;

  (void)dataout; (void)outlen;

  if (dev == NULL || driver == NULL || ctrl == NULL)
    {
      return -1;
    }

  drvr = (struct hid_driver_s *)driver;
  hid  = drvr->dev;
  if (hid == NULL)
    {
      return -1;
    }

  value     = USB_GETW(ctrl->value);
  index     = USB_GETW(ctrl->index) - g_fhid_intf_desc.ifno;
  len       = USB_GETW(ctrl->len);
  req       = dev->ep0->handle_req;
  req_type  = ctrl->type;
  req->priv = hid;
  req->len  = 0;

  if ((req_type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
    {
      switch (ctrl->req)
        {
        case USB_REQ_SETCONFIGURATION:
        case USB_REQ_SETINTERFACE:
          usbclass_hid_set_endpoint(driver, dev);
          break;

        case USB_REQ_GET_HIDDESC:
          {
            if (MSBYTE(value) == USB_HID_REPORT && index < g_fhid_report_map.report_num)
              {
                usbclass_hid_get_report(dev, len, index);
                hid_data = &hid->hid_data[index];
                hid_data->report_flag = 1;
                transfer_flag = hid_data->report_flag ^ hid_data->idle_flag;
                new_req = 1;
              }
          }
          break;

        default:
          break;
        }
    }
  else
    {
      switch (ctrl->req)
        {
        case USB_SET_IDLE:
          {
            if (index < g_fhid_report_map.report_num)
              {
                hid_data = &hid->hid_data[index];
                hid_data->idle_value = MSBYTE(value);
                hid_data->idle_flag = 1;
                transfer_flag = hid_data->report_flag ^ hid_data->idle_flag;
                new_req = 1;
              }
          }
          break;

        case USB_SET_PROTOCOL:
          {
            if (index < g_fhid_report_map.report_num)
              {
                hid->hid_data[index].protocol = (uint8_t)value;
                new_req = 1;
              }
          }
          break;

#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
        case USB_SET_REPORT:
          {
            if (index >= g_fhid_report_map.report_num || hid->hid_data[index].read_buf == NULL)
              {
                return -1;
              }
            (void)memset_s(req->buf, USB_COMP_EP0_BUFSIZ, 0, USB_COMP_EP0_BUFSIZ);
            req->len = MIN(len, USB_COMP_EP0_BUFSIZ);
            req->priv = &hid->hid_data[index];
            req->callback = fhid_output_request_complete;
            hid->hid_data[index].out_report_len = req->len;
            new_req = 1;
          }
          break;
#endif

        case USB_GET_IDLE:
          {
            if (index < g_fhid_report_map.report_num)
              {
                req->buf[0] = hid->hid_data[index].idle_value;
                req->len = 1;
                new_req = 1;
              }
          }
          break;

        case USB_GET_PROTOCOL:
          {
            if (index < g_fhid_report_map.report_num)
              {
                req->buf[0] = hid->hid_data[index].protocol;
                req->len = 1;
                new_req = 1;
              }
          }
          break;

        case USB_GET_REPORT:
          {
            (void)memset_s(req->buf, USB_COMP_EP0_BUFSIZ, 0, USB_COMP_EP0_BUFSIZ);
            req->len = MIN(len, USB_COMP_EP0_BUFSIZ);
            new_req = 1;
          }
          break;

        default:
          break;
        }
    }

  if (new_req)
    {
      ret = EP_SUBMIT(dev->ep0, req);
      if (ret != 0)
        {
          usb_err("Endpoint send fail!\n");
          req->result = 0;
          return -1;
        }

      /* There are four situations when the Host issues a request during a usb enumeration process:
       * 1. Only send a USB_SET_IDLE request once;
       * 2. Only send a USB_REQ_GET_HID_REPORT_DESC request once;
       * 3. Only send one USB_SET_IDLE and one USB_REQ_GET_HID_REPORT_DESC request;
       * 4. Send USB_SET_IDLE and USB_REQ_GET_HID_REPORT_DESC requests multiple times;
       * In these four cases, the data can only be cleared once and the data flow is ready to be started.
       */

      if (transfer_flag)
        {
          dprintf("transfer_flag start:%u\n", index);
          LOS_SpinLockSave(&hid_data->hid_lock, &flags);
          hid_data->write_head = 0;
          hid_data->write_tail = 0;
          hid_data->write_busy = 0;
          LOS_SpinUnlockRestore(&hid_data->hid_lock, flags);
        }
    }
  return 0;
}

static void usbclass_hid_disconnect(struct usbdevclass_driver_s *driver, struct usbdev_s *dev)
{
  struct hid_driver_s *hid_drvr;
  struct hid_dev_s *hid_dev;
  struct hid_data_ctl *hid_data;
  uint32_t flags;
  uint8_t index;

  (void)dev;

  hid_drvr = (struct hid_driver_s *)driver;
  hid_dev  = hid_drvr->dev;
  if (hid_dev == NULL)
    {
      return;
    }
  hid_dev->connected = 0;

  for (index = 0; index < HID_REPORT_MAP_NUM; index++)
    {
      hid_data = &hid_dev->hid_data[index];
      if (hid_data->in_ep_enabled == true)
        {
          LOS_SpinLockSave(&hid_data->hid_lock, &flags);
          hid_data->write_busy = 1;
          LOS_SpinUnlockRestore(&hid_data->hid_lock, flags);

          (void)EP_DISABLE(hid_data->in_ep);
          hid_data->in_ep_enabled = false;
        }

#ifdef CONFIG_DRIVERS_USB_HID_OUTPUT_REPORT
      if (hid_data->event_flag == true && !LOS_ListEmpty(&hid_data->read_event.stEventList))
        {
          /* If there is a read data task waiting, must write an event to notify the task to exit */

          (void)LOS_EventWrite(&hid_data->read_event, USB_HID_EXIT_EVENT);
        }
#endif
    }
}

static uint8_t *link_fhid_descriptors(uint16_t *total_size)
{
  int i;
  uint8_t *des;
  uint8_t *temp_des;
  uint16_t temp_des_len;
  uint16_t ds = 0;
  errno_t ret;

  /* Add the length of descriptors one by one */

  for (i = 0; i < HID_DESC_ARRAY_MAX_NUM && g_fhid_desc_array[i] != NULL; ++i)
    {
      ds += (uint16_t)(*g_fhid_desc_array[i]);
    }

  *total_size = ds;

  temp_des_len = skb_data_align(ds);
  des = memalign(USB_CACHE_ALIGN_SIZE, temp_des_len);
  if (des == NULL)
    {
      usb_err("System out of memory! Descriptors length: %u\n", ds);
      return NULL;
    }
  (void)memset_s((void *)des, temp_des_len, 0, temp_des_len);

  temp_des = des;

  /* Configuration descriptor needs to have the full length of rest of descriptors */

  USB_SETW(g_fhid_config_desc.totallen, ds);

  for (i = 0; i < HID_DESC_ARRAY_MAX_NUM && g_fhid_desc_array[i] != NULL; ++i)
    {
      const uint8_t *des_src = g_fhid_desc_array[i];
      uint8_t des_len        = *des_src;
      ret = memcpy_s(temp_des, temp_des_len, des_src, des_len);
      if (ret != EOK)
        {
          usb_err("memcpy fail!\n");
          free(des);
          return NULL;
        }
      temp_des += des_len;
      temp_des_len -= des_len;
    }

  return des;
}

void hid_get_device_dec_info(struct usbd_string **device_strings, struct usb_devdesc_s **device_desc)
{
  *device_strings = &g_fhid_device_strings[1];
  *device_desc    = &g_fhid_device_desc;
}

static void hid_mkdevdesc(uint8_t *buf)
{
  errno_t ret;

  if (USB_GETW(g_fhid_device_desc.vendor) == 0)
    {
      usb_err("VID is not set!\n");
      return;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, &g_fhid_device_desc, sizeof(g_fhid_device_desc));
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      return;
    }
}

static int16_t hid_mkcfgdesc(uint8_t *buf, struct usbdev_devinfo_s *devinfo)
{
  uint16_t total_len;
  uint8_t *des;
  errno_t ret;

  (void)devinfo;

  des = link_fhid_descriptors(&total_len);
  if (des == NULL)
    {
      return 0;
    }

  ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, des, total_len);
  if (ret != EOK)
    {
      usb_err("memcpy_s fail!, ret:%d\n", ret);
      free(des);
      return 0;
    }
  free(des);

  return (int16_t)total_len;
}

static int hid_mkstrdesc(uint8_t id, struct usb_strdesc_s *buf)
{
  errno_t ret;
  const char *str;
  int i;

  for (i = 0; g_fhid_device_strings[i].s != NULL; i++)
    {
      str = g_fhid_device_strings[i].s;
      if (g_fhid_device_strings[i].id == id)
        {
          ret = memcpy_s(buf, USB_COMP_EP0_BUFSIZ, (const void *)str, (uint32_t)str[0]);
          if (ret != EOK)
            {
              usb_err("memcpy_s failed, ret = %d\n", ret);
              return -1;
            }
          return str[0];
        }
    }

  usb_err("Can not find the id = %u of string\n", id);
  return -1;
}

#define HID_NCONFIGS    1
#define HID_CONFIGID    0
#define HID_NSTRIDS     5
static void hid_get_composite_devdesc(struct composite_devdesc_s *dev)
{
  (void)memset_s(dev, sizeof(struct composite_devdesc_s), 0, sizeof(struct composite_devdesc_s));

  dev->mkdevdesc  = hid_mkdevdesc;
  dev->mkconfdesc = hid_mkcfgdesc;
  dev->mkstrdesc  = hid_mkstrdesc;

  dev->nconfigs = HID_NCONFIGS; /* Number of configurations supported */
  dev->configid = HID_CONFIGID; /* The only supported configuration ID */

  dev->devinfo.nstrings = HID_NSTRIDS; /* Number of Strings */
}

static int hid_classobject(int minor, struct usbdev_devinfo_s *devinfo,
                           struct usbdevclass_driver_s **classdev)
{
  struct hid_softc *hid_s;
  struct hid_dev_s *priv;
  struct hid_driver_s *drvr;

  (void)minor;
  (void)devinfo;

  /* Allocate the structures needed */

  hid_s = (struct hid_softc *)malloc(sizeof(struct hid_softc));
  if (hid_s == NULL)
    {
      return -1;
    }

  /* Convenience pointers into the allocated blob */

  priv = &hid_s->dev;
  drvr = &hid_s->drvr;

  /* Initialize the USB serial driver structure */

  (void)memset_s(priv, sizeof(struct hid_dev_s), 0, sizeof(struct hid_dev_s));

  /* Initialize the USB class driver structure */

  drvr->drvr.speed = USB_SPEED_HIGH;
  drvr->drvr.ops   = &g_hid_driverops;
  drvr->dev        = priv;

  *classdev = &drvr->drvr;
  return 0;
}

static void hid_uninitialize(struct usbdevclass_driver_s *classdev)
{
  struct hid_driver_s *hid_drvr = (struct hid_driver_s *)classdev;
  struct hid_dev_s *priv;
  struct hid_softc *hid_s;
  uint32_t flags;

  dev_str_desc_free(&g_fhid_device_strings[1]);

  LOS_SpinLockSave(&g_hid_report_spinlock, &flags);
  hid_report_descriptor_clear();
  LOS_SpinUnlockRestore(&g_hid_report_spinlock, flags);

  fhid_descriptor_array_deinit();

  if (hid_drvr == NULL)
    {
      return;
    }

  priv = hid_drvr->dev;
  if (priv == NULL)
    {
      return;
    }

  hid_s = container_of(hid_drvr, struct hid_softc, drvr);
  if (hid_s != NULL)
    {
      free(hid_s);
    }
}

void usbdev_hid_initialize_sub(struct composite_devdesc_s *dev, int ifnobase, int minor)
{
  uint32_t flags;

  /* Ask the HID driver to fill in the constants we didn't
   * know here.
   */

  hid_get_composite_devdesc(dev);

  /* The callback functions for the HID class */

  dev->classobject = hid_classobject;
  dev->uninitialize = hid_uninitialize;

  g_fhid_intf_desc.ifno = ifnobase;
  dev->devinfo.ifnobase = ifnobase; /* Offset to Interface-IDs */
  dev->minor = minor;               /* The minor interface number */

  dev->devinfo.strbase = 0;         /* Offset to String Numbers */

  LOS_SpinLockSave(&g_hid_report_spinlock, &flags);
  int ret = fhid_descriptor_array_init();
  if (ret != 0)
    {
      LOS_SpinUnlockRestore(&g_hid_report_spinlock, flags);
      return;
    }
  LOS_SpinUnlockRestore(&g_hid_report_spinlock, flags);
  dev->devinfo.ninterfaces = g_fhid_config_desc.ninterfaces;
}

int usbdev_hid_initialize(void *handle)
{
  struct composite_devdesc_s dev;
  int ret;

  usbdev_hid_initialize_sub(&dev, 0, DEV_HID);

  ret = composite_initialize_softc(1, &dev, handle);
  if (ret < 0)
    {
      return -1;
    }

  PRINTK("  ** hid device initialized successfully! **\n");
  return 0;
}
