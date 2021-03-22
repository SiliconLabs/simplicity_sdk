/***************************************************************************//**
 * @file
 * @brief Event System API for Inter-process Communication.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_event_system.h"
#include "sl_memory_manager.h"
#include "sl_assert.h"
#include "sl_core.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool is_event_system_initialized = false;
static sl_slist_node_t *publishers;

/*******************************************************************************
 * @brief
 *  Initialize the event system.
 ******************************************************************************/
void sl_event_system_init(void)
{
  sl_slist_init(&publishers);
  is_event_system_initialized = true;
}

/*******************************************************************************
 * @brief
 *  Initialize a publisher context and register it in the event system
 *  with a given event class.
 ******************************************************************************/
sl_status_t sl_event_publisher_register(sl_event_publisher_t *publisher,
                                        sl_event_class_t event_class,
                                        sl_event_free_data_cb_t free_data_callback)
{
  // Parameter validation.
  if ( publisher == NULL
       || event_class >= SL_EVENT_CLASS_MAX
       || free_data_callback == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Only register publishers if the event system has been initialized
  EFM_ASSERT(is_event_system_initialized == true);

  // Initialize publisher context
  publisher->event_class = event_class;
  publisher->free_data_callback = free_data_callback;
  publisher->subscriber_count = 0;
  sl_slist_init(&publisher->subscribers);

  // Register publisher context with the given event class.
  sl_slist_push(&publishers, &publisher->node);

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief
 *  Publish an event, with data, within the event class of the publisher.
 ******************************************************************************/
sl_status_t sl_event_publish(sl_event_publisher_t *publisher,
                             uint32_t event_mask,
                             uint8_t event_prio,
                             void *event_data)
{
  // Implementation doesn't currently use message priorities.
  (void) event_prio;
  sl_status_t status = SL_STATUS_OK;
  sl_event_t            *event;
  sl_event_t            *temp;
  sl_event_subscriber_t *subscriber;

  // The event data cannot be NULL.
  if ( publisher == NULL
       || event_mask == 0
       || event_data == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Event context is allocated.
  status = sl_memory_alloc(sizeof(sl_event_t),
                           BLOCK_TYPE_SHORT_TERM,
                           (void **)&event);
  if ( status != SL_STATUS_OK ) {
    return status;
  }

  // Initialize event context
  event->publisher = publisher;
  event->event_data = event_data;
  event->reference_count = publisher->subscriber_count;

  // Publish event to subscriber event queues.
  SL_SLIST_FOR_EACH_ENTRY(publisher->subscribers, subscriber, sl_event_subscriber_t, node) {
    // Using a temp object since sl_event_process() nulls the event.
    temp = event;
    if ((event_mask & subscriber->event_mask) != 0) {
      // Implementation doesn't currently use message priorities.
      if (osOK != osMessageQueuePut(subscriber->event_queue, &event, 0, 0)) {
        sl_event_process(&temp);
        status = SL_STATUS_FAIL;
      }
    } else {
      // The subscriber does not listen to this event, let's consider it
      // processed.
      sl_event_process(&temp);
    }
  }

  return status;
}

/*******************************************************************************
 * @brief
 *  Subscribe to one or more event publishers which are publishers for a given
 *  event class. Subscribed events are placed in the queue provided as a
 *  parameter to this function call.
 ******************************************************************************/
sl_status_t sl_event_subscribe(sl_event_class_t event_class,
                               uint32_t event_mask,
                               sl_event_queue_t event_queue)
{
  sl_status_t status = SL_STATUS_OK;
  sl_event_subscriber_t *subscriber;
  sl_event_publisher_t *publisher;

  // Input validation
  if (event_class >= SL_EVENT_CLASS_MAX
      || event_mask == 0
      || event_queue == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Only register publishers if the event system has been initialized
  EFM_ASSERT(is_event_system_initialized == true);

  // Register subscription in the publisher context
  bool is_publisher_found = false;
  SL_SLIST_FOR_EACH_ENTRY(publishers, publisher, sl_event_publisher_t, node) {
    if ( event_class == publisher->event_class ) {
      is_publisher_found = true;

      // Allocate memory for the publisher's subscriber list entry.
      status = sl_memory_alloc(sizeof(sl_event_subscriber_t),
                               BLOCK_TYPE_LONG_TERM,
                               (void **)&subscriber);
      if ( status != SL_STATUS_OK ) {
        break;
      }
      subscriber->event_mask = event_mask;
      subscriber->event_queue = event_queue;

      CORE_DECLARE_IRQ_STATE;
      CORE_ENTER_ATOMIC();
      EFM_ASSERT(publisher->subscriber_count != UINT8_MAX);
      sl_slist_push(&publisher->subscribers, &subscriber->node);
      publisher->subscriber_count++;
      CORE_EXIT_ATOMIC();
    }
  }

  if ( !is_publisher_found ) {
    return SL_STATUS_NOT_FOUND;
  }

  return status;
}

/*******************************************************************************
 * @brief
 *  Signals to the event system that a subscriber has processed an event. This
 *  must be called by subscribers after consuming an event so that the event
 *  data may eventually be freed. The event reference passed to this function
 *  is nullified before returning.
 ******************************************************************************/
sl_status_t sl_event_process(sl_event_t **event)
{
  sl_status_t status = SL_STATUS_OK;
  // The event reference cannot be null
  if (*event == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Decrement reference count of event, and call the publisher's free
  // data callback if the number of references is 0.
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  (*event)->reference_count--;
  if ((*event)->reference_count == 0) {
    CORE_EXIT_ATOMIC();
    (*event)->publisher->free_data_callback((*event)->event_data);
    sl_memory_free((void*)(*event));
  } else {
    CORE_EXIT_ATOMIC();
  }

  // Nullify the event reference held by the callee.
  *event = NULL;

  return status;
}

/*******************************************************************************
 * @brief
 *  Create an event queue.
 ******************************************************************************/
sl_status_t sl_event_queue_create(uint32_t event_count,
                                  sl_event_queue_t *event_queue)
{
  *event_queue = osMessageQueueNew(event_count, sizeof(sl_event_t*), NULL);
  if (*event_queue != NULL) {
    return SL_STATUS_OK;
  }

  return SL_STATUS_FAIL;
}

/*******************************************************************************
 * @brief
 *  Get an event from an event queue.
 ******************************************************************************/
sl_status_t sl_event_queue_get(sl_event_queue_t event_queue,
                               uint8_t *event_prio,
                               uint32_t timeout,
                               sl_event_t **event)
{
  // Implementation doesn't currently use message priorities.
  (void) event_prio;
  sl_status_t status;
  osStatus_t os_status;

  // Wait to receive an event in the event queue for the specified timeout time.
  os_status = osMessageQueueGet(event_queue, event, NULL, timeout);
  switch (os_status) {
    case osOK:
      status = SL_STATUS_OK;
      break;
    case osErrorResource:
      status = SL_STATUS_EMPTY;
      break;
    case osErrorParameter:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    default:
      status = SL_STATUS_FAIL;
      break;
  }

  return status;
}

/*******************************************************************************
 * @brief
 *  Get the size of the event publisher structure.
 ******************************************************************************/
size_t sl_event_publisher_get_size(void)
{
  return sizeof(sl_event_publisher_t);
}

/*******************************************************************************
 * @brief
 *  Allocate the publisher structure to the heap using the common memory
 *  manager with a long-term lifespan.
 ******************************************************************************/
sl_status_t sl_event_publisher_alloc(sl_event_publisher_t **publisher)
{
  return sl_memory_alloc(sizeof(sl_event_publisher_t),
                         BLOCK_TYPE_LONG_TERM,
                         (void**)publisher);
}

#ifdef __cplusplus
}
#endif
