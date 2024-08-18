#include "app_init.h"
#include "boards.h"
#include "gpio.h"
#include "osal_task.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "stdint.h"
#include "test_suite_log.h"
#include <stdint.h>

#define SK6812_PIN S_MGPIO11

// 根据CPU时钟频率和指令执行时间，调整这些常量
#define CPU_CLOCK_MHZ 32 // 假设CPU时钟频率为48MHz
#define NS_PER_CYCLE (1000 / CPU_CLOCK_MHZ)

#define T0H 320 // 0码，高电平时间 (单位: ns)
#define T0L 800 // 0码，低电平时间 (单位: ns)
#define T1H 640 // 1码，高电平时间 (单位: ns)
#define T1L 640 // 1码，低电平时间 (单位: ns)
#define RES 80  // Reset码，低电平时间 (单位: µs)

#define BLINKY_TASK_STACK_SIZE 0x400
#define BLINKY_TASK_PRIO OSAL_TASK_PRIORITY_ABOVE_HIGH

// 简单的忙等待循环
static void delay_ns(uint32_t ns) {
  uint32_t cycles = ns / NS_PER_CYCLE;
  for (volatile uint32_t i = 0; i < cycles; i++) {
    __asm__("nop"); // 空指令，消耗时间
  }
}

static void sk6812_send_bit(uint8_t bit) {
  if (bit) {
    uapi_gpio_set_val(SK6812_PIN, GPIO_LEVEL_HIGH);
    delay_ns(T1H); // 延时高电平T1H时间
    uapi_gpio_set_val(SK6812_PIN, GPIO_LEVEL_LOW);
    delay_ns(T1L); // 延时低电平T1L时间
  } else {
    uapi_gpio_set_val(SK6812_PIN, GPIO_LEVEL_HIGH);
    delay_ns(T0H); // 延时高电平T0H时间
    uapi_gpio_set_val(SK6812_PIN, GPIO_LEVEL_LOW);
    delay_ns(T0L); // 延时低电平T0L时间
  }
}

static void sk6812_send_byte(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    sk6812_send_bit((byte << i) & 0x80); // 发送最高位开始的每一位
  }
}

static void sk6812_reset(void) {
  uapi_gpio_set_val(SK6812_PIN, GPIO_LEVEL_LOW);
  osal_msleep(RES); // 延时Reset低电平时间
}

static void *sk6812_task(const char *arg) {
  unused(arg);

  uapi_pin_set_mode(SK6812_PIN, HAL_PIO_FUNC_GPIO);
  uapi_gpio_set_dir(SK6812_PIN, GPIO_DIRECTION_OUTPUT);
  uapi_gpio_set_val(SK6812_PIN, GPIO_LEVEL_LOW);

  while (1) {
    // 发送SK6812数据
    sk6812_send_byte(0x00); // 示例: 全亮红色
    sk6812_send_byte(0x00); // 示例: 绿色关闭
    sk6812_send_byte(0x00); // 示例: 蓝色关闭
    sk6812_reset();         // 发送完数据后进行Reset
    osal_msleep(1000);      // 延时一段时间后再发送数据
  }
  return NULL;
}

static void sk6812_entry(void) {
  uint32_t ret;
  osal_task *taskid;

  // 创建任务调度
  osal_kthread_lock();

  // 创建任务
  taskid = osal_kthread_create((osal_kthread_handler)sk6812_task, NULL,
                               "sk6812_task", BLINKY_TASK_STACK_SIZE);

  // 设置任务优先级
  ret = osal_kthread_set_priority(taskid, BLINKY_TASK_PRIO);
  if (ret != OSAL_SUCCESS) {
    printf("create task failed.\n");
  }
  osal_kthread_unlock();
}

/* Run the sk6812_entry. */
app_run(sk6812_entry);
