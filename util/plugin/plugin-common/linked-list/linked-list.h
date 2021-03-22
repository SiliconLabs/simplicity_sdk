// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

typedef struct _EmberAfPluginLinkedListElement {
  struct _EmberAfPluginLinkedListElement* next;
  struct _EmberAfPluginLinkedListElement* previous;
  void* content;
} EmberAfPluginLinkedListElement;

typedef struct {
  EmberAfPluginLinkedListElement* head;
  EmberAfPluginLinkedListElement* tail;
  uint32_t count;
} EmberAfPluginLinkedList;

/** @brief Linked List Init
 *
 * This function will return an initialized and empty LinkedList object.  This
 * makes use of `malloc()` and can be freed when the
 * `sl_util_af_linked_list_deinit()`
 * function is called.
 */
EmberAfPluginLinkedList* sl_util_af_linked_list_init(void);

/** @brief Linked List Deinit
 *
 * This function will empty and free a LinkedList object.
 *
 * @param list Pointer to the list to clear and free
 */
void sl_util_af_linked_list_deinit(EmberAfPluginLinkedList* list);

/** @brief Linked List Push Back
 *
 * This function will push an element on to the back of a list.  This
 * makes use of `malloc()` and will call the corresponding `free()` the pop
 * function is called.
 *
 * @param list Pointer to the list to push the element to
 * @param content void pointer to an object or value to push to back of the list
 */
void sl_util_af_linked_list_push_back(
  EmberAfPluginLinkedList* list,
  void* content);

/** @brief Linked List Pop Front
 *
 * This function will pop an element off of the front of a list. Note that this
 * only free's the element, not the element's content. The caller of this
 * must free the content held by the element before popping and freeing the
 * element itself to avoid memory leaks.
 *
 * @param list Pointer to the list to pop an element from
 */
void sl_util_af_linked_list_pop_front(EmberAfPluginLinkedList* list);

/** @brief Linked List Remove Element
 *
 * This function will remove an element from a list. The caller of this must
 * free the content held by the element before removing and freeing the element
 * itself to avoid memory leaks.
 *
 * @param list Pointer to the list to pop an element from
 * @param elementPosition Pointer to the list element to remove from the list
 */
bool sl_util_af_linked_list_remove_element(
  EmberAfPluginLinkedList* list,
  EmberAfPluginLinkedListElement* elementPosition);

/** @brief Linked List Clear All Elements
 *
 * This function will remove all elements from a list. The caller of this must
 * free the content held by each element before clearing and freeing the
 * elements themselves to avoid memory leaks.
 *
 * @param list Pointer to the list to pop an element from
 */
bool sl_util_af_linked_list_clear_all_elements(EmberAfPluginLinkedList* list);

/** @brief Linked List Next Element
 *
 * This function returns a pointer to the next element to the provided element.
 * If the provided element is NULL, it will return the head item on the list.
 *
 * @param list Pointer to the list to get the next element from
 * @param elementPosition Pointer to the list element to get the next item from
 */
EmberAfPluginLinkedListElement* sl_util_af_linked_list_next_element(
  EmberAfPluginLinkedList* list,
  EmberAfPluginLinkedListElement* elementPosition);

#endif // __LINKED_LIST_H
