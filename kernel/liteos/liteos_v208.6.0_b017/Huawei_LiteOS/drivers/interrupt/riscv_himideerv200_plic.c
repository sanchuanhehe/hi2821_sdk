/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description : Implementation Of The Himideerv200 Interrupt.
 * Author: Huawei LiteOS Team
 * Create : 2020-06-02
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
 * ---------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
 * This file has been modified in accordance with Misra-C specifications.
 * ---------------------------------------------------------------------------- */

#include "arch/regs.h"
#include "riscv_interrupt.h"
#include "los_hwi_pri.h"

#ifdef LOS_LOCATION_VECTOR_IAR
#pragma  location = ".vector"
#endif

#define OS_RIGISTER_BIT_NUM         32U
#define RISCV_HWI_INTERRUPT_OFFSET  1U

STATIC LITE_OS_SEC_BSS HwiHandleInfo g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = { 0 };

#define SET_LOCAL_INTER_NUM_PRI(locpriAddr, hwiNum, priority) do {                      \
    UINT32 interPriVal = READ_CUSTOM_CSR(locpriAddr);                                   \
    /* clear the irqNum-th local interrupt priority */                                  \
    interPriVal &= (~(((UINT32)0xf << (((UINT32)(hwiNum) & 0x7U) << 2)) & UINT32_CUT_MASK));   \
    /* set the irqNum-th local interrupt priority */                                    \
    interPriVal |= ((UINT32)(priority) << (((UINT32)(hwiNum) & 0x7U) << 2));            \
    WRITE_CUSTOM_CSR_VAL(locpriAddr, interPriVal);                                      \
} while (0)

#ifdef LOSCFG_DEBUG_HWI
#define GET_LOCAL_INTER_NUM_PRI(locpriAddr, hwiNum) ({      \
    UINT32 ret = READ_CUSTOM_CSR(locpriAddr);               \
    ret &= 0xfU << (((UINT32)(hwiNum) & 0x7U) << 2);        \
    ret = ret >> (((UINT32)(hwiNum) & 0x7U) << 2);          \
    ret;                                                    \
})
#endif
#define SET_SYS_PRIOR(usrPrior) ((UINT8)((UINT8)(LOSCFG_HWI_PRIO_LIMIT - 1) + RISCV_HWI_INTERRUPT_OFFSET - (usrPrior)))

STATIC INLINE UINT32 LocalInterConfigRegNumGet(UINT32 interIndex)
{
    /* each interrupt use 4 bits, then regNum = interIndex / 8 */
    return (interIndex >> 3);       /* 3: divided by 8 */
}

STATIC INLINE UINT32 SetLocalPri(const UINT32 hwiNum, const UINT8 priority)
{
    switch (LocalInterConfigRegNumGet(hwiNum)) {
        case LOCIPRI_REG0:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI0, hwiNum, priority);
            break;
        case LOCIPRI_REG1:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI1, hwiNum, priority);
            break;
        case LOCIPRI_REG2:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI2, hwiNum, priority);
            break;
        case LOCIPRI_REG3:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI3, hwiNum, priority);
            break;
        case LOCIPRI_REG4:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI4, hwiNum, priority);
            break;
        case LOCIPRI_REG5:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI5, hwiNum, priority);
            break;
        case LOCIPRI_REG6:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI6, hwiNum, priority);
            break;
        case LOCIPRI_REG7:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI7, hwiNum, priority);
            break;
        case LOCIPRI_REG8:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI8, hwiNum, priority);
            break;
        case LOCIPRI_REG9:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI9, hwiNum, priority);
            break;
        case LOCIPRI_REG10:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI10, hwiNum, priority);
            break;
        case LOCIPRI_REG11:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI11, hwiNum, priority);
            break;
        case LOCIPRI_REG12:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI12, hwiNum, priority);
            break;
        case LOCIPRI_REG13:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI13, hwiNum, priority);
            break;
        case LOCIPRI_REG14:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI14, hwiNum, priority);
            break;
        case LOCIPRI_REG15:
            SET_LOCAL_INTER_NUM_PRI(LOCIPRI15, hwiNum, priority);
            break;
        default:
            PRINT_ERR("not support yet :%u\n", (hwiNum + OS_RISCV_SYS_VECTOR_CNT));
            return LOS_ERRNO_HWI_NUM_INVALID;
    }
    return LOS_OK;
}

UINT32 HalIrqSetPrio(UINT32 hwiNum, UINT16 priority)
{
    UINT32 ret;
    UINT8 localPri;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (!HWI_PRI_VALID(priority)) {
        return LOS_ERRNO_HWI_PRIO_INVALID;
    }

    /* Only customized interrupts support priority setting. */
    if (hwiNum < OS_RISCV_SYS_VECTOR_CNT) {
        return LOS_OK;
    }

    /* riscv's priority range is [1, 7] */
    localPri = SET_SYS_PRIOR(priority);

    /* set priority of local interrupt in register 0-15 */
    hwiNum = hwiNum - OS_RISCV_SYS_VECTOR_CNT;
    ret = SetLocalPri(hwiNum, localPri);

    return ret;
}

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    (VOID)hwiNum;
    return LOS_NOK;
}

UINT32 HalCurIrqGet(VOID)
{
    return (READ_CSR(mcause)) & 0xffU; // get low 8 bits
}

CHAR *HalIrqVersion(VOID)
{
    return (CHAR *)"PLIC";
}

HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return NULL;
    }
    return &g_hwiForm[hwiNum];
}

UINT32 HalIrqMask(UINT32 hwiNum)
{
    UINT32 idx = hwiNum / OS_RIGISTER_BIT_NUM;
    const UINT32 value = (UINT32)1 << (hwiNum - (idx * OS_RISCV_LOCAL_IRQ_VECTOR_CNT));

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    /* disable interrupt of register num 0-4 */
    switch (idx) {
        case LOCIEN_REG0:
            CLEAR_CSR(mie, value);
            break;
        case LOCIEN_REG1:
            CLEAR_CUSTOM_CSR(LOCIEN0, value);
            break;
        case LOCIEN_REG2:
            CLEAR_CUSTOM_CSR(LOCIEN1, value);
            break;
        case LOCIEN_REG3:
            CLEAR_CUSTOM_CSR(LOCIEN2, value);
            break;
        case LOCIEN_REG4:
            CLEAR_CUSTOM_CSR(LOCIEN3, value);
            break;
        default:
            PRINT_ERR("Irq index error\n");
            break;
    }

    return LOS_OK;
}

UINT32 HalIrqUnmask(UINT32 hwiNum)
{
    UINT32 idx = hwiNum / OS_RIGISTER_BIT_NUM;
    const UINT32 value = (UINT32)1 << (hwiNum - (idx * OS_RISCV_LOCAL_IRQ_VECTOR_CNT));

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    /* enable interrupt of register num 0-4 */
    switch (idx) {
        case LOCIEN_REG0:
            SET_CSR(mie, value);
            break;
        case LOCIEN_REG1:
            SET_CUSTOM_CSR(LOCIEN0, value);
            break;
        case LOCIEN_REG2:
            SET_CUSTOM_CSR(LOCIEN1, value);
            break;
        case LOCIEN_REG3:
            SET_CUSTOM_CSR(LOCIEN2, value);
            break;
        case LOCIEN_REG4:
            SET_CUSTOM_CSR(LOCIEN3, value);
            break;
        default:
            PRINT_ERR("Irq index error\n");
            break;
    }
    return LOS_OK;
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    /*
     * This register is supposed to be used when other interrupt pending
     * statuses are cleared by the SW nested interrupt handler without exiting
     * the handler and re-entering the interrupt handler
     */
    WRITE_CUSTOM_CSR_VAL(LOCIPCLR, hwiNum);
    dsb();
#endif
    return LOS_OK;
}

STATIC INLINE VOID HalIrqHandlerCall(UINT32 hwiNum)
{
    if (hwiNum >= LOSCFG_PLATFORM_HWI_LIMIT) {
        return;
    }

    OsIntHandle(hwiNum, &g_hwiForm[hwiNum]);
}

#ifdef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
VOID HalIrqHandler(UINT32 hwiNum)
{
    HalIrqHandlerCall(hwiNum);
}
#else
STATIC VOID HalIrqHandler(VOID)
{
    HalIrqHandlerCall(HalCurIrqGet());
}
#endif

UINT32 HalIrqPendingGet(UINT32 hwiNum, UINT8 *isPending)
{
    if (!HWI_NUM_VALID(hwiNum) || (isPending == NULL)) {
        return LOS_NOK;
    }

    UINT32 idx = hwiNum / OS_RIGISTER_BIT_NUM;
    UINT32 mask = (UINT32)1 << (hwiNum - (idx * OS_RISCV_LOCAL_IRQ_VECTOR_CNT));
    UINT32 value;

    switch (idx) {
        case LOCIPD_REG0:
            value = READ_CSR(mip);
            break;
        case LOCIPD_REG1:
            value = READ_CUSTOM_CSR(LOCIPD0);
            break;
        case LOCIPD_REG2:
            value = READ_CUSTOM_CSR(LOCIPD1);
            break;
        case LOCIPD_REG3:
            value = READ_CUSTOM_CSR(LOCIPD2);
            break;
        case LOCIPD_REG4:
            value = READ_CUSTOM_CSR(LOCIPD3);
            break;
        default:
            return LOS_ERRNO_HWI_NUM_INVALID;
    }
    *isPending = ((value & mask) != 0) ? 1 : 0;
    return LOS_OK;
}

#ifdef LOSCFG_DEBUG_HWI
STATIC UINT32 HalIrqGetEnableStatus(UINT32 hwiNum)
{
    UINT32 idx = hwiNum / OS_RIGISTER_BIT_NUM;
    UINT32 value;

    /* enable interrupt of register num 0-4 */
    switch (idx) {
        case LOCIEN_REG0:
            value = READ_CSR(mie);
            break;
        case LOCIEN_REG1:
            value = READ_CUSTOM_CSR(LOCIEN0);
            break;
        case LOCIEN_REG2:
            value = READ_CUSTOM_CSR(LOCIEN1);
            break;
        case LOCIEN_REG3:
            value = READ_CUSTOM_CSR(LOCIEN2);
            break;
        case LOCIEN_REG4:
            value = READ_CUSTOM_CSR(LOCIEN3);
            break;
        default:
            PRINT_ERR("Irq index error\n");
            return OS_HWI_UNSUPPORTED_STATUS ;
    }
    return (value >> (hwiNum - (idx * OS_RISCV_LOCAL_IRQ_VECTOR_CNT)));
}

STATIC INLINE UINT32 GetUsrPri(UINT32 sysPrior)
{
    return (LOSCFG_HWI_PRIO_LIMIT - 1) + RISCV_HWI_INTERRUPT_OFFSET - (sysPrior);
}

STATIC UINT32 HalIrqGetPrio(UINT32 hwiNum)
{
    UINT32 regPri;
    UINT32 regIndex;

    /* Only customized interrupts support priority setting. */
    if (hwiNum < OS_RISCV_SYS_VECTOR_CNT) {
        return OS_HWI_UNSUPPORTED_STATUS;
    }

    /* set priority of local interrupt in register 0-15 */
    hwiNum = hwiNum - OS_RISCV_SYS_VECTOR_CNT;

    regIndex = LocalInterConfigRegNumGet(hwiNum);

    switch (regIndex) {  /* 3: Obtains the group ID of the interrupt. */
        case LOCIPRI_REG0:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI0, hwiNum);
            break;
        case LOCIPRI_REG1:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI1, hwiNum);
            break;
        case LOCIPRI_REG2:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI2, hwiNum);
            break;
        case LOCIPRI_REG3:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI3, hwiNum);
            break;
        case LOCIPRI_REG4:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI4, hwiNum);
            break;
        case LOCIPRI_REG5:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI5, hwiNum);
            break;
        case LOCIPRI_REG6:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI6, hwiNum);
            break;
        case LOCIPRI_REG7:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI7, hwiNum);
            break;
        case LOCIPRI_REG8:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI8, hwiNum);
            break;
        case LOCIPRI_REG9:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI9, hwiNum);
            break;
        case LOCIPRI_REG10:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI10, hwiNum);
            break;
        case LOCIPRI_REG11:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI11, hwiNum);
            break;
        case LOCIPRI_REG12:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI12, hwiNum);
            break;
        case LOCIPRI_REG13:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI13, hwiNum);
            break;
        case LOCIPRI_REG14:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI14, hwiNum);
            break;
        case LOCIPRI_REG15:
            regPri = GET_LOCAL_INTER_NUM_PRI(LOCIPRI15, hwiNum);
            break;
        default:
            PRINT_ERR("not support yet :%u\n", (hwiNum + OS_RISCV_SYS_VECTOR_CNT));
            return OS_HWI_UNSUPPORTED_STATUS;
    }

    return GetUsrPri(regPri);
}

STATIC UINT32 HalIrqStatusGet(HWI_HANDLE_T hwiNum, HwiStatus *status)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    status->pri = HalIrqGetPrio(hwiNum);
    status->enable = HalIrqGetEnableStatus(hwiNum);
    status->affinity = 0x1U;
    (VOID)HalIrqPendingGet(hwiNum, (UINT8*)&status->pending);
    return LOS_OK;
}
#endif

VOID HalIrqInit(VOID)
{
    UINT32 idx;

    for (idx = OS_RISCV_SYS_VECTOR_CNT; idx < LOSCFG_PLATFORM_HWI_LIMIT; idx++) {
        g_hwiForm[idx].hook = NULL;
        g_hwiForm[idx].shareMode = 0;
#ifdef LOSCFG_SHARED_IRQ
        g_hwiForm[idx].next = NULL;
#endif
    }

#ifndef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
    static const HwiControllerOps g_plicOps = {
        .clearIrq       = HalIrqClear,
        .enableIrq      = HalIrqUnmask,
        .disableIrq     = HalIrqMask,
        .setIrqPriority = HalIrqSetPrio,
        .getCurIrqNum   = HalCurIrqGet,
        .getIrqVersion  = HalIrqVersion,
        .getHandleForm  = HalIrqGetHandleForm,
        .handleIrq      = HalIrqHandler,
#ifdef LOSCFG_DEBUG_HWI
        .getIrqStatus   = HalIrqStatusGet,
#endif
    };
    OsHwiControllerReg(&g_plicOps);
#endif
}
