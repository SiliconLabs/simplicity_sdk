/***************************************************************************//**
 * @file
 * @brief This file implements the GPIO monitor for RAILtest applications.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#if !defined(__ICCARM__)
// IAR doesn't have strings.h and puts those declarations in string.h
#include <strings.h>
#endif

#include "sl_core.h"
#include "em_device.h"
#include "dmadrv.h"
#include "hal_common.h"
#include "response_print.h"
#include "buffer_pool_allocator.h"

#include "rail.h"
#include "rail_rf_hal.h"
#include "app_common.h"

// EFR32 Built-in Single Pin Monitor

// This command creates a full logic monitor to monitor a single GPIO pin. The
// final output is two arrays of timestamps, one for rising edges, and one for
// falling edges.

// The timestamping source is based on the RAIL_TimerTick hardware timer. This
// logic monitor consumes a number of resources: Three PRS signals, two LDMA
// channels, two LMDAXBAR request signals, and a GPIO interrupt. The LDMA
// channels will be allocated based on the DMADRV if RAIL_DMA_CHANNEL is
// DMA_CHANNEL_DMADRV, and are otherwise set by the macro logic in the monitor
// init code. The two PRS channels will always be set by the macros below. The
// first two LDMAXBAR signals are always used. The GPIO interrupt will be
// chosen as the pin number that is being monitored.

#define MONITOR_NUM_SAMPLES  (50U)

#define MONITOR_PRS_CH_0     (0U)
#define MONITOR_PRS_CH_1     (1U)
#define MONITOR_PRS_CH_2     (2U)
#define INVALID_PRS_CHANNEL  (-1)
#define TOGGLES (&monitor_samples[0]) // Pin monitor on Series-1
#define RISING (&monitor_samples[0]) // Pin monitor on Series-2
#define FALLING (&monitor_samples[MONITOR_NUM_SAMPLES]) // Pin monitor on Series-2
#define TIMESTAMPS (&monitor_samples[0]) // Port monitor
#define GPIO_VALUES (&monitor_samples[MONITOR_NUM_SAMPLES]) // Port monitor

static uint32_t monitor_samples[MONITOR_NUM_SAMPLES * 2];
#if defined(_SILICON_LABS_32B_SERIES_1)
uint32_t initialState = false;
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
static int8_t monitor_prs_free_channel_0 = INVALID_PRS_CHANNEL;
static int8_t monitor_prs_free_channel_1 = INVALID_PRS_CHANNEL;
static int8_t monitor_prs_free_channel_2 = INVALID_PRS_CHANNEL;
#endif

static unsigned int dma_ch0;
static unsigned int dma_ch1;

typedef enum {
  SL_UNINITIALIZED,
  SL_PIN_MONITOR,
  SL_PORT_MONITOR,
} sl_monitor_state_t;

static sl_monitor_state_t sl_monitor_state = SL_UNINITIALIZED;

// Set up the monitor for an entire port. The monitor takes a single GPIO pin
// and connects it to 2 PRS channels. Those two PRS channels then each go to an
// LDMA via the LDMAXBAR (LDMA crossbar), which causes a RAIL_TimerTick sample
// to be written to the timestamps array and the port's GPIO DOUT to be written
// to the GPIO values array.
static sl_monitor_state_t sl_init_port_monitor(uint8_t port, uint8_t pin)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  // Enable clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_LDMA, true);
#ifdef _CMU_CLKEN0_LDMAXBAR_SHIFT
  CMU_ClockEnable(cmuClock_LDMAXBAR, true);
#endif
#else
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_PRS);
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_LDMA0);
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_LDMAXBAR0);
#endif //_SILICON_LABS_32B_SERIES_2

  // Reset the buffers from any previous operation.
  memset(monitor_samples, 0xFFU, sizeof(monitor_samples));

  // Allocate DMA channels based on RAIL_DMA_CHANNEL
#if RAIL_DMA_CHANNEL == DMA_CHANNEL_DMADRV
  Ecode_t dmaError = DMADRV_Init();
  if ((dmaError == ECODE_EMDRV_DMADRV_ALREADY_INITIALIZED)
      || (dmaError == ECODE_EMDRV_DMADRV_OK)) {
    Ecode_t dma_error = DMADRV_AllocateChannel(&dma_ch0, NULL);
    if (dma_error != ECODE_EMDRV_DMADRV_OK) {
      return SL_UNINITIALIZED;
    }
    dma_error = DMADRV_AllocateChannel(&dma_ch1, NULL);
    if (dma_error != ECODE_EMDRV_DMADRV_OK) {
      return SL_UNINITIALIZED;
    }
  } else {
    return SL_UNINITIALIZED;
  }
#elif RAIL_DMA_CHANNEL == DMA_CHANNEL_INVALID
  // If RAIL doesn't use a DMA channel, take the first two
  dma_ch0 = 0U;
  dma_ch1 = 1U;
#else
  // If RAIL does have a channel, take the next two (mod DMA_CHAN_COUNT)
  dma_ch0 = ((RAIL_DMA_CHANNEL + 1U) % DMA_CHAN_COUNT);
  dma_ch1 = ((RAIL_DMA_CHANNEL + 2U) % DMA_CHAN_COUNT);
#endif

  uint32_t ldma_prs_conn0;
  uint32_t ldma_prs_conn1;

  // Configure the PRS logic we need on Series 2. One would expect a signal
  // being passed to both inputs of an XNOR to always be 1, but passing it
  // between PRS channels like this causes enough of a glitch low to trigger
  // the next capture. This is the only case that needs three PRS signals,
  // and monitor_prs_free_channel_2 is otherwise unused
#if defined(_SILICON_LABS_32B_SERIES_2)
  // Set up free PRS channel 0
  monitor_prs_free_channel_0 = PRS_GetFreeChannel(prsTypeAsync);
  if (monitor_prs_free_channel_0 == INVALID_PRS_CHANNEL) {
    return SL_UNINITIALIZED;
  }
  PRS_SourceAsyncSignalSet((uint8_t)monitor_prs_free_channel_0,
                           PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           pin);
  // Set up free PRS channel 1
  monitor_prs_free_channel_1 = PRS_GetFreeChannel(prsTypeAsync);
  if (monitor_prs_free_channel_1 == INVALID_PRS_CHANNEL) {
    return SL_UNINITIALIZED;
  }
  PRS_SourceAsyncSignalSet((uint8_t)monitor_prs_free_channel_1,
                           PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           pin);
  // Set up free PRS channel 2
  monitor_prs_free_channel_2 = PRS_GetFreeChannel(prsTypeAsync);
  if (monitor_prs_free_channel_2 == INVALID_PRS_CHANNEL) {
    return SL_UNINITIALIZED;
  }
  PRS_SourceAsyncSignalSet((uint8_t)monitor_prs_free_channel_2,
                           PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           pin);
  // Connect PRS channels to LDMA
  PRS_ConnectConsumer(monitor_prs_free_channel_2, prsTypeAsync, prsConsumerLDMA_REQUEST0);
  PRS_ConnectConsumer(monitor_prs_free_channel_0, prsTypeAsync, prsConsumerLDMA_REQUEST0);
  PRS_ConnectConsumer(monitor_prs_free_channel_1, prsTypeAsync, prsConsumerLDMA_REQUEST1);
  PRS_Combine(monitor_prs_free_channel_1, monitor_prs_free_channel_2, prsLogic_A_XNOR_B);
  PRS_Combine(monitor_prs_free_channel_0, monitor_prs_free_channel_2, prsLogic_A_XNOR_B);

  ldma_prs_conn0 = ldmaPeripheralSignal_LDMAXBAR_PRSREQ0;
  ldma_prs_conn1 = ldmaPeripheralSignal_LDMAXBAR_PRSREQ1;

  // The PRS handles GPIO interrupts, so we need to configure one for
  // channel we're using.
  GPIO_ExtIntConfig((GPIO_Port_TypeDef)port, pin, pin, false, false, false);

  // There's no SINGLE_P2M_WORD define, so modify a SINGLE_P2M_BYTE descriptor
  LDMA_Descriptor_t descriptor = LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(
    RAIL_TimerTick, TIMESTAMPS, MONITOR_NUM_SAMPLES);
  descriptor.xfer.size = ldmaCtrlSizeWord;
  LDMA_TransferCfg_t transfer = LDMA_TRANSFER_CFG_PERIPHERAL(ldma_prs_conn0);
  LDMA_StartTransfer(dma_ch0, &transfer, &descriptor);

  descriptor.xfer.srcAddr = (uint32_t) &(GPIO->P[port].DIN);
  descriptor.xfer.dstAddr = (uint32_t) GPIO_VALUES;
  transfer.ldmaReqSel = ldma_prs_conn1;
  LDMA_StartTransfer(dma_ch1, &transfer, &descriptor);

#else
  sl_hal_prs_async_channel_config_t monitorGpioCfg = \
    SL_HAL_PRS_ASYNC_INIT_DEFAULT(MONITOR_PRS_CH_2, SL_HAL_PRS_ASYNC_NONE, SL_HAL_PRS_CONSUMER_NONE);
  monitorGpioCfg.producer_signal = (sl_hal_prs_async_producer_signal_t) (PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO
                                                                         | ((uint32_t)pin << _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT));
  monitorGpioCfg.consumer_event = SL_HAL_PRS_CONSUMER_LDMAXBAR0_DMAREQ0;
  sl_hal_prs_async_init_channel(&monitorGpioCfg);
  sl_hal_prs_connect_channel_consumer(MONITOR_PRS_CH_2, SL_HAL_PRS_TYPE_ASYNC, monitorGpioCfg.consumer_event);

  monitorGpioCfg.channel = MONITOR_PRS_CH_0;
  sl_hal_prs_async_init_channel(&monitorGpioCfg);
  sl_hal_prs_connect_channel_consumer(MONITOR_PRS_CH_0, SL_HAL_PRS_TYPE_ASYNC, monitorGpioCfg.consumer_event);

  monitorGpioCfg.channel = MONITOR_PRS_CH_1;
  monitorGpioCfg.consumer_event = SL_HAL_PRS_CONSUMER_LDMAXBAR0_DMAREQ1;
  sl_hal_prs_async_init_channel(&monitorGpioCfg);
  sl_hal_prs_connect_channel_consumer(MONITOR_PRS_CH_1, SL_HAL_PRS_TYPE_ASYNC, monitorGpioCfg.consumer_event);

  sl_hal_prs_async_combine_signals(MONITOR_PRS_CH_1, MONITOR_PRS_CH_2, SL_HAL_PRS_LOGIC_A_XNOR_B);
  sl_hal_prs_async_combine_signals(MONITOR_PRS_CH_0, MONITOR_PRS_CH_2, SL_HAL_PRS_LOGIC_A_XNOR_B);

  ldma_prs_conn0 = SL_HAL_LDMA_PERIPHERAL_SIGNAL_LDMAXBAR0_PRSREQ0;
  ldma_prs_conn1 = SL_HAL_LDMA_PERIPHERAL_SIGNAL_LDMAXBAR0_PRSREQ1;

  // The PRS handles GPIO interrupts, so we need to configure one for
  // channel we're using.
  int32_t int_no = pin;
  sl_gpio_configure_external_interrupt(&(sl_gpio_t){port, pin }, &int_no, SL_GPIO_INTERRUPT_NO_EDGE, NULL, (void *)NULL);

  sl_hal_ldma_descriptor_t descriptor0 = SL_HAL_LDMA_DESCRIPTOR_SINGLE_P2M(SL_HAL_LDMA_CTRL_SIZE_WORD,
                                                                           RAIL_TimerTick,
                                                                           TIMESTAMPS,
                                                                           MONITOR_NUM_SAMPLES);
  sl_hal_ldma_transfer_config_t transferCfg0 = SL_HAL_LDMA_TRANSFER_CFG_PERIPHERAL(ldma_prs_conn0);
  sl_hal_ldma_init_transfer(LDMA0, dma_ch0, &transferCfg0, &descriptor0);
  sl_hal_ldma_start_transfer(LDMA0, dma_ch0);

  sl_hal_ldma_descriptor_t descriptor1 = SL_HAL_LDMA_DESCRIPTOR_SINGLE_P2M(SL_HAL_LDMA_CTRL_SIZE_WORD,
                                                                           &(GPIO->P[port].DIN),
                                                                           GPIO_VALUES,
                                                                           MONITOR_NUM_SAMPLES);
  sl_hal_ldma_transfer_config_t transferCfg1 = SL_HAL_LDMA_TRANSFER_CFG_PERIPHERAL(ldma_prs_conn1);
  sl_hal_ldma_init_transfer(LDMA0, dma_ch1, &transferCfg1, &descriptor1);
  sl_hal_ldma_start_transfer(LDMA0, dma_ch1);
#endif
  return SL_PORT_MONITOR;
}

// Set up the monitor for a single pin. The monitor takes a single GPIO pin and
// connects it to 2 PRS channels. One of the channels is inverted. Those two PRS
// channels then each go to an LDMA via the LDMAXBAR (LDMA crossbar), which
// causes one RAIL_TimerTick sample to be written to the appropriate array per
// GPIO rising or falling edge.
static sl_monitor_state_t sl_init_pin_monitor(uint8_t port, uint8_t pin)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  // Enable clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_LDMA, true);
#ifdef _CMU_CLKEN0_LDMAXBAR_SHIFT
  CMU_ClockEnable(cmuClock_LDMAXBAR, true);
#endif
#else
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_PRS);
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_LDMA0);
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_LDMAXBAR0);
#endif //_SILICON_LABS_32B_SERIES_2

  // Reset the buffers from any previous operation.
  memset(monitor_samples, 0xFFU, sizeof(monitor_samples));

  // Allocate DMA channels based on RAIL_DMA_CHANNEL
#if RAIL_DMA_CHANNEL == DMA_CHANNEL_DMADRV
  Ecode_t dmaError = DMADRV_Init();
  if ((dmaError == ECODE_EMDRV_DMADRV_ALREADY_INITIALIZED)
      || (dmaError == ECODE_EMDRV_DMADRV_OK)) {
    Ecode_t dma_error = DMADRV_AllocateChannel(&dma_ch0, NULL);
    if (dma_error != ECODE_EMDRV_DMADRV_OK) {
      return SL_UNINITIALIZED;
    }
    dma_error = DMADRV_AllocateChannel(&dma_ch1, NULL);
    if (dma_error != ECODE_EMDRV_DMADRV_OK) {
      return SL_UNINITIALIZED;
    }
  } else {
    return SL_UNINITIALIZED;
  }
#elif RAIL_DMA_CHANNEL == DMA_CHANNEL_INVALID
  // If RAIL doesn't use a DMA channel, take the first two
  dma_ch0 = 0U;
  dma_ch1 = 1U;
#else
  // If RAIL does have a channel, take the next two (mod DMA_CHAN_COUNT)
  dma_ch0 = ((RAIL_DMA_CHANNEL + 1U) % DMA_CHAN_COUNT);
  dma_ch1 = ((RAIL_DMA_CHANNEL + 2U) % DMA_CHAN_COUNT);
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
  // Configure the PRS logic we need on Series 2
  // Set up free PRS channel 0
  monitor_prs_free_channel_0 = (int8_t)PRS_GetFreeChannel(prsTypeAsync);
  if (monitor_prs_free_channel_0 == INVALID_PRS_CHANNEL) {
    return SL_UNINITIALIZED;
  }
  PRS_SourceAsyncSignalSet(monitor_prs_free_channel_0,
                           PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           pin);
  // Set up free PRS channel 1
  monitor_prs_free_channel_1 = (int8_t)PRS_GetFreeChannel(prsTypeAsync);
  if (monitor_prs_free_channel_1 == INVALID_PRS_CHANNEL) {
    return SL_UNINITIALIZED;
  }
  // Connect PRS channels to LDMA
  PRS_ConnectConsumer(monitor_prs_free_channel_0, prsTypeAsync, prsConsumerLDMA_REQUEST0);
  PRS_ConnectConsumer(monitor_prs_free_channel_1, prsTypeAsync, prsConsumerLDMA_REQUEST1);
  PRS_Combine(monitor_prs_free_channel_1, monitor_prs_free_channel_0, prsLogic_NOT_B);

  uint32_t ldma_prs_conn0 = ldmaPeripheralSignal_LDMAXBAR_PRSREQ0;
  uint32_t ldma_prs_conn1 = ldmaPeripheralSignal_LDMAXBAR_PRSREQ1;

  // The PRS handles GPIO interrupts, so we need to configure one for
  // channel we're using.
  GPIO_ExtIntConfig((GPIO_Port_TypeDef)port, pin, pin, false, false, false);

  // There's no SINGLE_P2M_WORD define, so modify a SINGLE_P2M_BYTE descriptor
  LDMA_Descriptor_t descriptor = LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(
    RAIL_TimerTick, RISING, MONITOR_NUM_SAMPLES);
  descriptor.xfer.size = ldmaCtrlSizeWord;
  LDMA_TransferCfg_t transfer = LDMA_TRANSFER_CFG_PERIPHERAL(ldma_prs_conn0);
  LDMA_StartTransfer(dma_ch0, &transfer, &descriptor);

  descriptor.xfer.dstAddr = (uint32_t) FALLING;
  transfer.ldmaReqSel = ldma_prs_conn1;
  LDMA_StartTransfer(dma_ch1, &transfer, &descriptor);
#else
  sl_hal_prs_async_channel_config_t monitorGpioCfg0 = \
    SL_HAL_PRS_ASYNC_INIT_DEFAULT(MONITOR_PRS_CH_0, SL_HAL_PRS_ASYNC_NONE, SL_HAL_PRS_CONSUMER_NONE);
  monitorGpioCfg0.producer_signal = (sl_hal_prs_async_producer_signal_t) (PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO
                                                                          | ((uint32_t)pin << _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT));
  monitorGpioCfg0.consumer_event = SL_HAL_PRS_CONSUMER_LDMAXBAR0_DMAREQ0;

  sl_hal_prs_async_channel_config_t monitorGpioCfg1 = \
    SL_HAL_PRS_ASYNC_INIT_DEFAULT(MONITOR_PRS_CH_1, SL_HAL_PRS_ASYNC_NONE, SL_HAL_PRS_CONSUMER_NONE);
  monitorGpioCfg1.producer_signal = (sl_hal_prs_async_producer_signal_t) (PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO
                                                                          | ((uint32_t)pin << _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT));
  monitorGpioCfg1.consumer_event = SL_HAL_PRS_CONSUMER_LDMAXBAR0_DMAREQ1;

  sl_hal_prs_async_init_channel(&monitorGpioCfg0);
  sl_hal_prs_connect_channel_consumer(MONITOR_PRS_CH_0, SL_HAL_PRS_TYPE_ASYNC, monitorGpioCfg0.consumer_event);

  sl_hal_prs_async_init_channel(&monitorGpioCfg1);
  sl_hal_prs_connect_channel_consumer(MONITOR_PRS_CH_1, SL_HAL_PRS_TYPE_ASYNC, monitorGpioCfg1.consumer_event);
  sl_hal_prs_async_combine_signals(MONITOR_PRS_CH_1, MONITOR_PRS_CH_0, SL_HAL_PRS_LOGIC_NOT_B);

  uint32_t ldma_prs_conn0 = SL_HAL_LDMA_PERIPHERAL_SIGNAL_LDMAXBAR0_PRSREQ0;
  uint32_t ldma_prs_conn1 = SL_HAL_LDMA_PERIPHERAL_SIGNAL_LDMAXBAR0_PRSREQ1;

  // The PRS handles GPIO interrupts, so we need to configure one for
  // channel we're using.
  int32_t int_no = pin;
  sl_gpio_configure_external_interrupt(&(sl_gpio_t){port, pin }, &int_no, SL_GPIO_INTERRUPT_NO_EDGE, NULL, (void *)NULL);

  sl_hal_ldma_descriptor_t descriptor0 = SL_HAL_LDMA_DESCRIPTOR_SINGLE_P2M(SL_HAL_LDMA_CTRL_SIZE_WORD,
                                                                           RAIL_TimerTick,
                                                                           RISING,
                                                                           MONITOR_NUM_SAMPLES);
  sl_hal_ldma_transfer_config_t transferCfg0 = SL_HAL_LDMA_TRANSFER_CFG_PERIPHERAL(ldma_prs_conn0);
  sl_hal_ldma_init_transfer(LDMA0, dma_ch0, &transferCfg0, &descriptor0);
  sl_hal_ldma_start_transfer(LDMA0, dma_ch0);

  sl_hal_ldma_descriptor_t descriptor1 = SL_HAL_LDMA_DESCRIPTOR_SINGLE_P2M(SL_HAL_LDMA_CTRL_SIZE_WORD,
                                                                           RAIL_TimerTick,
                                                                           FALLING,
                                                                           MONITOR_NUM_SAMPLES);
  sl_hal_ldma_transfer_config_t transferCfg1 = SL_HAL_LDMA_TRANSFER_CFG_PERIPHERAL(ldma_prs_conn1);
  sl_hal_ldma_init_transfer(LDMA0, dma_ch1, &transferCfg1, &descriptor1);
  sl_hal_ldma_start_transfer(LDMA0, dma_ch1);
#endif
  return SL_PIN_MONITOR;
}

static void sl_print_port_monitor_data(sl_cli_command_arg_t *args)
{
  responsePrintStart(sl_cli_get_command_string(args, 0));
  // rising and timestamps refer to the same data
  printf("{Timestamps:%lu", TIMESTAMPS[0]);

  uint32_t samples = 1U;
  while ((samples < MONITOR_NUM_SAMPLES)
         && (RISING[samples] != 0xFFFFFFFFUL)) {
    printf(" %lu", RISING[samples]);
    samples++;
  }

  // Port monitor uses GPIO values and timestamps
  printf("}{Values:0x%.8lx", GPIO_VALUES[0]);

  uint32_t i = 1U;
  while ((i < samples) && (FALLING[i] != 0xFFFFFFFFUL)) {
    printf(" %.8lx", FALLING[i]);
    i++;
  }
  responsePrintEnd("}}");
}

static void sl_print_pin_monitor_data(sl_cli_command_arg_t *args)
{
  responsePrintStart(sl_cli_get_command_string(args, 0));

#if defined(_SILICON_LABS_32B_SERIES_1)
  uint32_t samples = (initialState == 0U) ? 0U : 1U;
  printf("{Rising:%lu", TOGGLES[samples]);
  samples += 2U;

  while ((samples < MONITOR_NUM_SAMPLES)
         && (TOGGLES[samples] != 0xFFFFFFFFUL)) {
    printf(" %lu", TOGGLES[samples]);
    samples += 2U;
  }

  uint32_t i = (initialState == 0U) ? 1U : 0U;
  printf("}{Falling:%lu", TOGGLES[i]);
  i += 2U;

  // The GPIO may toggle while we're outputting falling samples. In order to
  // keep the data consistent, we need to limit the number of falling samples
  // to the number of rising samples, +1 if a falling sample happened first.
  // Loop over all of the equal samples, in case multiple toggles happened on
  // the same tick
  while ((i < (samples + 2U)) && (TOGGLES[i] != 0xFFFFFFFFUL)) {
    printf(" %lu", TOGGLES[i]);
    i += 2U;
  }
#else
  printf("{Rising:%lu", RISING[0]);

  uint32_t samples = 1U;
  while ((samples < MONITOR_NUM_SAMPLES)
         && (RISING[samples] != 0xFFFFFFFFUL)) {
    printf(" %lu", RISING[samples]);
    samples++;
  }

  uint32_t i = 0U;
  // The GPIO may toggle while we're outputting falling samples. In order to
  // keep the data consistent, we need to limit the number of falling samples
  // to the number of rising samples, +1 if a falling sample happened first.
  // Loop over all of the equal samples, in case multiple toggles happened on
  // the same tick
  if (samples < MONITOR_NUM_SAMPLES) {
    do {
      if (FALLING[i] < RISING[i]) {
        samples++;
        break;
      }
      i++;
    } while ((FALLING[i - 1] == RISING[i - 1]) && (i < samples));
  }

  // Pin monitor uses rising and falling edges
  printf("}{Falling:%lu", FALLING[0]);

  i = 1U;
  while ((i < samples) && (FALLING[i] != 0xFFFFFFFFUL)) {
    printf(" %lu", FALLING[i]);
    i++;
  }
#endif
  responsePrintEnd("}}");
}

static void sl_print_monitor_data(sl_cli_command_arg_t *args)
{
  if (sl_monitor_state == SL_PIN_MONITOR) {
    sl_print_pin_monitor_data(args);
  } else if (sl_monitor_state == SL_PORT_MONITOR) {
    sl_print_port_monitor_data(args);
  } else { // SL_UNINITIALIZED
    responsePrintError(sl_cli_get_command_string(args, 0), 0x53,
                       "The monitor is not initialized. Call 'init' before '%s'",
                       sl_cli_get_argument_string(args, 0));
  }
}

static void sl_print_monitor_help(sl_cli_command_arg_t *args)
{
  RAILTEST_PRINTF("\nUse the following commands:\n \
  '%s init' [port pin]    - To setup monitor on a single GPIO\n \
  '%s portInit' [port pin]- To setup monitor on a whole GPIO port\n \
  '%s get'                - To get GPIO rising and falling edges\n \
  '%s stop'               - To disable\n",            \
                  sl_cli_get_command_string(args, 0), \
                  sl_cli_get_command_string(args, 0), \
                  sl_cli_get_command_string(args, 0), \
                  sl_cli_get_command_string(args, 0));
}

#if defined(_SILICON_LABS_32B_SERIES_2)
static void sl_deinit_prs_channel(int8_t *channel)
{
  if (*channel != INVALID_PRS_CHANNEL) {
    PRS_ConnectSignal(*channel, prsTypeAsync, prsSignalNone);
  }
  *channel = INVALID_PRS_CHANNEL;
}
#endif

static void sl_stop_monitor(void)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  // Stop all LDMA transfers and deallocate the channels, but everything else
  // can be left as is. Leave clocks running, in case they are needed elsewhere
  LDMA_StopTransfer(dma_ch0);
  LDMA_StopTransfer(dma_ch1);
  // Deinit PRS channels
  sl_deinit_prs_channel(&monitor_prs_free_channel_0);
  sl_deinit_prs_channel(&monitor_prs_free_channel_1);
  sl_deinit_prs_channel(&monitor_prs_free_channel_2);
#else
  sl_hal_ldma_stop_transfer(LDMA0, dma_ch0);
  sl_hal_ldma_stop_transfer(LDMA0, dma_ch1);
#endif

#if RAIL_DMA_CHANNEL == DMA_CHANNEL_DMADRV
  DMADRV_FreeChannel(dma_ch0);
  DMADRV_FreeChannel(dma_ch1);
#endif
}

void monitorGpio(sl_cli_command_arg_t *args)
{
  if ((sl_cli_get_argument_count(args) < 1)
      || (strcasecmp(sl_cli_get_argument_string(args, 0), "help") == 0)) {
    sl_print_monitor_help(args);
    return;
  }

  if (strcasecmp(sl_cli_get_argument_string(args, 0), "init") == 0) {
    if (sl_cli_get_argument_count(args) < 3) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x53,
                         "Invalid parameters for 'init' - Need GPIO port and pin");
      return;
    } else if (sl_monitor_state == SL_PIN_MONITOR || sl_monitor_state == SL_PORT_MONITOR) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x53,
                         "Gpio monitor is already running - call stop first");
      return;
    }

    sl_monitor_state = sl_init_pin_monitor(
      strtoul(sl_cli_get_argument_string(args, 1), NULL, 0),
      strtoul(sl_cli_get_argument_string(args, 2), NULL, 0));

    if (sl_monitor_state == SL_PIN_MONITOR) {
      responsePrint(sl_cli_get_command_string(args, 0), "Status:Initialized");
    } else {
      sl_stop_monitor();
      responsePrintError(sl_cli_get_command_string(args, 0), 0x54,
                         "Error during monitor initialization.");
    }
  } else if (strcasecmp(sl_cli_get_argument_string(args, 0), "portInit") == 0) {
    if (sl_cli_get_argument_count(args) < 3) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x53,
                         "Invalid parameters for 'init' - Need GPIO port and pin");
      return;
    } else if (sl_monitor_state == SL_PIN_MONITOR || sl_monitor_state == SL_PORT_MONITOR) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x53,
                         "Gpio monitor is already running - call stop first");
      return;
    }
    sl_monitor_state = sl_init_port_monitor(
      strtoul(sl_cli_get_argument_string(args, 1), NULL, 0),
      strtoul(sl_cli_get_argument_string(args, 2), NULL, 0));

    if (sl_monitor_state == SL_PORT_MONITOR) {
      responsePrint(sl_cli_get_command_string(args, 0), "Status:Initialized");
    } else {
      sl_stop_monitor();
      responsePrintError(sl_cli_get_command_string(args, 0), 0x54,
                         "Error during monitor initialization.");
    }
  } else if (strcasecmp(sl_cli_get_argument_string(args, 0), "get") == 0) {
    sl_print_monitor_data(args);
  } else if (strcasecmp(sl_cli_get_argument_string(args, 0), "stop") == 0) {
    if (sl_monitor_state == SL_UNINITIALIZED) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x53,
                         "The monitor is not initialized. Call 'init' before '%s'",
                         sl_cli_get_argument_string(args, 0));
      return;
    }
    sl_stop_monitor();

    sl_monitor_state = SL_UNINITIALIZED;
    responsePrint(sl_cli_get_command_string(args, 0), "Status:Stopped");
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x53,
                       "%s is invalid option.",
                       sl_cli_get_argument_string(args, 0));
    sl_print_monitor_help(args);
  }
}
