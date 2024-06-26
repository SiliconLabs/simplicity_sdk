/***************************************************************************//**
 * @file
 * @brief This plugin provides support for registering a Unix file descriptor with
 * a callback.  This allows this code to call select() on a set of file
 * descriptors with knowledge of exactly what functions to call when
 * the file descriptor is ready for reading or writing.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "file-descriptor-dispatch.h"
#include "platform/service/cli/inc/sl_cli_threaded_host.h"

#include <stdlib.h>     // for malloc()
#include <sys/time.h>   // for select()
#include <sys/select.h> // for fd_set
#include <sys/types.h>  // ""
#define __USE_MISC
#include <unistd.h>     // ""
#include <errno.h>      // ""

//=============================================================================
// Globals

struct LinkListItemStruct {
  sl_zigbee_af_file_descriptor_dispatch_struct_t dispatchStruct;
  struct LinkListItemStruct* next;
  struct LinkListItemStruct* prev;
  bool markedForRemoval;
  bool badFd;
};
typedef struct LinkListItemStruct sli_zigbee_link_list_item_t;

static bool executingCallbacks = false;

static sli_zigbee_link_list_item_t* theList = NULL;

static uint8_t cliLoopCount = 0;
const char sli_zigbee_af_file_descriptor_dispatch_plugin_name[] = "FD Dispatch";
#define PLUGIN_NAME sli_zigbee_af_file_descriptor_dispatch_plugin_name

//#define DEBUG_ON
#if defined(DEBUG_ON)
#define debugPrint(...) sl_zigbee_af_core_println(__VA_ARGS__)
#else
#define debugPrint(...)
#endif

#ifdef SL_ZIGBEE_TEST
// Simulation apps are still using platform/base
// instead of platform/service
#define sli_cli_get_pipe_read_fd() (-1)
#define sli_cli_is_input_handled() (false)
#endif // SL_ZIGBEE_TEST

//=============================================================================
// Forward Declarations

static sli_zigbee_link_list_item_t* findDispatchByFileDescriptor(int fileDescriptor);
sli_zigbee_link_list_item_t* addItem(sli_zigbee_link_list_item_t* previous,
                                     sl_zigbee_af_file_descriptor_dispatch_struct_t* dispatchStruct);
static void scanForBadFds(void);
//=============================================================================

sl_status_t sl_zigbee_af_file_descriptor_dispatch_add(sl_zigbee_af_file_descriptor_dispatch_struct_t* dispatchStruct)
{
  if (NULL != findDispatchByFileDescriptor(dispatchStruct->fileDescriptor)) {
    sl_zigbee_af_core_println("%p dispatch for fileDescriptor %d already exists",
                              PLUGIN_NAME,
                              dispatchStruct->fileDescriptor);
    return SL_STATUS_INVALID_STATE;
  }

  if ((dispatchStruct->operation == SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_NONE)
      || (dispatchStruct->operation >= SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_MAX)) {
    sl_zigbee_af_core_println("Error: %p bad file descriptor operation of %d", dispatchStruct->operation);
    return SL_STATUS_INVALID_STATE;
  }

  sli_zigbee_link_list_item_t* iterator = theList;

  while (iterator != NULL && iterator->next != NULL) {
    iterator = iterator->next;
  }

  iterator = addItem(iterator, dispatchStruct);
  if (NULL == iterator) {
    return SL_STATUS_FAIL;
  }

  if (theList == NULL) {
    theList = iterator;
  }
  return SL_STATUS_OK;
}

#if defined(DEBUG_ON)
static void debugPrintList(void)
{
  sli_zigbee_link_list_item_t* iterator = theList;
  sl_zigbee_af_core_print("Current FD List:");
  while (iterator != NULL) {
    sl_zigbee_af_core_print("%d ", iterator->dispatchStruct.fileDescriptor);
    iterator = iterator->next;
    if (iterator != NULL) {
      sl_zigbee_af_core_print("-> ");
    }
  }
  sl_zigbee_af_core_println("");
}
#else
#define debugPrintList()
#endif

sli_zigbee_link_list_item_t* addItem(sli_zigbee_link_list_item_t* previous, sl_zigbee_af_file_descriptor_dispatch_struct_t* dispatchStruct)
{
  sli_zigbee_link_list_item_t* new = malloc(sizeof(sli_zigbee_link_list_item_t));
  if (new == NULL) {
    sl_zigbee_af_core_println("Error: %p failed to allocate memory for dispatch list element.",
                              PLUGIN_NAME);
    return NULL;
  }

  debugPrint("%p Previous FD in list is %d",
             PLUGIN_NAME,
             (previous == NULL
              ? -1
              : previous->dispatchStruct.fileDescriptor));

  memmove(&(new->dispatchStruct), dispatchStruct, sizeof(sl_zigbee_af_file_descriptor_dispatch_struct_t));
  new->markedForRemoval = false;
  new->badFd = false;
  new->next = NULL;
  new->prev = previous;
  if (previous) {
    previous->next = new;
  }
  debugPrint("%p added dispatch for FD %d", PLUGIN_NAME, new->dispatchStruct.fileDescriptor);
  debugPrintList();
  return new;
}

static sli_zigbee_link_list_item_t* findDispatchByFileDescriptor(int fileDescriptor)
{
  sli_zigbee_link_list_item_t* iterator = theList;
  while (iterator != NULL) {
    if (iterator->dispatchStruct.fileDescriptor == fileDescriptor) {
      return iterator;
    }
    iterator = iterator->next;
  }

  return NULL;
}

static void removeItem(sli_zigbee_link_list_item_t* toRemove)
{
  if (toRemove == theList) {
    theList = toRemove->next;
  } else if (toRemove->prev != NULL) {
    toRemove->prev->next = toRemove->next;
  }
  if (toRemove->next != NULL) {
    toRemove->next->prev = toRemove->prev;
  }

  debugPrint("%p removed dispatch for FD %d", PLUGIN_NAME, toRemove->dispatchStruct.fileDescriptor);
  free(toRemove);
  debugPrintList();
}

bool sl_zigbee_af_file_descriptor_dispatch_remove(int fileDescriptor)
{
  sli_zigbee_link_list_item_t* toRemove = findDispatchByFileDescriptor(fileDescriptor);

  if (toRemove == NULL) {
    sl_zigbee_af_core_println("Error: %p could not find fileDescriptor %d to remove dispatch.",
                              PLUGIN_NAME,
                              fileDescriptor);
    return false;
  }

  // We must prevent modification of the list while we are iterating through it.
  // Instead mark the item for removal at a later date.
  if (executingCallbacks) {
    toRemove->markedForRemoval = true;
    debugPrint("%p marked FD %d for removal later.", PLUGIN_NAME, fileDescriptor);
    return true;
  }

  removeItem(toRemove);
  return true;
}

static void cleanupItemsMarkedForRemoval(void)
{
  sli_zigbee_link_list_item_t* iterator = theList;
  while (iterator != NULL) {
    if (iterator->markedForRemoval == true) {
      removeItem(iterator);
    }
    iterator = iterator->next;
  }
}

static void setCliInFd(int *maxFd, fd_set *readSet, fd_set *errorSet)
{
  int pipeReadFd = sli_cli_get_pipe_read_fd();
  FD_SET(pipeReadFd, readSet);
  FD_SET(pipeReadFd, errorSet);
  *maxFd = (*maxFd < pipeReadFd ? pipeReadFd : *maxFd);
}

#ifdef EZSP_CPC
extern int sli_zigbee_get_cpc_fd(void);
static void setCpcFd(int *maxFd, fd_set *readSet, fd_set *errorSet)
{
  int cpcFd = sli_zigbee_get_cpc_fd();
  FD_SET(cpcFd, readSet);
  FD_SET(cpcFd, errorSet);
  *maxFd = (*maxFd < cpcFd ? cpcFd : *maxFd);
}
#endif // EZSP_CPC

sl_status_t sl_zigbee_af_file_descriptor_dispatch_wait_for_events(uint32_t timeoutMs)
{
  fd_set readSet;
  fd_set writeSet;
  fd_set exceptSet;
  // Set to -2 so highestFd + 1 = -1, which is still an invalid value
  int highestFd = -2;
  sli_zigbee_link_list_item_t* iterator = theList;

  static bool firstRun = true;
  if (firstRun) {
    firstRun = false;
    debugPrint("%p first run, not waiting for data.", PLUGIN_NAME);
    return SL_STATUS_OK;
  }

  // If timeoutMs is 0, then don't bother calling select.
  // Just return here. Bug: EMAPPFWKV2-1196.
  if (timeoutMs == 0) {
    return SL_STATUS_OK;
  }

  FD_ZERO(&readSet);
  FD_ZERO(&writeSet);
  FD_ZERO(&exceptSet);

  while (iterator != NULL && iterator->badFd == false) {
    if (iterator->dispatchStruct.operation == SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_READ) {
      debugPrint("%p added read FD %d",
                 PLUGIN_NAME,
                 iterator->dispatchStruct.fileDescriptor);
      FD_SET(iterator->dispatchStruct.fileDescriptor,
             &readSet);
    }
    if (iterator->dispatchStruct.operation == SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_WRITE) {
      debugPrint("%p added write FD %d",
                 PLUGIN_NAME,
                 iterator->dispatchStruct.fileDescriptor);
      FD_SET(iterator->dispatchStruct.fileDescriptor,
             &writeSet);
    }
    if (iterator->dispatchStruct.operation == SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_EXCEPT) {
      debugPrint("%p added except FD %d",
                 PLUGIN_NAME,
                 iterator->dispatchStruct.fileDescriptor);
      FD_SET(iterator->dispatchStruct.fileDescriptor,
             &exceptSet);
    }
    if (iterator->dispatchStruct.fileDescriptor > highestFd) {
      highestFd = iterator->dispatchStruct.fileDescriptor;
    }
    iterator = iterator->next;
  }

  // Set CLI pipe read fd so that we can wake up the host
  // by entering CLI commands.
  setCliInFd(&highestFd, &readSet, &exceptSet);
#ifdef EZSP_CPC
  // Set CPC fd so that we can wake up the host when CPC deamon
  // has packets for the host instead of waiting the tick.
  setCpcFd(&highestFd, &readSet, &exceptSet);
#endif // EZSP_CPC

  struct timeval timeoutStruct = {
    timeoutMs / 1000,           // seconds
    (timeoutMs % 1000) * 1000,  // micro seconds
  };

  debugPrint("%p select() called, highestFd %d, timeout %d ms", PLUGIN_NAME, highestFd, timeoutMs);

  int status = 0;
  int cliReadFd = sli_cli_get_pipe_read_fd();
  if (highestFd >= 0) {
    status = select(highestFd + 1,   // per select() manpage
                    &readSet,
                    &writeSet,
                    &exceptSet,
                    (timeoutMs != MAX_INT32U_VALUE
                     ? &timeoutStruct
                     : NULL));
    cliLoopCount++;
    // If the command is handled by the CLI component,
    // or if the data is available to read on the CLI pipe read fd but is not handled
    // in the CLI component for an amount of time,
    // read the data to empty the pipe so that it is ready for the next command.
    if (sli_cli_is_input_handled() || (status > 0 && cliLoopCount == MAX_INT8U_VALUE && FD_ISSET(cliReadFd, &readSet))) {
      static char buff[SL_CLI_THREADED_HOST_PIPE_DATA_LENGTH];
      assert(SL_CLI_THREADED_HOST_PIPE_DATA_LENGTH
             == read(sli_cli_get_pipe_read_fd(), buff, SL_CLI_THREADED_HOST_PIPE_DATA_LENGTH));
      cliLoopCount = 0;
    }
  }
  if (status < 0 && errno != EINTR) {
    sl_zigbee_af_core_println("%p select() failed: %p", PLUGIN_NAME, strerror(errno));

    // EMZIGBEE-2126 Try to isolate the bad file descriptor,
    if (errno == EBADF) {
      // Individually scan each FD and mark any that are in error to exclude
      // from further select polls.
      scanForBadFds();
    }
  }

  if (status < 0 || highestFd < 0) {
    // For error status or no descriptors to be polled, impose the intended
    // select timeout delay to prevent CPU spin on repeated select error.
    if (timeoutMs != MAX_INT32U_VALUE) {
      usleep((long)(timeoutMs * 1000));
    } else {
      sleep(1);
    }

    return SL_STATUS_OK;
  }

  debugPrint("%p select() returned %d", PLUGIN_NAME, status);

  if (status == 0) {
    // timeout, no FDs set.
    return SL_STATUS_OK;
  }

  iterator = theList;
  executingCallbacks = true;
  while (iterator != NULL) {
    if (iterator->dispatchStruct.callback != NULL) {
      // NOTE: We must check markedForRemoval/badFd before each callback since a
      // previous callback may have modified the state
      if (!iterator->markedForRemoval && !iterator->badFd) {
        if (FD_ISSET(iterator->dispatchStruct.fileDescriptor, &readSet)) {
          (iterator->dispatchStruct.callback)(iterator->dispatchStruct.dataPassedToCallback,
                                              SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_READ);
        }
        if (FD_ISSET(iterator->dispatchStruct.fileDescriptor, &writeSet)) {
          (iterator->dispatchStruct.callback)(iterator->dispatchStruct.dataPassedToCallback,
                                              SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_WRITE);
        }
        if (FD_ISSET(iterator->dispatchStruct.fileDescriptor, &exceptSet)) {
          (iterator->dispatchStruct.callback)(iterator->dispatchStruct.dataPassedToCallback,
                                              SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_EXCEPT);
        }
      }
    }
    iterator = iterator->next;
  }
  executingCallbacks = false;

  cleanupItemsMarkedForRemoval();

  return SL_STATUS_OK;
}

// If above select() returns a EBADF error, scan each file descriptor to
// identify and exclude the faulty file descriptor from further select polling.
// Invoke callback to notify application and possibly prompt recovery.
static void scanForBadFds(void)
{
  fd_set readSet;
  fd_set writeSet;
  fd_set exceptSet;
  sli_zigbee_link_list_item_t* iterator = theList;

  while (iterator != NULL && iterator->badFd == false) {
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_ZERO(&exceptSet);

    if (iterator->dispatchStruct.operation == SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_READ) {
      FD_SET(iterator->dispatchStruct.fileDescriptor, &readSet);
    }
    if (iterator->dispatchStruct.operation == SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_WRITE) {
      FD_SET(iterator->dispatchStruct.fileDescriptor, &writeSet);
    }
    if (iterator->dispatchStruct.operation == SL_ZIGBEE_AF_FILE_DESCRIPTOR_OPERATION_EXCEPT) {
      FD_SET(iterator->dispatchStruct.fileDescriptor, &exceptSet);
    }

    struct timeval zeroTime = {
      0, // seconds
      0, // micro seconds
    };

    int status = select(iterator->dispatchStruct.fileDescriptor + 1,
                        &readSet,
                        &writeSet,
                        &exceptSet,
                        &zeroTime);

    if (status < 0) {
      if (errno == EBADF) {
        sl_zigbee_af_core_println("%p bad file descriptor: %d", PLUGIN_NAME,
                                  iterator->dispatchStruct.fileDescriptor);
        sl_zigbee_af_file_descriptor_dispatch_bad_file_descriptor_cb(iterator->dispatchStruct.fileDescriptor);
        iterator->badFd = true;
      }
    }

    iterator = iterator->next;
  }
}
