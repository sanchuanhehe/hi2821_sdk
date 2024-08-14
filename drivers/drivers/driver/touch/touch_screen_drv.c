/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: touch screen driver.
 * Author: @CompanyNameTag
 * Create: 2022-04-12
 */

#include "touch_screen_drv.h"
#include "tcxo.h"
#include "common_def.h"
#include "soc_osal.h"
#include "touch_screen_def.h"
#include "input_app.h"
#if defined(TPTYPE_TMA525B)
#include "tma525b_drv1.h"
#elif defined(TPTYPE_ZTW523)
#include "ztw523_drv1.h"
#else
#include "ztw523_drv1.h"
#endif

#define TOUCH_SCREEN_DEV_NAME_LEN 16
#define TOUCH_SCREEN_DEVNAME "input/event"
#define TOUCH_SCREEN_DYNAMIC_MINOR 65
#define TOUCH_SCREEN_CMDID_MAX 16

#define TOUCH_SCREEN_TASK_PRIO      5
#define TOUCH_SCREEN_TASK_SIZE      0x800
#define TOUCH_SCREEN_QUEUE_MAX_SIZE 60
#define TOUCH_SCREEN_READY_EVENT    (1 << 0)

struct ts_driver_data g_touch_drv = {0};
struct ts_driver_data *g_attr = NULL;

report_event_cb g_report_event_cb = NULL;

#if defined(SUPPORT_POWER_MANAGER)
event_callback_func g_power_manager_cb = NULL;
#endif

static struct ts_driver_data *touch_screen_get_driver_data(void)
{
    return &g_touch_drv;
}

static uint32_t touch_screen_get_state(struct ts_driver_data *tsdd)
{
    if (tsdd != NULL && tsdd->queue != NULL) {
        return (uint32_t)ts_queue_is_empty(tsdd->queue);
    }

    if (tsdd == NULL) {
        tp_err("tsdd is null!");
        return EXT_ERR_FAILURE;
    }
    if (tsdd->queue == NULL) {
        tp_err("tsdd queue is null!");
    }

    return EXT_ERR_FAILURE;
}

static void convert_touch_info(input_dev_data_t* data, input_event_info* touch_info)
{
    data->x = (int16_t)touch_info->tp_event_info.x_axis[0];
    data->y = (int16_t)touch_info->tp_event_info.y_axis[0];
    data->type = INDEV_TYPE_TOUCH;
    data->timestamp = (uint32_t)touch_info->tv_usec;
    if ((touch_info->tp_event == MC_TP_PRESS) || (touch_info->tp_event == MC_TP_MOVE)) {
        data->state = INDEV_STATE_PRESS;
    } else if (touch_info->tp_event == MC_TP_RELEASE) {
        data->state = INDEV_STATE_RELEASE;
    } else {
        data->state = INDEV_STATE_UNKNOWN;
    }
}

static void report_or_enqueue_event(input_event_info* touch_info, struct ts_driver_data *tsdd)
{
    if (g_report_event_cb != NULL) {
        input_dev_data_t data;
        convert_touch_info(&data, touch_info);
        if (data.state == INDEV_STATE_UNKNOWN) {
            return;
        }
        g_report_event_cb(&data);
    } else {
        int32_t ret = ts_en_queue(tsdd->queue, touch_info);
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("Failed to enqueue ts queue! ret=0x%x", ret);
        }
    }
}
#if defined(SUPPORT_POWER_MANAGER)
static void report_event_to_power_manager(input_event_info *touch_info)
{
    if (g_power_manager_cb != NULL) {
        input_dev_data_t data;
        convert_touch_info(&data, touch_info);
        if (data.state == INDEV_STATE_UNKNOWN) {
            return;
        }
        g_power_manager_cb(data.state, 0);
    }
    return;
}

uint32_t ts_register_power_manager_cb(event_callback_func callback)
{
    if (g_power_manager_cb != NULL) {
        return EXT_ERR_FAILURE;
    }
    g_power_manager_cb = callback;
    return EXT_ERR_SUCCESS;
}
#endif
ext_errno touch_info_process(void)
{
    struct ts_driver_data *tsdd = touch_screen_get_driver_data();
    input_event_info touch_info;
    int32_t ret = 0;

    if (tsdd->touch_api->touch_get_tpinfo != NULL) {
        ret = tsdd->touch_api->touch_get_tpinfo((uint8_t *)(&touch_info), sizeof(input_event_info));
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("get touch info fail! ret=0x%x", ret);
        }

        report_or_enqueue_event(&touch_info, tsdd);
#if defined(SUPPORT_POWER_MANAGER)
        report_event_to_power_manager(&touch_info);
#endif
    }

    return EXT_ERR_SUCCESS;
}

static void ts_callback_func(void)
{
    ext_errno ret;
    if (g_attr != NULL) {
        common_notify_mail_t mail = {0};
        mail.comm.app = APP_TOUCH;
        ret = osal_msg_queue_write_copy(get_app_queue_id(), (void *)&mail,
                                        sizeof(common_notify_mail_t), 0);
        if (ret != OSAL_SUCCESS) {
            tp_err("touch app msg send failed! err=0x%x! \r\n", ret);
        }
    }
}

static ext_errno ts_register_callback(struct ts_driver_data *attr)
{
    ext_errno ret = EXT_ERR_FAILURE;
    g_attr = attr;

    if (attr->touch_api->register_callback != NULL) {
        ret = attr->touch_api->register_callback(attr->private_data, ts_callback_func);
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("register touch screen callback fail! ret=0x%x", ret);
            return ret;
        }
    } else {
        tp_err("touch screen callback register func is empty!");
    }

    return ret;
}

static ext_errno ts_unregister_callback(struct ts_driver_data *attr)
{
    ext_errno ret = EXT_ERR_FAILURE;

    if (attr->touch_api->unregister_callback != NULL) {
        ret = attr->touch_api->unregister_callback(attr->private_data);
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("unregister touch screen callback fail! ret=0x%x", ret);
            return ret;
        }
        g_attr = NULL;
    } else {
        tp_err("touch screen callback unregister func is empty!");
    }

    return ret;
}

static int32_t ts_dev_open(const void *private_data)
{
    int32_t ret;
    struct ts_driver_data *tsdd = NULL;
    if (private_data == NULL) {
        tp_err("ts_open: invalid dev or driver_data!\n");
        return EXT_ERR_FAILURE;
    }

    // todo: get tsdd from private_data
    tsdd = touch_screen_get_driver_data();
    tsdd->queue = ts_init_queue(TOUCH_SCREEN_QUEUE_MAX_SIZE);
    if (tsdd->queue == NULL) {
        tp_err("create touch screen queue fail!\r\n");
        return EXT_ERR_FAILURE;
    }
    ret = osal_event_init(&(tsdd->event));
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("touch osal_event_init fail! ret=0x%x\r\n", ret);
        return ret;
    }

    tsdd->task_handle = osal_kthread_create((osal_kthread_handler)touch_info_process,
                                            (void *)touch_screen_get_driver_data(),
                                            "touch_screen", TOUCH_SCREEN_TASK_SIZE);
    if (tsdd->task_handle == NULL) {
        tp_err("create touch screen thread fail!\r\n");
        return EXT_ERR_FAILURE;
    }
    osal_kthread_set_priority(tsdd->task_handle, TOUCH_SCREEN_TASK_PRIO);

    if (tsdd->touch_api->touch_init != NULL) {
        ret = tsdd->touch_api->touch_init(tsdd->private_data);
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("touch_init fail! ret=0x%x\r\n", ret);
            return ret;
        }
    }

    ret = ts_register_callback(tsdd);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("register callback fail! ret=0x%x\r\n", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t ts_dev_release(const void *private_data)
{
    int32_t ret;
    struct ts_driver_data *tsdd = NULL;
    if (private_data == NULL) {
        tp_err("ts_dev_release: invalid dev or driver_data!\n");
        return EXT_ERR_FAILURE;
    }

    // todo: get tsdd from private_data
    tsdd = touch_screen_get_driver_data();
    ret = ts_unregister_callback(tsdd);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("unregister callback fail! ret=0x%x\r\n", ret);
        return ret;
    }
    return EXT_ERR_SUCCESS;
}

static uint32_t ts_dev_poll(const osal_poll *osal_poll, const void *private_data)
{
    unused(osal_poll);
    unused(private_data);
    // todo: get tsdd from private_data
    struct ts_driver_data *tsdd = NULL;
    tsdd = touch_screen_get_driver_data();
    if (tsdd == NULL) {
        tp_err("ts_driver_data is NULL");
        return EXT_ERR_FAILURE;
    }

    if (tsdd->ops->priv_operator != NULL && tsdd->ops->priv_operator(tsdd) == 0) {
        return OSAL_POLLIN | OSAL_POLLRDNORM;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t ts_ioctl_read_touch_info(uint32_t cmd, void *arg, const void *private_data)
{
    unused(cmd);
    ext_errno ret;
    struct ts_driver_data *tsdd = NULL;
    input_event_info *touch_info = (input_event_info *)arg;

    if (private_data == NULL) {
        tp_err("invalid dev or driver_data!");
        return EXT_ERR_FAILURE;
    }

    // todo: get tsdd from private_data
    tsdd = touch_screen_get_driver_data();
    if (tsdd->queue == NULL) {
        tp_err("invalid touch screen queue!");
        return EXT_ERR_FAILURE;
    }

    ret = ts_de_queue(tsdd->queue, touch_info);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("get touch info fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

static osal_ioctl_cmd g_ts_cmd_list[TOUCH_SCREEN_CMDID_MAX] = {
    {TOUCH_SCREEN_GET_EVENT, ts_ioctl_read_touch_info},
};

static osal_dev *g_ts_dev = NULL;
static osal_fileops g_ts_ops = {
    .open = ts_dev_open,
    .release = ts_dev_release,
    .poll = ts_dev_poll,
    .cmd_list = g_ts_cmd_list,
    .cmd_cnt = sizeof(g_ts_cmd_list) / sizeof(g_ts_cmd_list[0]),
};

struct ts_ops g_ts_status_ops = {
    .priv_operator  = touch_screen_get_state,
};

static ext_errno touch_screen_register_device(void)
{
    ext_errno ret;

    g_touch_drv.private_data = touch_screen_get_peri_attr();
    g_touch_drv.touch_api = touch_screen_get_api();
    g_touch_drv.ops = &g_ts_status_ops;
    g_ts_dev = osal_dev_create(TOUCH_SCREEN_DEVNAME);
    g_ts_dev->minor = TOUCH_SCREEN_DYNAMIC_MINOR;
    g_ts_dev->fops = &g_ts_ops;

    ret = osal_dev_register(g_ts_dev);
    if (ret != 0) {
        tp_err("touch screen osal_dev_register --------failed! ret=0x%x\n", ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno touch_screen_module_init(void)
{
    ext_errno ret;

    ret = touch_screen_register_device();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("touch_screen_register_device ----------failed! ret=0x%x\n", ret);
    }
    tp_err("touch_screen_register_device ----------succ!!!\n");

    return EXT_ERR_SUCCESS;
}

ext_errno touch_screen_module_deinit(void)
{
    return EXT_ERR_SUCCESS;
}

uint32_t ts_register_report_event_cb(report_event_cb callback)
{
    if (g_report_event_cb != NULL) {
        return EXT_ERR_FAILURE;
    }
    g_report_event_cb = callback;
    return EXT_ERR_SUCCESS;
}

uint32_t ts_unregister_report_event_cb(void)
{
    g_report_event_cb = NULL;
    return EXT_ERR_SUCCESS;
}

uint32_t ts_init(void)
{
    int32_t ret;
    struct ts_driver_data *tsdd = NULL;

    g_touch_drv.private_data = touch_screen_get_peri_attr();
    g_touch_drv.touch_api = touch_screen_get_api();
    g_touch_drv.ops = &g_ts_status_ops;
    tsdd = touch_screen_get_driver_data();
    tsdd->queue = ts_init_queue(TOUCH_SCREEN_QUEUE_MAX_SIZE);
    if (tsdd->queue == NULL) {
        tp_err("create touch screen queue fail!\r\n");
        return EXT_ERR_FAILURE;
    }

    ret = ts_register_callback(tsdd);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("register callback fail! ret=0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

uint32_t ts_read(const input_dev_data_t* data)
{
    unused(data);
    return EXT_ERR_SUCCESS;
}

uint32_t ts_close(void)
{
    return EXT_ERR_SUCCESS;
}
