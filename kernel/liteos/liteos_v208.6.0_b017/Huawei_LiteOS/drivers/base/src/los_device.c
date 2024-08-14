/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Driver Framework Device Management
 * Author: Huawei LiteOS Team
 * Create: 2020-08-15
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

#include "los_driverbase_pri.h"

STATIC UINT32 OsDeviceInit(struct LosDevice *dev)
{
    UINT32 muxHandle;
    UINT32 ret;

    ret = LOS_MuxCreate(&muxHandle);
    if (ret != LOS_OK) {
        return LOS_ERRNO_DRIVER_DEVICE_INITIALFAIL;
    }

    dev->mutex = muxHandle;
    dev->driver = NULL;

    return LOS_OK;
}

STATIC VOID OsDeviceDeinit(struct LosDevice *dev)
{
    (VOID)LOS_MuxDelete(dev->mutex);
}

STATIC UINT32 OsDeviceAttach(struct LosDevice *dev)
{
    UINT32 ret = LOS_OK;
    struct LosDriver *drv = NULL;

    DEVICE_LOCK(dev);

    if (dev->driver == NULL) {
        LOS_DL_LIST_FOR_EACH_ENTRY(drv, &g_driverList, struct LosDriver, driverItem) {
            if (OsDeviceMatchDriver(dev, drv)) {
                ret = OsDriverAttachDevice(drv, dev);
            }
        }
    }

    DEVICE_UNLOCK(dev);
    return ret;
}

STATIC UINT32 OsDeviceDetach(struct LosDevice *dev)
{
    UINT32 ret = LOS_OK;

    DEVICE_LOCK(dev);

    if (dev->driver != NULL) {
        ret = OsDriverDetachDevice(dev->driver, dev);
    }

    DEVICE_UNLOCK(dev);

    return ret;
}

STATIC UINT32 OsDeviceAdd(struct LosDevice *dev)
{
    OsSysAddDevice(dev);
    OsDevicePmAdd(dev);

    /*
     * When device is registering ahead of the matched driver registration,
     * the matched driver would not be found. It should return ok to let the
     * registration continue, and the attaching can take place when the
     * the matched driver is registered.
     */
    return OsDeviceAttach(dev);
}

STATIC VOID OsDeviceDel(struct LosDevice *dev)
{
    if (OsDeviceDetach(dev) == LOS_OK) {
        OsDevicePmRemove(dev);
        OsSysRemoveDevice(dev);
    }
}

UINT32 LOS_DeviceRegister(struct LosDevice *dev)
{
    UINT32 ret;

    if ((dev == NULL) || (dev->name == NULL)) {
        return LOS_ERRNO_DRIVER_INPUT_INVALID;
    }

    if (dev->isRegistered) {
        return LOS_ERRNO_DRIVER_DEVICE_REGISTERED;
    }

    ret = OsDeviceInit(dev);
    if (ret != LOS_OK) {
        goto OUT;
    }

    ret = OsDeviceAdd(dev);
    if (ret != LOS_OK) {
        goto OUT_DEINIT;
    }

    return ret;

OUT_DEINIT:
    OsDeviceDeinit(dev);
OUT:
    return ret;
}

VOID LOS_DeviceUnregister(struct LosDevice *dev)
{
    if (dev == NULL) {
        return;
    }

    if (!dev->isRegistered) {
        return;
    }

    OsDeviceDel(dev);
    OsDeviceDeinit(dev);
}

VOID *LOS_DeviceDataGet(const struct LosDevice *dev)
{
    if (dev == NULL) {
        return NULL;
    }

    return dev->data;
}

UINTPTR LOS_DeviceRegBaseGet(const struct LosDevice *dev, UINT32 index)
{
    if ((dev == NULL) || (index >= dev->cfg.numRegs)) {
        return LOS_ERRNO_DRIVER_INPUT_INVALID;
    }

    return dev->cfg.regs[index].base;
}

UINTPTR LOS_DeviceRegSizeGet(const struct LosDevice *dev, UINT32 index)
{
    if ((dev == NULL) || (index >= dev->cfg.numRegs)) {
        return LOS_ERRNO_DRIVER_INPUT_INVALID;
    }

    return dev->cfg.regs[index].size;
}

UINTPTR LOS_DeviceIrqNumGet(const struct LosDevice *dev)
{
    if (dev == NULL) {
        return LOS_ERRNO_DRIVER_INPUT_INVALID;
    }

    return dev->cfg.irqNum;
}
