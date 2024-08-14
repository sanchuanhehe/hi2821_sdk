/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: USB port for project
 * Author: @CompanyNameTag
 * Create:  2023-01-10
 */

#include "usb_phy.h"
#include "stddef.h"

static const usb_phy_ops_t *g_usb_phy_ops = NULL;
static bool g_otg_usb_dev_stat = false;

void usb_phy_reg(const usb_phy_ops_t *usb_phy_ops)
{
    g_usb_phy_ops = usb_phy_ops;
}

void usb_phy_start_hcd(void)
{
    if ((g_usb_phy_ops == NULL) || (g_usb_phy_ops->usbStartHcd == NULL)) {
        return;
    }

    g_usb_phy_ops->usbStartHcd();
}

void usb_phy_stop_hcd(void)
{
    if ((g_usb_phy_ops == NULL) || (g_usb_phy_ops->usbStopHcd == NULL)) {
        return;
    }

    g_usb_phy_ops->usbStopHcd();
}

void usb_phy_host_device(void)
{
    if ((g_usb_phy_ops == NULL) || (g_usb_phy_ops->usbHost2Device == NULL)) {
        return;
    }

    g_usb_phy_ops->usbHost2Device();
}

void usb_phy_device_host(void)
{
    if ((g_usb_phy_ops == NULL) || (g_usb_phy_ops->usbDevice2Host == NULL)) {
        return;
    }

    g_usb_phy_ops->usbDevice2Host();
}

void usb_phy_suspend(void)
{
    if ((g_usb_phy_ops == NULL) || (g_usb_phy_ops->usbSuspend == NULL)) {
        return;
    }

    g_usb_phy_ops->usbSuspend();
}

void usb_phy_resume(void)
{
    if ((g_usb_phy_ops == NULL) || (g_usb_phy_ops->usbResume == NULL)) {
        return;
    }

    g_usb_phy_ops->usbResume();
}

void usb_phy_set_device_state(void)
{
    g_otg_usb_dev_stat = true;
}

void usb_phy_clear_device_state(void)
{
    g_otg_usb_dev_stat = false;
}

bool usb_phy_is_device_mode(void)
{
    return g_otg_usb_dev_stat;
}
