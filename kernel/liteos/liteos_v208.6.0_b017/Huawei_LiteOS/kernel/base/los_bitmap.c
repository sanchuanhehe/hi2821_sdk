/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: BitMap
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

#include "los_bitmap.h"
#include "los_config.h"

#define OS_BITMAP_MASK 0x1FU

VOID LOS_BitmapSet(UINT32 *bitmap, UINT16 pos)
{
    if (bitmap == NULL) {
        return;
    }

    *bitmap |= 1U << (pos & OS_BITMAP_MASK);
}

VOID LOS_BitmapClr(UINT32 *bitmap, UINT16 pos)
{
    if (bitmap == NULL) {
        return;
    }

    *bitmap &= ~(1U << (pos & OS_BITMAP_MASK));
}

UINT16 LOS_HighBitGet(UINT32 bitmap)
{
    if (bitmap == 0) {
        return LOS_INVALID_BIT_INDEX;
    }

    return (UINT16)(OS_BITMAP_MASK - (UINT32)CLZ(bitmap));
}

UINT16 LOS_LowBitGet(UINT32 bitmap)
{
    if (bitmap == 0) {
        return LOS_INVALID_BIT_INDEX;
    }

    return CTZ(bitmap);
}
