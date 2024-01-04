/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm sleep port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */
#ifndef PM_SLEEP_PORTING_H
#define PM_SLEEP_PORTING_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_pm_sleep_port PM sleep port
 * @ingroup  middleware_chips_pm
 * @{
 */

#ifndef CONFIG_PM_LIGHT_SLEEP_THRESHOLD_MS
#define CONFIG_PM_LIGHT_SLEEP_THRESHOLD_MS 1
#endif

#ifndef CONFIG_PM_DEEP_SLEEP_THRESHOLD_MS
#define CONFIG_PM_DEEP_SLEEP_THRESHOLD_MS 20
#endif

void pm_wakeup_rtc_init(void);
void pm_wakeup_rtc_start(uint32_t time_ms);

void pm_port_start_tickless(void);
void pm_port_stop_tickless(uint32_t sleep_ms);
uint32_t pm_port_get_sleep_ms(void);
void pm_port_allow_deepsleep(bool allow);
void pm_port_enter_wfi(void);
void pm_port_start_wakeup_timer(uint32_t sleep_ms);
void pm_port_lightsleep_config(void);
void pm_port_light_wakeup_config(void);
void pm_port_deepsleep_config(void);
void pm_port_deep_wakeup_config(void);
void lowpower_cpu_suspend(void);
void lowpower_cpu_resume(void);
void pm_port_cpu_suspend(void);
void pm_port_cpu_resume(void);
uint16_t pm_port_get_sleep_event_status(void);
uint16_t pm_port_get_wakeup_event_status(void);
void pm_port_sleep_config_int(void);

#define PM_GET_SLEEP_EVENT_STATUS   pm_port_get_sleep_event_status()
#define PM_GET_WKUP_EVENT_STATUS  pm_port_get_wakeup_event_status()

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

