/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Driver Framework Driver Management
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
#include "los_init.h"

LOS_DL_LIST g_deviceList;
LOS_DL_LIST g_driverList;
UINT32 g_driverBaseMutex;
UINT32 g_pmListMutex;

UINT32 OsDriverBaseInit(VOID)
{
    UINT32 ret;

    LOS_ListInit(&g_deviceList);
    LOS_ListInit(&g_driverList);

    ret = LOS_MuxCreate(&g_driverBaseMutex);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = LOS_MuxCreate(&g_pmListMutex);
    if (ret != LOS_OK) {
        (VOID)LOS_MuxDelete(g_driverBaseMutex);
    }

    return ret;
}
LOS_SYS_INIT(OsDriverBaseInit, SYS_INIT_LEVEL_COMPONENT, SYS_INIT_SYNC_0);

UINT32 OsDriverAttachDevice(struct LosDriver *drv, struct LosDevice *dev)
{
    INT32 ret;

    dev->driver = drv;

    if (drv->ops.probe != NULL) {
        ret = drv->ops.probe(dev);
        if (ret != LOS_OK) {
            PRINT_WARN("probing device %s with driver %s failed\n", dev->name, drv->name);
            dev->driver = NULL;
            return LOS_ERRNO_DRIVER_DRIVER_PROBE_FAIL;
        }

        OsDeviceBindDriver(dev, drv);
    }

    return LOS_OK;
}

UINT32 OsDriverDetachDevice(struct LosDriver *drv, struct LosDevice *dev)
{
    INT32 ret;

    if (drv->ops.remove != NULL) {
        ret = drv->ops.remove(dev);
        if (ret != LOS_OK) {
            PRINT_ERR("remove device %s with driver %s failed\n", dev->name, drv->name);
            return LOS_ERRNO_DRIVER_DEVICE_BUSY;
        }
    }

    OsDeviceUnbindDriver(dev, drv);
    dev->driver = NULL;

    return LOS_OK;
}

STATIC UINT32 OsDriverAttach(struct LosDriver *drv)
{
    struct LosDevice *dev = NULL;
    UINT32 ret = LOS_OK;

    DRIVER_LOCK(drv);

    LOS_DL_LIST_FOR_EACH_ENTRY(dev, &g_deviceList, struct LosDevice, deviceItem) {
        if (OsDeviceMatchDriver(dev, drv) && (dev->driver == NULL)) {
            ret = OsDriverAttachDevice(drv, dev);
        }
    }

    DRIVER_UNLOCK(drv);

    return ret;
}

STATIC UINT32 OsDriverDetach(struct LosDriver *drv)
{
    struct LosDevice *dev = NULL;
    LOS_DL_LIST *prev = NULL;
    UINT32 ret = LOS_OK;

    DRIVER_LOCK(drv);

    LOS_DL_LIST_FOR_EACH_ENTRY(dev, &(drv->deviceList), struct LosDevice, driverNode) {
        prev = dev->driverNode.pstPrev;
        ret = OsDriverDetachDevice(drv, dev);
        if (ret != LOS_OK) {
            DRIVER_UNLOCK(drv);
            return ret;
        }
        dev = LOS_DL_LIST_ENTRY(prev, struct LosDevice, driverNode);
    }

    DRIVER_UNLOCK(drv);

    return ret;
}

UINT32 LOS_DriverRegister(struct LosDriver *drv)
{
    UINT32 ret;

    if ((drv == NULL) || (drv->name == NULL)) {
        return LOS_ERRNO_DRIVER_INPUT_INVALID;
    }

    if (drv->isRegistered) {
        return LOS_ERRNO_DRIVER_DRIVER_REGISTERED;
    }

    LOS_ListInit(&drv->deviceList);
    ret = LOS_MuxCreate(&drv->mutex);
    if (ret != LOS_OK) {
        return LOS_ERRNO_DRIVER_MUX_FAIL;
    }

    OsSysAddDriver(drv);

    ret = OsDriverAttach(drv);
    if (ret != LOS_OK) {
        OsSysRemoveDriver(drv);
        (VOID)LOS_MuxDelete(drv->mutex);
        return ret;
    }

    return LOS_OK;
}

UINT32 LOS_DriverUnregister(struct LosDriver *drv)
{
    UINT32 ret;

    if ((drv == NULL) || (drv->name == NULL)) {
        return LOS_ERRNO_DRIVER_INPUT_INVALID;
    }

    if (!drv->isRegistered) {
        return LOS_ERRNO_DRIVER_DRIVER_NOTFOUND;
    }

    ret = OsDriverDetach(drv);
    if (ret != LOS_OK) {
        return ret;
    }

    OsSysRemoveDriver(drv);

    (VOID)LOS_MuxDelete(drv->mutex);

    return LOS_OK;
}
