/***************************************************************************//**
 * @brief ZigBee Stack CMSIS RTOS adaptation code.
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
#include "sl_event_system.h"
#include "sl_memory_manager.h"
#include "zigbee_rtos_adaptation.h"
//------------------------------------------------------------------------------
// Zigbee Stack task definitions

static osEventFlagsId_t zigbee_ipc_flags_id;
__ALIGNED(4) static uint8_t zigbee_ipc_flags_cb[osEventFlagsCbSize];
static osEventFlagsAttr_t zigbee_ipc_flags_attr = {
  .name = "ZigbeeIPCFlags",
  .cb_mem = zigbee_ipc_flags_cb,
  .cb_size = osEventFlagsCbSize,
  .attr_bits = 0
};

//Zigbee stack size is specified in bytes and is word aligned
static osThreadId_t zigbee_stack_task_id;
__ALIGNED(8) static uint8_t zigbee_stack_task_stack[SL_ZIGBEE_STACK_RTOS_TASK_STACK_SIZE];
__ALIGNED(4) static uint8_t zigbee_stack_task_cb[osThreadCbSize];
static osThreadAttr_t zigbee_stack_task_attr;

static osSemaphoreId_t zigbee_stack_task_semaphore_id;
__ALIGNED(4) static uint8_t zigbee_stack_task_semaphore_cb[osSemaphoreCbSize];
static osSemaphoreAttr_t zigbee_stack_task_semaphore_attr = {
  .name = "Zigbee Stack task semaphore",
  .cb_mem = zigbee_stack_task_semaphore_cb,
  .cb_size = osSemaphoreCbSize,
  .attr_bits = 0
};
static volatile osMutexId_t zigbee_ipc_mutex_id;
static const osMutexAttr_t zigbee_ipc_mutex_attr = {
  .name = "Zigbee IPC Mutex",
  .attr_bits = osMutexRecursive | osMutexPrioInherit,
};
static void zigbee_stack_task(void *p_arg);
static void zigbee_stack_task_yield(void);
extern uint32_t sli_zigbee_stack_get_ms_to_next_wakeup(void);
extern void sl_zigbee_wakeup_common_task(void);

// Global pending IPC message
#ifdef SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT
#include "zigbee_ipc_command_messages.h"
static sli_zigbee_ipc_cmd_handler_t sli_zigbee_ipc_pending_command_handler = NULL;
static sli_zigbee_ipc_cmd_t* sli_zigbee_ipc_pending_command = NULL;
extern sl_event_publisher_t sli_zigbee_ipc_publisher;
static void event_data_free_cb_irq(void *event)
{
  sl_event_t * event_ptr = event;
  sl_memory_free(event_ptr);
}
#endif //SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT

void sl_zigbee_wakeup_stack_task(void)
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
  osStatus_t retVal = osSemaphoreRelease(zigbee_stack_task_semaphore_id);
  assert(retVal != osErrorParameter);
}

void sli_zigbee_stack_rtos_task_init_cb(void)
{
  // Create ZigBee task.
  zigbee_stack_task_attr.name = "Zigbee Stack";
  zigbee_stack_task_attr.stack_mem = &zigbee_stack_task_stack[0];
  zigbee_stack_task_attr.stack_size = sizeof(zigbee_stack_task_stack);
  zigbee_stack_task_attr.cb_mem = zigbee_stack_task_cb;
  zigbee_stack_task_attr.cb_size = osThreadCbSize;
  zigbee_stack_task_attr.priority = (osPriority_t)SL_ZIGBEE_STACK_RTOS_TASK_PRIORITY;
  zigbee_stack_task_attr.attr_bits = 0;
  zigbee_stack_task_attr.tz_module = 0;

  zigbee_stack_task_id = osThreadNew(zigbee_stack_task,
                                     NULL,
                                     &zigbee_stack_task_attr);
  assert(zigbee_stack_task_id != NULL);

  zigbee_stack_task_semaphore_id =   osSemaphoreNew(ZIGBEE_TASK_SEMAPHORE_MAX_COUNT,
                                                    ZIGBEE_TASK_SEMAPHORE_INITIAL_COUNT,
                                                    &zigbee_stack_task_semaphore_attr);
  assert(zigbee_stack_task_semaphore_id != NULL);

  // Create mutex for Zigbee IPC
  zigbee_ipc_mutex_id = osMutexNew(&zigbee_ipc_mutex_attr);
  if (zigbee_ipc_mutex_id == NULL) {
    // Unable to create mutex
    assert(0);
  }

  zigbee_ipc_flags_id = osEventFlagsNew(&zigbee_ipc_flags_attr);
  assert(zigbee_ipc_flags_id != NULL);
#ifdef SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT
  // Register event system publisher for publishing callbacks from stack to application framework
  sl_event_publisher_register(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, event_data_free_cb_irq);
#endif //SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT
#if defined(SL_CATALOG_ZIGBEE_SYSTEM_COMMON_APP_FRAMEWORK_PRESENT)
  // Subscribe to publisher and set up queue for receiving events in the application framework task
  sl_event_queue_create(100, &callback_event_queue);
  sl_event_subscribe(SL_EVENT_CLASS_ZIGBEE, //event_class
                     0xFFFFFFFF,            //event_mask
                     callback_event_queue);
#endif //defined(SL_CATALOG_ZIGBEE_SYSTEM_COMMON_APP_FRAMEWORK_PRESENT)
}

//------------------------------------------------------------------------------
// Static functions.
#if defined(SL_ZIGBEE_AF_NCP) && defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
void sli_iostream_on_uart_rx(sl_iostream_t *handle)
{
  sl_zigbee_wakeup_common_task();
}
#endif

static void zigbee_stack_task(void *p_arg)
{
  (void)p_arg;

  sli_zigbee_stack_init_callback();
#ifdef SL_ZIGBEE_AF_NCP
  sli_zigbee_ncp_init_callback();
#endif

  while (true) {
    sli_zigbee_stack_tick_callback();
#ifdef SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT
    if ( osEventFlagsGet(zigbee_ipc_flags_id) & ZIGBEE_IPC_COMMAND_AVAILABLE_FLAG ) {
      sli_zigbee_ipc_pending_command_handler(sli_zigbee_ipc_pending_command);
      osEventFlagsClear(zigbee_ipc_flags_id, ZIGBEE_IPC_COMMAND_AVAILABLE_FLAG);
      (void)osEventFlagsSet(zigbee_ipc_flags_id, ZIGBEE_IPC_COMMAND_PROCESSED_FLAG);
    }
#endif// SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT
#ifdef SL_ZIGBEE_AF_NCP
    sli_zigbee_ncp_tick_callback();
#endif

    // Yield the ZigBee stack task if possible.
    zigbee_stack_task_yield();
  }
}
#ifdef SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT
void sli_zigbee_send_ipc_cmd(sli_zigbee_ipc_cmd_handler_t handler,
                             sli_zigbee_ipc_cmd_t* cmd)
{
  // Zigbee APIs can be called from several tasks
  // If they are called from the stack task, the call is direct without IPC
  // Else, they need go flow through the IPC
  if (zigbee_stack_task_id == osThreadGetId()) {
    handler(cmd);
  } else {
    osStatus_t ret = osMutexAcquire(zigbee_ipc_mutex_id, 0);

    sli_zigbee_ipc_pending_command_handler = handler;
    sli_zigbee_ipc_pending_command = cmd;

    osEventFlagsSet(zigbee_ipc_flags_id, ZIGBEE_IPC_COMMAND_AVAILABLE_FLAG);

    // Posts the vNCP task so that it processes the IPC command
    sl_zigbee_wakeup_stack_task();

    // Block until the vNCP is done processing the IPC command
    // This basically pends on a dedicated OS construct (Flag, Semaphore, etc.)

    osEventFlagsWait(zigbee_ipc_flags_id,
                     ZIGBEE_IPC_COMMAND_PROCESSED_FLAG,
                     osFlagsWaitAny,
                     0);

    osEventFlagsClear(zigbee_ipc_flags_id, ZIGBEE_IPC_COMMAND_PROCESSED_FLAG);

    ret = osMutexRelease(zigbee_ipc_mutex_id);
    (void)ret;
  }
}
#endif// SL_CATALOG_ZIGBEE_REAL_IPC_PRESENT
static void zigbee_stack_task_yield(void)
{
  uint32_t yield_time_ms = 0;
  uint32_t yield_time_ticks = 0;

  // Function below adds or removes EM1 requirement
  // and also checks stack and app events to see how long
  // the zigbee task can pend for
#ifdef SL_CATALOG_ZIGBEE_FORCE_SLEEP_AND_WAKEUP_PRESENT
  if (sli_zigbee_stack_is_force_sleep() == true) {
    yield_time_ms = osWaitForever;
  } else
#endif //#ifdef SL_CATALOG_ZIGBEE_FORCE_SLEEP_AND_WAKEUP_PRESENT
  {
    yield_time_ms = sli_zigbee_stack_get_ms_to_next_wakeup();
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
    osStatus_t retVal = osSemaphoreAcquire(zigbee_stack_task_semaphore_id, yield_time_ticks);
    assert((retVal != osErrorParameter) && (retVal != osErrorResource));
  }
}

bool sli_zigbee_is_stack_task_or_isr_current_context(void)
{
  return ( (zigbee_stack_task_id == osThreadGetId())
           || CORE_InIrqContext());
}

#if !defined(SL_CATALOG_ZIGBEE_SYSTEM_COMMON_APP_FRAMEWORK_PRESENT)
void sl_zigbee_wakeup_common_task(void)
{
  sl_zigbee_wakeup_stack_task();
}
#endif
