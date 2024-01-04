/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Driver Framework Device Power Management
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

LOS_DL_LIST_HEAD(g_devicePmList);
LOS_DL_LIST_HEAD(g_devicePmPreparedList);
LOS_DL_LIST_HEAD(g_devicePmSuspendedList);

VOID OsDevicePmAdd(struct LosDevice *dev)
{
    PM_LOCK(g_pmListMutex);
    LOS_ListTailInsert(&g_devicePmList, &(dev->pmNode));
    PM_UNLOCK(g_pmListMutex);
}

VOID OsDevicePmRemove(struct LosDevice *dev)
{
    PM_LOCK(g_pmListMutex);
    LOS_ListDelInit(&(dev->pmNode));
    PM_UNLOCK(g_pmListMutex);
}

STATIC INLINE struct LosDevice *OsToDevice(LOS_DL_LIST *entry)
{
    return LOS_DL_LIST_ENTRY(entry, struct LosDevice, pmNode);
}

STATIC INT32 OsDeviceSuspend(struct LosDevice *dev)
{
    if ((dev->driver == NULL) || (dev->driver->pmOps.suspend == NULL)) {
        return LOS_OK;
    }

    return dev->driver->pmOps.suspend(dev);
}

STATIC INT32 OsDeviceResume(struct LosDevice *dev)
{
    if ((dev->driver == NULL) || (dev->driver->pmOps.resume == NULL)) {
        return LOS_OK;
    }

    return dev->driver->pmOps.resume(dev);
}

STATIC INT32 OsDevicePrepare(struct LosDevice *dev)
{
    if ((dev->driver == NULL) || (dev->driver->pmOps.prepare == NULL)) {
        return LOS_OK;
    }

    return dev->driver->pmOps.prepare(dev);
}

STATIC INT32 OsDeviceComplete(struct LosDevice *dev)
{
    if ((dev->driver == NULL) || (dev->driver->pmOps.complete == NULL)) {
        return LOS_OK;
    }

    return dev->driver->pmOps.complete(dev);
}

STATIC INT32 OsPmListSuspend(VOID)
{
    struct LosDevice *dev = NULL;
    INT32 error = LOS_OK;

    PM_LOCK(g_pmListMutex);
    while (!LOS_ListEmpty(&g_devicePmPreparedList)) {
        dev = OsToDevice(g_devicePmPreparedList.pstPrev);
        PM_UNLOCK(g_pmListMutex);
        error = OsDeviceSuspend(dev);
        PM_LOCK(g_pmListMutex);
        if (error != LOS_OK) {
            break;
        }

        if (!LOS_ListEmpty(&dev->pmNode)) {
            LOS_ListDelete(&dev->pmNode);
            LOS_ListTailInsert(&g_devicePmSuspendedList, &dev->pmNode);
        }
    }
    PM_UNLOCK(g_pmListMutex);
    return error;
}

STATIC INT32 OsPmListPrepare(VOID)
{
    struct LosDevice *dev = NULL;
    INT32 error = LOS_OK;

    PM_LOCK(g_pmListMutex);
    while (!LOS_ListEmpty(&g_devicePmList)) {
        dev = OsToDevice(g_devicePmList.pstPrev);
        PM_UNLOCK(g_pmListMutex);
        error = OsDevicePrepare(dev);
        PM_LOCK(g_pmListMutex);
        if (error != LOS_OK) {
            break;
        }
        if (!LOS_ListEmpty(&dev->pmNode)) {
            LOS_ListDelete(&dev->pmNode);
            LOS_ListTailInsert(g_devicePmPreparedList.pstNext, &dev->pmNode);
        }
    }
    PM_UNLOCK(g_pmListMutex);
    return error;
}

STATIC VOID OsPmListResume(VOID)
{
    struct LosDevice *dev = NULL;
    INT32 error;

    PM_LOCK(g_pmListMutex);
    while (!LOS_ListEmpty(&g_devicePmSuspendedList)) {
        dev = OsToDevice(g_devicePmSuspendedList.pstNext);
        PM_UNLOCK(g_pmListMutex);
        error = OsDeviceResume(dev);
        PM_LOCK(g_pmListMutex);
        if (error != LOS_OK) {
            break;
        }

        if (!LOS_ListEmpty(&dev->pmNode)) {
            LOS_ListDelete(&dev->pmNode);
            LOS_ListTailInsert(&g_devicePmPreparedList, &dev->pmNode);
        }
    }
    PM_UNLOCK(g_pmListMutex);
}

STATIC VOID OsPmListComplete(VOID)
{
    struct LosDevice *dev = NULL;
    INT32 error;

    PM_LOCK(g_pmListMutex);
    while (!LOS_ListEmpty(&g_devicePmPreparedList)) {
        dev = OsToDevice(g_devicePmPreparedList.pstNext);
        LOS_ListDelete(&dev->pmNode);
        LOS_ListTailInsert(&g_devicePmList, &dev->pmNode);
        PM_UNLOCK(g_pmListMutex);
        error = OsDeviceComplete(dev);
        PM_LOCK(g_pmListMutex);
        if (error != LOS_OK) {
            break;
        }
    }
    PM_UNLOCK(g_pmListMutex);
}

INT32 LOS_PmSuspend(VOID)
{
    INT32 error;

    error = OsPmListPrepare();
    if (!error) {
        error = OsPmListSuspend();
    }
    return error;
}

VOID LOS_PmResume(VOID)
{
    OsPmListResume();
    OsPmListComplete();
}
