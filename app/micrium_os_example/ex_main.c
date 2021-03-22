/***************************************************************************//**
 * @file
 * @brief Example main()
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <bsp/siliconlabs/generic/include/bsp_os.h>

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

#include  <ex_description.h>

#include  "common/common/ex_common.h"

#if (OS_CFG_TRACE_EN == DEF_ENABLED)
#include  <kernel/include/os_trace.h>
#endif

#if (defined(RTOS_MODULE_FS_AVAIL) && defined(EX_FS_INIT_AVAIL))
#include  "fs/ex_fs.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_MAIN_START_TASK_PRIO              21u
#define  EX_MAIN_START_TASK_STK_SIZE         768u

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Start Task Stack.
static CPU_STK Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
//                                                                 Start Task TCB.
static OS_TCB Ex_MainStartTaskTCB;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_MainStartTask(void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                 main()
 *
 * @brief  This is the standard entry point for C applications. It is assumed that your code will
 *         call main() once you have performed all necessary initialization.
 *******************************************************************************************************/
int main(void)
{
  RTOS_ERR err;

  CPU_Init();                                                   // Initialize CPU.
  BSP_SystemInit();                                             // Initialize System.

  OSInit(&err);                                                 // Initialize the Kernel.
                                                                // Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 1);

  OSTaskCreate(&Ex_MainStartTaskTCB,                            // Create the Start Task.
               "Ex Main Start Task",
               Ex_MainStartTask,
               DEF_NULL,
               EX_MAIN_START_TASK_PRIO,
               &Ex_MainStartTaskStk[0],
               (EX_MAIN_START_TASK_STK_SIZE / 10u),
               EX_MAIN_START_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 1);

  OSStart(&err);                                                // Start the kernel.
                                                                // Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 1);

  return (1);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_MainStartTask()
 *
 * @brief  This is the task that will be called by the Startup when all services are initializes
 *         successfully.
 *
 * @param  p_arg   Argument passed from task creation. Unused, in this case.
 *******************************************************************************************************/
static void Ex_MainStartTask(void *p_arg)
{
  RTOS_ERR err;

  PP_UNUSED_PARAM(p_arg);                                       // Prevent compiler warning.

#if (OS_CFG_TRACE_EN == DEF_ENABLED)
  OS_TRACE_INIT();                                              // Initialize the Kernel events trace recorder.
#endif

  Ex_CommonInit();                                              // Call common module initialization example.

  BSP_OS_Init();                                                // Initialize the BSP. It is expected that the BSP ...
                                                                // ... will register all the hardware controller to ...
                                                                // ... the platform manager at this moment.
#if (defined(RTOS_MODULE_FS_AVAIL) && defined(EX_FS_INIT_AVAIL))
  Ex_FS_Init();                                                 // Call File System initialization example.
#endif

  // TODO : Add application code or other example calls here.

  while (DEF_ON) {
    //                                                             Delay Start Task execution for
    OSTimeDly(5000,                                             // 5000 OS Ticks
              OS_OPT_TIME_PERIODIC,                             // from now.
              &err);
    //                                                             Check error code.
    APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  }
}
