/***************************************************************************//**
 * @brief ZigBee App Framework Interface CMSIS RTOS adaptation code.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include PLATFORM_HEADER
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif

#include "sl_zigbee.h"

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#if defined(SL_ZIGBEE_AF_NCP) && defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
#include "sl_iostream.h"
#endif

#include "sl_zigbee_system_common.h"
#include "sl_zigbee_rtos_task_config.h"
#include "zigbee_ipc_command_messages.h"
#include "sl_event_system.h"
#include "sl_memory_manager.h"
#include "zigbee_rtos_adaptation.h"
#include "zigbee_sleep_config.h"
extern void sli_zigbee_process_stack_callbacks_event(sl_event_t *cb_event);

extern void sl_zigbee_af_acquire_lock(void);
extern void sl_zigbee_af_release_lock(void);

// Zigbee Application framework task definitions

//Zigbee AF task size is specified in bytes and is word aligned
static osThreadId_t zigbee_app_framework_task_id;
__ALIGNED(8) static uint8_t zigbee_app_framework_task_stack[SL_ZIGBEE_APP_FRAMEWORK_RTOS_TASK_STACK_SIZE];
__ALIGNED(4) static uint8_t zigbee_app_framework_task_cb[osThreadCbSize];
static osThreadAttr_t zigbee_app_framework_task_attr;

static osSemaphoreId_t zigbee_app_framework_task_semaphore_id;
__ALIGNED(4) static uint8_t zigbee_app_framework_task_semaphore_cb[osSemaphoreCbSize];
static osSemaphoreAttr_t zigbee_app_framework_task_semaphore_attr = {
  .name = "Zigbee AF task semaphore",
  .cb_mem = zigbee_app_framework_task_semaphore_cb,
  .cb_size = osSemaphoreCbSize,
  .attr_bits = 0
};

static void zigbee_app_framework_task(void *p_arg);
static void zigbee_app_framework_task_yield(void);
static void sl_zigbee_rtos_wakeup_app_framework_task(void);

//------------------------------------------------------------------------------
// APIs and callbacks.
void sl_zigbee_wakeup_common_task(void)
{
  sl_zigbee_wakeup_stack_task();
  sl_zigbee_rtos_wakeup_app_framework_task();
}

sl_event_queue_t callback_event_queue;

void sl_zigbee_wakeup_app_framework_task(void)
{
  // NOTE: This semaphore is not really used to protect any resources
  // It is a way to get the zigbee task to run and yield
  // Initial value of the semaphore is 0. So we will block when we try to acquire
  // Once a CLI task or ISR releases the semaphore, this task will be able to run
  // We do not use OS event flags here for the following reason
  // If multiple ISR events cause the flag to get set many times before the task is able to run
  // then, these get queued and eventually run when the OS timer task runs. This  could
  // cause an event queue overflow if there is a higher priority task. Therefore, we use a
  // semaphore in its place.
  osStatus_t retVal = osSemaphoreRelease(zigbee_app_framework_task_semaphore_id);
  assert(retVal != osErrorParameter);
}

void sli_zigbee_app_framework_rtos_task_init_cb(void)
{
  // Create ZigBee task.
  zigbee_app_framework_task_attr.name = "Zigbee AppFramework";
  zigbee_app_framework_task_attr.stack_mem = &zigbee_app_framework_task_stack[0];
  zigbee_app_framework_task_attr.stack_size = sizeof(zigbee_app_framework_task_stack);
  zigbee_app_framework_task_attr.cb_mem = zigbee_app_framework_task_cb;
  zigbee_app_framework_task_attr.cb_size = osThreadCbSize;
  zigbee_app_framework_task_attr.priority = (osPriority_t)SL_ZIGBEE_APP_FRAMEWORK_RTOS_TASK_PRIORITY;
  zigbee_app_framework_task_attr.attr_bits = 0;
  zigbee_app_framework_task_attr.tz_module = 0;

  zigbee_app_framework_task_id = osThreadNew(zigbee_app_framework_task,
                                             NULL,
                                             &zigbee_app_framework_task_attr);
  assert(zigbee_app_framework_task_id != NULL);

  zigbee_app_framework_task_semaphore_id =   osSemaphoreNew(ZIGBEE_TASK_SEMAPHORE_MAX_COUNT,
                                                            ZIGBEE_TASK_SEMAPHORE_INITIAL_COUNT,
                                                            &zigbee_app_framework_task_semaphore_attr);
  assert(zigbee_app_framework_task_semaphore_id != NULL);
}

//------------------------------------------------------------------------------
// Static functions.
#if defined(SL_ZIGBEE_AF_NCP) && defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
void sli_iostream_on_uart_rx(sl_iostream_t *handle)
{
  sl_zigbee_wakeup_common_task();
}
#endif

void sl_zigbee_rtos_wakeup_app_framework_task(void)
{
  // NOTE: This semaphore is not really used to protect any resources
  // It is a way to get the zigbee task to run and yield
  // Initial value of the semaphore is 0. So we will block when we try to acquire
  // Once a CLI task or ISR releases the semaphore, this task will be able to run
  // We do not use OS event flags here for the following reason
  // If multiple ISR events cause the flag to get set many times before the task is able to run
  // then, these get queued and eventually run when the OS timer task runs. This  could
  // cause an event queue overflow if there is a higher priority task. Therefore, we use a
  // semaphore in its place.
  osStatus_t retVal = osSemaphoreRelease(zigbee_app_framework_task_semaphore_id);
  assert(retVal != osErrorParameter);
}

static void zigbee_app_framework_task(void *p_arg)
{
  (void)p_arg;

  sli_zigbee_app_framework_init_callback();

  while (true) {
#ifdef SL_CATALOG_KERNEL_PRESENT
    sl_zigbee_af_acquire_lock();
#endif
    sli_zigbee_app_framework_tick_callback();

    sl_event_t *event_ptr;

    //Process all events in the event queue
    while ( SL_STATUS_OK == sl_event_queue_get(callback_event_queue, 0, 1, &event_ptr)) {
      sli_zigbee_process_stack_callbacks_event(event_ptr);
      sl_event_process(&event_ptr);
    }

#ifdef SL_CATALOG_KERNEL_PRESENT
    sl_zigbee_af_release_lock();
#endif

    // Yield the ZigBee stack task if possible.
    zigbee_app_framework_task_yield();
  }
}
static void zigbee_app_framework_task_yield(void)
{
  uint32_t yield_time_ms = 0;
  uint32_t yield_time_ticks = 0;

#ifdef SL_CATALOG_ZIGBEE_FORCE_SLEEP_AND_WAKEUP_PRESENT
  if (sli_zigbee_app_framework_is_force_sleep() == true) {
    yield_time_ms = osWaitForever;
  } else
#endif //#ifdef SL_CATALOG_ZIGBEE_FORCE_SLEEP_AND_WAKEUP_PRESENT
  {
    // Function below adds or removes EM1 requirement
    // and also checks stack and app events to see how long
    // the zigbee task can pend for
    yield_time_ms = sli_zigbee_app_framework_set_pm_requirements_and_get_ms_to_next_wakeup();
  }

  // Convert time in ms to ticks
  if (yield_time_ms != osWaitForever) {
    yield_time_ticks = (osKernelGetTickFreq() * yield_time_ms) / 1000;
  } else {
    yield_time_ticks = osWaitForever;
  }

  // NOTE: This semaphore is not really used to protect any resources
  // It is a way to get the zigbee task to run and yield
  // Initial value of the semaphore is 0. So we will block when we try to acquire
  // Once a CLI task or ISR releases the semaphore, this task will be able to run
  // We do not use OS event flags here for the following reason
  // If multiple ISR events cause the flag to get set many times before the task is able to run
  // then, these get queued and eventually run when the OS timer task runs. This  could
  // cause an event queue overflow if there is a higher priority task. Therefore, we use a
  // semaphore in its place.
  if (yield_time_ticks > 0) {
    osStatus_t retVal = osSemaphoreAcquire(zigbee_app_framework_task_semaphore_id, yield_time_ticks);
    assert((retVal != osErrorParameter) && (retVal != osErrorResource));
  }
}
