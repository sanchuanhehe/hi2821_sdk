/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Usb Irq Ref File
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

#include "implementation/_intr_ref.h"
#include <los_hwi.h>

#define BUS_IRQ_DEFAULT_PRIORITY  0x0

int
bus_request_intr(unsigned int irq, int flags, irq_handler handler, void *arg)
{
	uint32_t ret;
	HWI_IRQ_PARAM_S irqParam;

	if (OS_INT_ACTIVE)
		return ((int)LOS_ERRNO_HWI_INTERR);

	irqParam.swIrq = (int)irq;
	irqParam.pDevId = arg;
	irqParam.pName = NULL;

	ret = LOS_HwiCreate(irq, BUS_IRQ_DEFAULT_PRIORITY, (HWI_MODE_T)flags, (HWI_PROC_FUNC)handler, &irqParam);
	if (ret == LOS_OK) {
		(void)HalIrqUnmask(irq);
	}
	return ((int)ret);
}

int
bus_release_intr(unsigned int irq, void *dev_id)
{
	HWI_IRQ_PARAM_S irqParam;

	if (OS_INT_ACTIVE)
		return (-1);

	irqParam.swIrq = (int)irq;
	irqParam.pDevId = dev_id;

	(void)LOS_HwiDelete(irq, &irqParam);
	return (0);
}
