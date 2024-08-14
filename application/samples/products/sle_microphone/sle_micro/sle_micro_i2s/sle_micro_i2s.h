/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO sample of I2s. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-12, Create file. \n
 */
#ifndef SLE_MICRO_I2S_H
#define SLE_MICRO_I2S_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MASTER 1
#define SLAVE 0

typedef enum {
    STD_MODE = 0,                               /*!< @if Eng Standard mode.
                                                     @else   标准模式。 @endif */
    MULTICHANNEL_MODE                           /*!< @if Eng Multichannel mode.
                                                     @else   多路模式。 @endif */
} hal_sio_transfer_mode_t;

typedef enum hal_sio_data_width {
    RESERVED,                                   /*!< @if Eng RESERVED.
                                                     @else   保留。 @endif */
    SIXTEEN_BIT,                                /*!< @if Eng 16 BITS.
                                                     @else   16位。 @endif */
    EIGHTEEN_BIT,                               /*!< @if Eng 18 BITS.
                                                     @else   18位。 @endif */
    TWENTY_BIT,                                 /*!< @if Eng 20 BITS.
                                                     @else   20位。 @endif */
    TWENTY_FOUR_BIT,                            /*!< @if Eng 24 BITS.
                                                     @else   24位。 @endif */
    THIRTY_TWO_BIT                              /*!< @if Eng 32 BITS.
                                                     @else   32位。 @endif */
} hal_sio_data_width_t;

typedef enum hal_sio_channel_number {
    TWO_CH,                                     /*!< @if Eng 2 Channels.
                                                     @else   2通道。 @endif */
    FOUR_CH,                                    /*!< @if Eng 4 Channels.
                                                     @else   4通道。 @endif */
    EIGHT_CH,                                   /*!< @if Eng 8 Channels.
                                                     @else   8通道。 @endif */
    SIXTEEN_CH                                  /*!< @if Eng 16 Channels.
                                                     @else   16通道。 @endif */
} hal_sio_channel_number_t;

typedef enum hal_sio_voice_channel {
    SIO_LEFT = 0,                               /*!< @if Eng SIO left voice channel.
                                                     @else   SIO 左声道。 @endif */
    SIO_RIGHT,                                  /*!< @if Eng SIO right voice channel.
                                                     @else   SIO 右声道。 @endif */
    NONE_MODE
} hal_sio_voice_channel_t;

typedef enum hal_sio_clk_edge {
    FALLING_EDGE = 0,                           /*!< @if Eng Falling edge.
                                                     @else   下降沿。 @endif */
    RISING_EDGE,                                /*!< @if Eng Rising edge
                                                     @else   上升沿。 @endif */
    NONE_EDGE
} hal_sio_clk_edge_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif