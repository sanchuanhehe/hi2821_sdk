/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR NEC header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */
#ifndef IR_NEC_H
#define IR_NEC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void ir_transmit_nec(uint8_t user_code_h, uint8_t data_code);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif