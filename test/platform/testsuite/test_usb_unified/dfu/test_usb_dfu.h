/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test dfu header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-13, Create file. \n
 */
#ifndef TEST_DFU_H
#define TEST_DFU_H

int usb_dfu_init(void);
void usb_dfu_wait_ugrade_done_and_reset(void);

#endif
