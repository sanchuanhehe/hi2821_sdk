/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: CMSIS Interface V2.0
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
 *---------------------------------------------------------------------------*/

#include "cmsis_os_pri.h"
#include "los_hwi.h"
#include "los_sched_pri.h"
#include "los_task.h"

#ifdef LOSCFG_COMPAT_CMSIS_VER_2
/* Kernel initialization state */
LITE_OS_SEC_RODATA_USRSPACE osKernelState_t  g_kernelState = osKernelInactive;

#define KERNEL_UNLOCKED 0
#define KERNEL_LOCKED   1

//  ==== Kernel Management Functions ====
osStatus_t osKernelInitialize(void)
{
    if (OS_INT_ACTIVE) {
        return osErrorISR;
    }

    if (g_kernelState != osKernelInactive) {
        return osError;
    }

    if (OsMain() == LOS_OK) {
        g_kernelState = osKernelReady;
        return osOK;
    } else {
        return osError;
    }
}

osStatus_t osKernelStart(void)
{
    if (OS_INT_ACTIVE) {
        return osErrorISR;
    }
    if (g_kernelState != osKernelReady) {
        return osError;
    }

    g_kernelState = osKernelRunning;
    OsStart();
    return osOK;
}

int32_t osKernelLock(void)
{
    int32_t lock;

    if (OS_INT_ACTIVE) {
        return (int32_t)osErrorISR;
    }

    if (!g_taskScheduled) {
        return (int32_t)osError;
    }

    if (OsPercpuGet()->taskLockCnt > 0) {
        lock = KERNEL_LOCKED;
    } else {
        LOS_TaskLock();
        lock = KERNEL_UNLOCKED;
    }

    return lock;
}

int32_t osKernelUnlock(void)
{
    int32_t lock;

    if (OS_INT_ACTIVE) {
        return (int32_t)osErrorISR;
    }

    if (!g_taskScheduled) {
        return (int32_t)osError;
    }

    if (OsPercpuGet()->taskLockCnt > 0) {
        LOS_TaskUnlock();
        if (OsPercpuGet()->taskLockCnt != 0) {
            return (int32_t)osError;
        }
        lock = KERNEL_LOCKED;
    } else {
        lock = KERNEL_UNLOCKED;
    }

    return lock;
}

int32_t osKernelRestoreLock(int32_t lock)
{
    if (OS_INT_ACTIVE) {
        return (int32_t)osErrorISR;
    }

    if (!g_taskScheduled) {
        return (int32_t)osError;
    }

    switch (lock) {
        case KERNEL_UNLOCKED:
            LOS_TaskUnlock();
            if (OsPercpuGet()->taskLockCnt != 0) {
                break;
            }
            return KERNEL_UNLOCKED;
        case KERNEL_LOCKED:
            LOS_TaskLock();
            return KERNEL_LOCKED;
        default:
            break;
    }

    return (int32_t)osError;
}
#endif
