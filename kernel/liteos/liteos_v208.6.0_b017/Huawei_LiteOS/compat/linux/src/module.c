/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: initcall for startup stage
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

#include "linux/module.h"
#include "los_init.h"

/* sections for initcall levels */
extern InitcallFunc __initcall0_start;
extern InitcallFunc __initcall0_end;
extern InitcallFunc __initcall1_start;
extern InitcallFunc __initcall1_end;
extern InitcallFunc __initcall2_start;
extern InitcallFunc __initcall2_end;
extern InitcallFunc __initcall3_start;
extern InitcallFunc __initcall3_end;

#define START(level)            &__initcall##level##_start
#define END(level)              &__initcall##level##_end
#define INITCALL_START(level)   START(level)
#define INITCALL_END(level)     END(level)
#define START_END_COUNT         2
#define ARRAY_SIZE(a)           (sizeof(a) / sizeof((a)[0]))

STATIC INLINE UINT32 DoInitcall(const InitcallFunc *start, const InitcallFunc *end)
{
    UINT32 ret;
    UINT32 index = 0;
    const InitcallFunc *func;
    for (func = start; func != end; ++func, ++index) {
        if (*func == NULL) {
            continue;
        }
        ret = (UINT32)(*func)();
        if (ret != LOS_OK) {
            PRINT_ERR("failed initcall func index:%u, ret:0x%08x\n", index, ret);
            return ret;
        }
    }

    return LOS_OK;
}

UINT32 DoInitcalls(VOID)
{
    /* If you want to call after LEVEL_ARCH, you need to extend the table */
    const InitcallFunc *initcalls[][START_END_COUNT] = {
        {INITCALL_START(LEVEL_PURE),        INITCALL_END(LEVEL_PURE)},
        {INITCALL_START(LEVEL_CORE),        INITCALL_END(LEVEL_CORE)},
        {INITCALL_START(LEVEL_POSTCORE),    INITCALL_END(LEVEL_POSTCORE)},
        {INITCALL_START(LEVEL_ARCH),        INITCALL_END(LEVEL_ARCH)},
    };

    for (UINT32 i = 0; i < ARRAY_SIZE(initcalls); ++i) {
        if (DoInitcall(initcalls[i][0], initcalls[i][1]) != LOS_OK) {
            PRINT_ERR("failed initcall level:%u.\n", i);
            return LOS_NOK;
        }
    }
    return LOS_OK;
}
LOS_SYS_INIT(DoInitcalls, SYS_INIT_LEVEL_COMPONENT, SYS_INIT_SYNC_1);
