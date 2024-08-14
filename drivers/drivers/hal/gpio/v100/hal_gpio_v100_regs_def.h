/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 gpio register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */
#ifndef HAL_GPIO_V100_REGS_H
#define HAL_GPIO_V100_REGS_H

#include <stdint.h>
#include "gpio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v100_regs_def GPIO V100 Regs Definition
 * @ingroup  drivers_hal_gpio
 * @{
 */

/**
 * @brief  This union represents the bit fields in the Port A Data Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swporta_dr_data {
    uint32_t d32;                                             /*!< Raw register data. */
    struct {
        uint32_t port_a_data         : CONFIG_GPIO_PWIDTH_A;  /*!< Values written to this register are output on the
                                                                   I/O signals for Port A if the corresponding data
                                                                   direction bits for Port A are set to Output mode
                                                                   and the corresponding control bit for Port A is
                                                                   set to Software mode. The value read back is equal
                                                                   to the last value written to this register. */
    } b;                                                      /*!< Register bits. */
} gpio_swporta_dr_data_t;

/**
 * @brief  This union represents the bit fields in the Port A Data Direction Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swporta_ddr_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t port_a_data_direction  : CONFIG_GPIO_PWIDTH_A; /*!< Values written to this register independently
                                                                     control the direction of the corresponding data bit
                                                                     in Port A. The default direction can be configured
                                                                     as input or output after system reset through the
                                                                     GPIO_DFLT_DIR_A parameter. */
    } b;                                                        /*!< Register bits. */
} gpio_swporta_ddr_data_t;

/**
 * @brief  This union represents the bit fields in the Port A Data Source Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swporta_ctl_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t synchronization_level      : 1;    /*!< Writing a 1 to this register results in all level-sensitive
                                                         interrupts being synchronized to pclk_intr. */
    } b;                                            /*!< Register bits. */
} gpio_swporta_ctl_data_t;

/**
 * @brief  This union represents the bit fields in the Port B Data Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportb_dr_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t port_b_data         : CONFIG_GPIO_PWIDTH_B;    /*!< Values written to this register are output on the
                                                                     I/O signals for Port B if the corresponding data
                                                                     direction bits for Port B are set to Output mode
                                                                     and the corresponding control bit for Port B is set
                                                                     to Software mode. The value read back is equal to
                                                                     the last value written to this register. */
    } b;                                                        /*!< Register bits. */
} gpio_swportb_dr_data_t;

/**
 * @brief  This union represents the bit fields in the Port B Data Direction Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportb_ddr_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t port_b_data_direction  : CONFIG_GPIO_PWIDTH_B; /*!< Values written to this register independently
                                                                     control the direction of the corresponding data bit
                                                                     in Port B. The default direction can be configured
                                                                     as input or output after system reset through the
                                                                     GPIO_DFLT_DIR_B parameter. */
    } b;                                                        /*!< Register bits. */
} gpio_swportb_ddr_data_t;

/**
 * @brief  This union represents the bit fields in the Port B Data Source Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportb_ctl_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t synchronization_level      : 1;    /*!< Writing a 1 to this register results in all level-sensitive
                                                         interrupts being synchronized to pclk_intr. */
    } b;                                            /*!< Register bits. */
} gpio_swportb_ctl_data_t;

/**
 * @brief  This union represents the bit fields in the Port C Data Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportc_dr_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t port_c_data         : CONFIG_GPIO_PWIDTH_C;    /*!< Values written to this register are output on the
                                                                     I/O signals for Port C if the corresponding data
                                                                     direction bits for Port C are set to Output mode
                                                                     and the corresponding control bit for Port C is set
                                                                     to Software mode. The value read back is equal to
                                                                     the last value written to this register. */
    } b;                                                        /*!< Register bits. */
} gpio_swportc_dr_data_t;

/**
 * @brief  This union represents the bit fields in the Port C Data Direction Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportc_ddr_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t port_c_data_direction : CONFIG_GPIO_PWIDTH_C;  /*!< Values written to this register independently
                                                                     control the direction of the corresponding data bit
                                                                     in Port C. The default direction can be configured
                                                                     as input or output after system reset through the
                                                                     GPIO_DFLT_DIR_C parameter. */
    } b;                                                        /*!< Register bits. */
} gpio_swportc_ddr_data_t;

/**
 * @brief  This union represents the bit fields in the Port C Data Source Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportc_ctl_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t synchronization_level      : 1;    /*!< Writing a 1 to this register results in all level-sensitive
                                                         interrupts being synchronized to pclk_intr. */
    } b;                                            /*!< Register bits. */
} gpio_swportc_ctl_data_t;

/**
 * @brief  This union represents the bit fields in the Port D Data Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportd_dr_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t port_d_data         : CONFIG_GPIO_PWIDTH_D;    /*!< Values written to this register are output on the
                                                                     I/O signals for Port D if the corresponding data
                                                                     direction bits for Port D are set to Output mode
                                                                     and the corresponding control bit for Port D is
                                                                     set to Software mode. The value read back is equal
                                                                     to the last value written to this register. */
    } b;                                                        /*!< Register bits. */
} gpio_swportd_dr_data_t;

/**
 * @brief  This union represents the bit fields in the Port D Data Direction Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportd_ddr_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t port_d_data_direction : CONFIG_GPIO_PWIDTH_D;  /*!< Values written to this register independently
                                                                     control the direction of the corresponding data
                                                                     bit in Port D. The default direction can be
                                                                     configured as input or output after system reset
                                                                     through the GPIO_DFLT_DIR_D parameter. */
    } b;                                                        /*!< Register bits. */
} gpio_swportd_ddr_data_t;

/**
 * @brief  This union represents the bit fields in the Port D Data Source Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_swportd_ctl_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t synchronization_level      : 1;    /*!< Writing a 1 to this register results in all level-sensitive
                                                         interrupts being synchronized to pclk_intr. */
    } b;                                            /*!< Register bits. */
} gpio_swportd_ctl_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt enable Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_inten_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t interrupt_enable    : CONFIG_GPIO_PWIDTH_A;    /*!< Allows each bit of Port A to be configured for
                                                                     interrupts. By default the generation of interrupts
                                                                     is disabled. Whenever a 1 is written to a bit of
                                                                     this register, it configures the corresponding bit
                                                                     on Port A to become an interrupt; otherwise, Port
                                                                     A operates as a normal GPIO signal. Interrupts are
                                                                     disabled on the corresponding bits of Port A if
                                                                     the corresponding data direction register is set
                                                                     to Output or if Port A mode is set to Hardware. */
    } b;                                                        /*!< Register bits. */
} gpio_inten_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt mask Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_intmask_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t interrupt_mask      : CONFIG_GPIO_PWIDTH_A;    /*!< Controls whether an interrupt on Port A can create
                                                                     an interrupt for the interrupt controller by not
                                                                     masking it. By default, all interrupts bits are
                                                                     unmasked. Whenever a 1 is written to a bit in this
                                                                     register, it masks the interrupt generation
                                                                     capability for this signal; otherwise interrupts
                                                                     are allowed through. The unmasked status can be
                                                                     read as well as the resultant status after
                                                                     masking. */
    } b;                                                        /*!< Register bits. */
} gpio_intmask_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt level Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_inttype_level_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t interrupt_level     : CONFIG_GPIO_PWIDTH_A;    /*!< Controls the type of interrupt that can occur on
                                                                     Port A. Whenever a 0 is written to a bit of this
                                                                     register, it configures the interrupt type to be
                                                                     level-sensitive; otherwise, it is
                                                                     edge-sensitive. */
    } b;                                                        /*!< Register bits. */
} gpio_inttype_level_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt polarity Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_int_polarity_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t interrupt_polarity  : CONFIG_GPIO_PWIDTH_A;    /*!< Controls the polarity of edge or level sensitivity
                                                                     that can occur on input of Port A. Whenever a 0 is
                                                                     written to a bit of this register, it configures
                                                                     the interrupt type to falling-edge or active-low
                                                                     sensitive; otherwise, it is rising-edge or
                                                                     active-high sensitive. */
    } b;                                                        /*!< Register bits. */
} gpio_int_polarity_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt status Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_intstatus_data {
    uint32_t d32;                                                      /*!< Raw register data. */
    struct {
        uint32_t interrupt_status           : CONFIG_GPIO_PWIDTH_A;    /*!< Interrupt status of Port A. */
    } b;                                                               /*!< Register bits. */
} gpio_intstatus_data_t;

/**
 * @brief  This union represents the bit fields in the Raw interrupt status Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_raw_intstatus_data {
    uint32_t d32;                                                      /*!< Raw register data. */
    struct {
        uint32_t raw_interrupt_status       : CONFIG_GPIO_PWIDTH_A;    /*!< Raw interrupt of status of Port A
                                                                            (premasking bits). */
    } b;                                                               /*!< Register bits. */
} gpio_raw_intstatus_data_t;

/**
 * @brief  This union represents the bit fields in the Debounce enable Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_debounce_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t debounce_enable     : CONFIG_GPIO_PWIDTH_A;    /*!< Controls whether an external signal that is the
                                                                     source of an interrupt needs to be debounced to
                                                                     remove any spurious glitches. Writing a 1 to a bit
                                                                     in this register enables the debouncing circuitry.
                                                                     A signal must be valid for two periods of an
                                                                     external clock before it is internally
                                                                     processed. */
    } b;                                                        /*!< Register bits. */
} gpio_debounce_data_t;

/**
 * @brief  This union represents the bit fields in the Clear interrupt Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_porta_eoi_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t clear_interrupt_w   : CONFIG_GPIO_PWIDTH_A;    /*!< Controls the clearing of edge type interrupts from
                                                                     Port A. When a 1 is written into a corresponding
                                                                     bit of this register, the interrupt is cleared.
                                                                     All interrupts are cleared when Port A is not
                                                                     configured for interrupts. */
    } b;                                                        /*!< Register bits. */
} gpio_porta_eoi_data_t;

/**
 * @brief  This union represents the bit fields in the External Port A Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_ext_porta_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t external_port_a     : CONFIG_GPIO_PWIDTH_A;    /*!< When Port A is configured as Input, then reading
                                                                     this location reads the values on the signal. When
                                                                     the data direction of Port A is set as Output,
                                                                     reading this location reads the data register for
                                                                     Port A. */
    } b;                                                        /*!< Register bits. */
} gpio_ext_porta_data_t;

/**
 * @brief  This union represents the bit fields in the External Port B Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_ext_portb_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t external_port_b     : CONFIG_GPIO_PWIDTH_B;    /*!< When Port B is configured as Input, then reading
                                                                     this location reads the values on the signal. When
                                                                     the data direction of Port B is set as Output,
                                                                     reading this location reads the data register for
                                                                     Port B. */
    } b;                                                        /*!< Register bits. */
} gpio_ext_portb_data_t;

/**
 * @brief  This union represents the bit fields in the External Port C Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_ext_portc_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t external_port_c     : CONFIG_GPIO_PWIDTH_C;    /*!< When Port C is configured as Input, then reading
                                                                     this location reads the values on the signal. When
                                                                     the data direction of Port C is set as Output,
                                                                     reading this location reads the data register for
                                                                     Port C. */
    } b;                                                        /*!< Register bits. */
} gpio_ext_portc_data_t;

/**
 * @brief  This union represents the bit fields in the External Port D Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_ext_portd_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t external_port_d     : CONFIG_GPIO_PWIDTH_D;    /*!< When Port D is configured as Input, then reading
                                                                     this location reads the values on the signal. When
                                                                     the data direction of Port D is set as Output,
                                                                     reading this location reads the data register for
                                                                     Port D. */
    } b;                                                        /*!< Register bits. */
} gpio_ext_portd_data_t;

/**
 * @brief  This union represents the bit fields in the Synchronization level Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_ls_sync_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t synchronization_level      : 1;                /*!< Writing a 1 to this register results in all
                                                                     level-sensitive interrupts being synchronized to
                                                                     pclk_intr. */
    } b;                                                        /*!< Register bits. */
} gpio_ls_sync_data_t;

/**
 * @brief  This union represents the bit fields in the GPIO ID code Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_id_code_data {
    uint32_t d32;                                              /*!< Raw register data. */
    struct {
        uint32_t gpio_id_code        : CONFIG_GPIO_WIDTH;       /*!< This is a user-specified code that a system can
                                                                     read. It can be used for chip identification, and
                                                                     so on. */
    } b;                                                        /*!< Register bits. */
} gpio_id_code_data_t;

/**
 * @brief  This union represents the bit fields in the GPIO ID code Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_int_bothedge_data {
    uint32_t d32;                                               /*!< Raw register data. */
    struct {
        uint32_t gpio_int_bothedge   : CONFIG_GPIO_PWIDTH_A;    /*!< This register is available only if PORT A is
                                                                     configured to generate interrupts
                                                                     (GPIO_PORTA_INTR = Include(1)) and interrupt
                                                                     detection is configured to generate on both rising
                                                                     and falling edges of external input signal
                                                                     (GPIO_INT_BOTH_EDGE = Include(1)). */
    } b;                                                        /*!< Register bits. */
} gpio_int_bothedge_data_t;

/**
 * @brief  This union represents the bit fields in the GPIO Component Version Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_ver_id_code_data {
    uint32_t d32;                                    /*!< Raw register data. */
    struct {
        uint32_t gpio_component_version     : 32;    /*!< ASCII value for each number in the version, followed by
                                                          *. For example 32_30_31_2A represents the version 2.01*. */
    } b;                                             /*!< Register bits. */
} gpio_ver_id_code_data_t;

/**
 * @brief  This union represents the bit fields in the GPIO Configuration Register 1.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_config_reg1_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t apb_data_width             : 2;    /*!< The value of this register is derived from the
                                                         GPIO_APB_DATA_WIDTH configuration parameter. */
        uint32_t num_ports                  : 2;    /*!< The value of this register is derived from the GPIO_NUM_PORT
                                                         configuration parameter. */
        uint32_t porta_single_ctl           : 1;    /*!< The value of this register is derived from the
                                                         GPIO_PORTA_SINGLE_CTL configuration parameter. */
        uint32_t portb_single_ctl           : 1;    /*!< The value of this register is derived from the
                                                         GPIO_PORTB_SINGLE_CTL configuration parameter. */
        uint32_t portc_single_ctl           : 1;    /*!< The value of this register is derived from the
                                                         GPIO_PORTC_SINGLE_CTL configuration parameter. */
        uint32_t portd_single_ctl           : 1;    /*!< The value of this register is derived from the
                                                         GPIO_PORTD_SINGLE_CTL configuration parameter. */
        uint32_t hw_porta                   : 1;    /*!< The value of this register is derived from the GPIO_HW_PORTA
                                                         configuration parameter. */
        uint32_t hw_portb                   : 1;    /*!< The value of this register is derived from the GPIO_HW_PORTB
                                                         configuration parameter. */
        uint32_t hw_portc                   : 1;    /*!< The value of this register is derived from the GPIO_HW_PORTC
                                                         configuration parameter. */
        uint32_t hw_portd                   : 1;    /*!< The value of this register is derived from the GPIO_HW_PORTD
                                                         configuration parameter. */
        uint32_t porta_intr                 : 1;    /*!< The value of this register is derived from the GPIO_PORTA_INTR
                                                         configuration parameter. */
        uint32_t debounce                   : 1;    /*!< The value of this register is derived from the GPIO_DEBOUNCE
                                                         configuration parameter. */
        uint32_t add_encoded_params         : 1;    /*!< The value of this register is derived from the
                                                         GPIO_ADD_ENCODED_PARAMS configuration parameter. */
        uint32_t gpio_id                    : 1;    /*!< The value of this register is derived from the GPIO_ID
                                                         configuration parameter. */
        uint32_t encoded_id_width           : 5;    /*!< The value of this register is equal to GPIO_ID_WIDTH-1. */

        uint32_t reserved21_31              : 11;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} gpio_config_reg1_data_t;

/**
 * @brief  This union represents the bit fields in the  GPIO Configuration Register 2.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gpio_config_reg2_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t encoded_id_pwidth_a        : 5;    /*!< The value of this register is equal to GPIO_PWIDTH_A-1. */
        uint32_t encoded_id_pwidth_b        : 5;    /*!< The value of this register is equal to GPIO_PWIDTH_B-1. */
        uint32_t encoded_id_pwidth_c        : 5;    /*!< The value of this register is equal to GPIO_PWIDTH_C-1. */
        uint32_t encoded_id_pwidth_d        : 5;    /*!< The value of this register is equal to GPIO_PWIDTH_D-1. */

        uint32_t reserved20_31              : 12;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} gpio_config_reg2_data_t;

typedef struct gpio_info_stru {
    volatile uint32_t gpio_swporta_dr;          /*!< (0x00) : port A data register */
    volatile uint32_t gpio_swporta_ddr;         /*!< (0x04) : port A data direction register */
    volatile uint32_t gpio_swporta_ctl;         /*!< (0x08) : Port A data source register */

    volatile uint32_t gpio_swportb_dr;          /*!< (0x0c) : Port B data register */
    volatile uint32_t gpio_swportb_ddr;         /*!< (0x10) : Port B data direction register */
    volatile uint32_t gpio_swportb_ctl;         /*!< (0x14) : Port B data source register */

    volatile uint32_t gpio_swportc_dr;          /*!< (0x18) : Port C data register */
    volatile uint32_t gpio_swportc_ddr;         /*!< (0x1c) : Port C data direction register */
    volatile uint32_t gpio_swportc_ctl;         /*!< (0x20) : Port C data source register */

    volatile uint32_t gpio_swportd_dr;          /*!< (0x24) : Port D data register */
    volatile uint32_t gpio_swportd_ddr;         /*!< (0x28) : Port D data direction register */
    volatile uint32_t gpio_swportd_ctl;         /*!< (0x2c) : Port D data source register */

    volatile uint32_t gpio_inten;               /*!< (0x30) : Interrupt enable */
    volatile uint32_t gpio_intmask;             /*!< (0x34) : Interrupt mask */
    volatile uint32_t gpio_inttype_level;       /*!< (0x38) : Interrupt level */

    volatile uint32_t gpio_int_polarity;        /*!< (0x3c) : Interrupt polarity */
    volatile uint32_t gpio_intstatus;           /*!< (0x40) : Interrupt status */
    volatile uint32_t gpio_raw_intstatus;       /*!< (0x44) : Raw interrupt status */

    volatile uint32_t gpio_debounce;            /*!< (0x48) : Debounce enable */
    volatile uint32_t gpio_porta_eoi;           /*!< (0x4c) : Clear interrupt */
    volatile uint32_t gpio_ext_porta;           /*!< (0x50) : External Port A */

    volatile uint32_t gpio_ext_portb;           /*!< (0x54) : External Port B */
    volatile uint32_t gpio_ext_portc;           /*!< (0x58) : External Port C */
    volatile uint32_t gpio_ext_portd;           /*!< (0x5c) : External Port D */

    volatile uint32_t gpio_ls_sync;             /*!< (0x60) : Synchronization level */
    volatile uint32_t gpio_id_code;             /*!< (0x64) : GPIO ID code */
    volatile uint32_t gpio_int_bothedge;        /*!< (0x68) : interrupt double edge type */

    volatile uint32_t gpio_ver_id_code;         /*!< (0x6c) : GPIO Component Version */
    volatile uint32_t gpio_config_reg2;         /*!< (0x70) : GPIO Configuration Register 2 */
    volatile uint32_t gpio_config_reg1;         /*!< (0x74) : GPIO Configuration Register 1 */
} gpio_info_stru_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
