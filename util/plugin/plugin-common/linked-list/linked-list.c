// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include "linked-list.h"
#include <stdlib.h>

EmberAfPluginLinkedList* sl_util_af_linked_list_init(void)
{
  EmberAfPluginLinkedList* list =
    (EmberAfPluginLinkedList*)malloc(sizeof(EmberAfPluginLinkedList));
  if (list != NULL) {
    memset(list, 0, sizeof(EmberAfPluginLinkedList));
  }
  return list;
}

void sl_util_af_linked_list_deinit(EmberAfPluginLinkedList* list)
{
  while (list->count > 0) {
    sl_util_af_linked_list_pop_front(list);
  }
  free(list);
}

void sl_util_af_linked_list_push_back(
  EmberAfPluginLinkedList* list,
  void* content)
{
  EmberAfPluginLinkedListElement* element =
    (EmberAfPluginLinkedListElement*)malloc(sizeof(
                                              EmberAfPluginLinkedListElement));
  if (element != NULL) {
    element->content = content;
    element->next = NULL;
    element->previous = list->tail;
    if (list->head == NULL) {
      list->head = element;
    } else {
      list->tail->next = element;
    }
    list->tail = element;
    ++(list->count);
  }
}

void sl_util_af_linked_list_pop_front(EmberAfPluginLinkedList* list)
{
  if (list->count > 0) {
    EmberAfPluginLinkedListElement* head = list->head;
    if (list->tail == head) {
      list->tail = NULL;
    }
    list->head = list->head->next;
    free(head);
    --(list->count);
  }
}

bool sl_util_af_linked_list_remove_element(
  EmberAfPluginLinkedList* list,
  EmberAfPluginLinkedListElement* element)
{
  if ((element != NULL) && (list->head != NULL)) {
    if (element == list->head) {
      if (list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
      } else {
        list->head = element->next;
        element->next->previous = NULL;
      }
    } else if (element == list->tail) {
      list->tail = element->previous;
      element->previous->next = NULL;
    } else {
      element->previous->next = element->next;
      element->next->previous = element->previous;
    }
    --(list->count);
    free(element);
    return true;
  }
  return false;
}

bool sl_util_af_linked_list_clear_all_elements(EmberAfPluginLinkedList* list)
{
  while (list->head != NULL) {
    sl_util_af_linked_list_pop_front(list);
  }
  return true;
}

EmberAfPluginLinkedListElement* sl_util_af_linked_list_next_element(
  EmberAfPluginLinkedList* list,
  EmberAfPluginLinkedListElement* elementPosition)
{
  if (elementPosition == NULL) {
    return list->head;
  } else {
    return elementPosition->next;
  }
}
