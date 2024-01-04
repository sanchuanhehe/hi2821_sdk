/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides CAN driver api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-02-22, Create file. \n
 */
#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "common_def.h"
#include "can_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_can CAN
 * @ingroup  drivers_driver
 * @{
 */

#define build_can_fd_format_standard_data_frame(id, length_code, bit_rate_switch) { \
    .std_id = (id), \
    .extend_id = 0, \
    .rtr = 0, \
    .ide = 0, \
    .fdf = 1, \
    .brs = (bit_rate_switch), \
    .dlc = (length_code), \
}

#define build_can_fd_format_extend_data_frame(id, ex_id, length_code, bit_rate_switch) { \
    .std_id = (id), \
    .extend_id = (ex_id), \
    .rtr = 0, \
    .ide = 1, \
    .fdf = 1, \
    .brs = (bit_rate_switch), \
    .dlc = (length_code), \
}
#define build_can_standard_data_frame(id, length_code) { \
    .std_id = (id), \
    .extend_id = 0, \
    .rtr = 0, \
    .ide = 0, \
    .fdf = 0, \
    .brs = 0, \
    .dlc = (length_code), \
}

#define build_can_extend_data_frame(id, ex_id, length_code) { \
    .std_id = (id), \
    .extend_id = (ex_id), \
    .rtr = 0, \
    .ide = 1, \
    .fdf = 0, \
    .brs = 0, \
    .dlc = (length_code), \
}

#define build_can_standard_remote_frame(id) { \
    .std_id = (id), \
    .extend_id = 0, \
    .rtr = 1, \
    .ide = 0, \
    .fdf = 0, \
    .brs = 0, \
    .dlc = BYTE_0, \
}

#define build_can_extend_remote_frame(id, ex_id) { \
    .std_id = (id), \
    .extend_id = (ex_id), \
    .rtr = 1, \
    .ide = 1, \
    .fdf = 0, \
    .brs = 0, \
    .dlc = BYTE_0, \
}
/**
 * @if Eng
 * @brief  CAN opreation mode.
 * @else
 * @brief  CAN 工作模式。
 * @endif
 */
typedef enum can_mode {
    NORMAL_OPREATION = 0x0,
    LOOP_BACK_MODE = 0x1
} can_mode_t;

/**
 * @if Eng
 * @brief  CAN filter type.
 * @else
 * @brief  CAN 滤波器格式。
 * @endif
 */
typedef enum filter_type {
    RANGE_FILTER = 0x0,
    DUAL_ID_FILTER,
    CLASSIC_FILTER = 0x3
} filter_type_t;

/**
 * @if Eng
 * @brief  CAN Data length code.
 * @else
 * @brief  CAN数据长度码。
 * @endif
 */
typedef enum data_length_code {
    BYTE_0 = 0x0,
    BYTE_1,
    BYTE_2,
    BYTE_3,
    BYTE_4,
    BYTE_5,
    BYTE_6,
    BYTE_7,
    BYTE_8,
    BYTE_12,
    BYTE_16,
    BYTE_20,
    BYTE_24,
    BYTE_32,
    BYTE_48,
    BYTE_64 = 0xF,
} data_length_code_t;

/**
 * @if Eng
 * @brief  CAN controller status.
 * @else
 * @brief  CAN 控制器状态。
 * @endif
 */
typedef enum can_sts {
    ERROR_ACTIVE,
    ERROR_PASSIVE,
    BUS_OFF
} can_sts_t;

/**
 * @if Eng
 * @brief  Nominal and Data bit time config.
 * @else
 * @brief  配置标称位时间和数据段位时间。
 * @endif
 */
typedef struct can_global_config {
    can_mode_t mode;                   /*!< @if Eng CAN opreation mode.
                                            @else   CAN工作模式。 @endif */
    uint32_t ntseg1;                   /*!< @if Eng Nominal time segment before sample point.
                                            @else   CAN-FD仲裁段采样点前的时间段 @endif */
    uint32_t ntseg2;                   /*!< @if Eng Nominal time segment after sample point.
                                            @else   CAN-FD仲裁段采样点后的时间段 @endif */
    uint32_t nsjw;                     /*!< @if Eng Nominal resyncronization jump width.
                                            @else   CAN-FD仲裁段重同步伸缩宽度 @endif */
    uint32_t nbrp;                     /*!< @if Eng Nominal bit rate prescaler.
                                            @else   CAN-FD仲裁段分频系数 @endif */
#if defined(CONFIG_CAN_FD_MODE_ENABLE)
    uint32_t dtseg1;                   /*!< @if Eng Data time segment after sample point.
                                            @else   CAN-FD数据段采样点后的时间段 @endif */
    uint32_t dtseg2;                   /*!< @if Eng Data time segment after sample point.
                                            @else   CAN-FD数据段采样点后的时间段 @endif */
    uint32_t dsjw;                     /*!< @if Eng Data resyncronization jump width.
                                            @else   CAN-FD数据段重同步伸缩宽度 @endif */
    uint32_t dbrp;                     /*!< @if Eng Data bit rate prescaler.
                                            @else   CAN-FD数据段分频系数 @endif */
#endif
} can_global_config_t;

/**
 * @if Eng
 * @brief  CAN filter structure.
 * @else
 * @brief  CAN滤波结构。
 * @endif
 */
typedef struct can_filter {
    uint32_t fid1;                     /*!< @if Eng First ID of the filter.
                                            @else   滤波器的首要ID @endif */
    uint32_t fid2;                     /*!< @if Eng Second ID of the filter, which can be used as a mask,
                                                    secondary identifier, or right boundary.
                                            @else   滤波器的次要ID，可作为掩码、次要标识符或右边界。 @endif */
    filter_type_t filter_type;         /*!< @if Eng Filter mode.
                                            @else   滤波器模式 @endif */
    uint32_t fifo_id;                  /*!< @if Eng Index of the Rx-FIFO bound to the filter.
                                            @else   滤波器绑定的Rx-FIFO索引。 @endif */
} can_filter_t;

/**
 * @if Eng
 * @brief  Structure of the CAN frame Arbitration field.
 * @else
 * @brief  CAN帧仲裁域结构。
 * @endif
 */
typedef struct can_msg_attr {
    uint32_t std_id;                   /*!< @if Eng Standard identifier.
                                            @else   标准标识符。 @endif */
    uint32_t extend_id;                /*!< @if Eng Extra identifier.
                                            @else   扩展标识符。 @endif */
    uint8_t rtr;                       /*!< @if Eng Remote frame flag.
                                            @else   远程帧标志。 @endif */
    uint8_t ide;                       /*!< @if Eng Extended format flag.
                                            @else   扩展格式标志。 @endif */
#if defined(CONFIG_CAN_FD_MODE_ENABLE)
    uint8_t fdf;                       /*!< @if Eng FD format flag.
                                            @else   FD帧格式标志。 @endif */
    uint8_t brs;                       /*!< @if Eng Bit rate switch.
                                            @else   波特率切换标志。 @endif */
#endif
    data_length_code_t dlc;            /*!< @if Eng Data length code.
                                            @else   数据长度码。 @endif */
} can_msg_attr_t;

/**
 * @if Eng
 * @brief  Receive interrupt callback function.
 * @param  [in]  received_msg Arbitration field of the received CAN frame.
 * @param  [in]  data Received data.
 * @param  [in]  length Received data length.
 * @else
 * @brief  接收中断回调函数。
 * @param  [in]  received_msg 接收到的CAN帧仲裁域。
 * @param  [in]  data 接收到的数据。
 * @param  [in]  length 接收到的数据长度。
 * @endif
 */
typedef void (*rx_callback_t)(can_msg_attr_t received_msg, uint8_t *data, uint32_t length);

/**
 * @if Eng
 * @brief  Initialize and Configure CAN-FD.
 * @param  [in]  global_config CAN-FD global configuration, including bit time and operating mode configuration.
 * @param  [in]  std_filter Filtering configuration of the CAN-FD standard identifier.
 * @param  [in]  std_filter_num Number of the standard ID filter element.
 * @param  [in]  ex_filter Filtering configuration of the CAN-FD extended identifier.
 * @param  [in]  ex_filter_num Number of the extended ID filter element.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化并配置CAN-FD。
 * @param  [in]  global_config CAN-FD全局配置，包含位时间和工作模式配置。
 * @param  [in]  std_filter CAN-FD 标准标识符滤波配置。
 * @param  [in]  std_filter_num CAN-FD 标准滤波配置数。
 * @param  [in]  ex_filter CAN-FD 扩展标识符滤波配置。
 * @param  [in]  ex_filter_num CAN-FD 扩展滤波配置数。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_can_init(can_global_config_t global_config, can_filter_t *std_filter, uint32_t std_filter_num,
                        can_filter_t *ex_filter, uint32_t ex_filter_num);

/**
 * @if Eng
 * @brief  Deinitialize CAN-FD.
 * @else
 * @brief  去初始化CAN-FD。
 * @endif
 */
void uapi_can_deinit(void);

/**
 * @if Eng
 * @brief  CAN-FD request to send frames.
 * @param  [in]  can_msg Expected attribute field CAN data frames or remote frames to be transmitted.
 * @param  [in]  data Expected data to be sent.
 * @param  [in]  length Data length.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  CAN-FD 请求发送报文。
 * @param  [in]  can_msg 预计发送的CAN数据帧和远程帧的仲裁域。
 * @param  [in]  data 预计发送的数据。
 * @param  [in]  length 数据长度。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_can_send_message(can_msg_attr_t can_msg, uint8_t *data, uint32_t length);

/**
 * @if Eng
 * @brief  CAN-FD read Rx-FIFO for CAN frames.
 * @param  [in]  fifo_index Expected fifo index to be transmitted.
 * @param  [out] receive_msg Arbitration field CAN data frames or remote frames to be received.
 * @param  [out] data Buffer for receiving data
 * @param  [out] length Received data length.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  CAN-FD 从Rx-FIFO中读取CAN帧。
 * @param  [in]  fifo_index 接收fifo的索引。
 * @param  [out] receive_msg 预计接收到的CAN帧仲裁域。
 * @param  [out] data 接受数据的缓冲区。
 * @param  [out] length 接收到的数据真实长度。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_can_receive_msg(uint32_t fifo_index,
                               can_msg_attr_t *receive_msg, uint8_t *data, uint32_t *length);

/**
 * @if Eng
 * @brief  Register the callback function for receiving interrupts.
 * @param  [in]  fifo_index Index of the Rx-FIFO bound to the callback function
 * @param  [in]  rx_callback Rx interrupt callback function.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  接收中断回调函数注册。
 * @param  [in]  fifo_index 回调函数绑定的fifo索引。
 * @param  [in]  rx_callback 接收中断回调函数。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_can_rx_callback_register(uint32_t fifo_index, rx_callback_t rx_callback);

/**
 * @if Eng
 * @brief  Unregister the callback function for receiving interrupts.
 * @param  [in]  fifo_index Index of the Rx-FIFO bound to the callback function
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  接收中断回调函数解注册。
 * @param  [in]  fifo_index 回调函数绑定的fifo索引。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_can_rx_callback_unregister(uint32_t fifo_index);

/**
 * @if Eng
 * @brief  Query the error status of the CAN controller.
 * @retval For details, see @ref can_sts_t.
 * @else
 * @brief  查询CAN控制器错误状态。
 * @retval 参考 @ref can_sts_t 。
 * @endif
 */
can_sts_t uapi_can_get_controller_sts(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif