/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT message source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-02， Create file. \n
 */

#include "at_base.h"
#include "at_channel.h"
#include "at_process.h"
#include "at_msg.h"

static unsigned long g_at_msg_queue;
#define AT_NO_WAIT 0x0
#define AT_WAIT_FOREVER 0xFFFFFFFF

void at_msg_process(at_msg_block_t *msg)
{
    switch (msg->type) {
        case AT_CMD_MSG:
            at_proc_cmd_handle(msg->sub_msg.cmd.channel_id);
            break;
        case AT_CMD_RESULT_MSG:

            break;
        case AT_CMD_URC_REPORT_MSG:

            break;
        case AT_CMD_TIMEOUT_MSG:

            break;
        case AT_CMD_INTERACTIVITY_MSG:

            break;
        default:
            break;
    }
}

errcode_t at_msg_send(at_msg_block_t *msg)
{
    if (at_msg_queue_write(g_at_msg_queue, msg, sizeof(at_msg_block_t), AT_NO_WAIT) != 0) {
        return ERRCODE_AT_MSG_SEND_ERROR;
    }
    return ERRCODE_SUCC;
}

void uapi_at_msg_main(void *unused)
{
    unused(unused);
    at_msg_block_t msg;
    uint32_t buffer_size = sizeof(at_msg_block_t);

    at_msg_queue_create(AT_MSG_MAX_NUM, sizeof(at_msg_block_t), &g_at_msg_queue);
    at_channel_check_and_enable();

    for (;;) {
        if (at_msg_queue_read(g_at_msg_queue, &msg, &buffer_size, AT_WAIT_FOREVER) == 0) {
            at_msg_process(&msg);
        }
        (void)at_yield();
#ifdef UT_TEST
        break;
#endif
    }
}
