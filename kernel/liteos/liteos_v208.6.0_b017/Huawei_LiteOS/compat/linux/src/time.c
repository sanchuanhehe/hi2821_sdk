/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Time
 * Author: Huawei LiteOS Team
 * Create: 2021-07-29
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

#include "linux/jiffies.h"
#include "linux/delay.h"
#include "linux/sched/clock.h"
#include "linux/kernel.h"
#include "limits.h"

void msleep(unsigned int msecs)
{
    LOS_Msleep(msecs);
}

unsigned long long get_jiffies_64(void)
{
    return LOS_TickCountGet();
}

unsigned int jiffies_to_msecs(const unsigned long j)
{
    return LOS_Tick2MS(j);
}

unsigned long msecs_to_jiffies(const unsigned int m)
{
    /* Negative value, means infinite timeout: */
    if ((INT32)m < 0) {
        return (unsigned long)MAX_JIFFY_OFFSET;
    }

#if (HZ <= OS_SYS_MS_PER_SECOND) && !(OS_SYS_MS_PER_SECOND % HZ)
    /*
     * HZ is equal to or smaller than 1000, and 1000 is a nice
     * round multiple of HZ, divide with the factor between them,
     * but round upwards:
     */
    return ((m + (OS_SYS_MS_PER_SECOND / HZ)) - 1) / (OS_SYS_MS_PER_SECOND / HZ);
#else
    PRINT_ERR("HZ: %u is not supported in %s\n", HZ, __FUNCTION__);
    return ENOSUPP;
#endif
}

unsigned long long sched_clock(void)
{
    return LOS_CurrNanosec();
}
