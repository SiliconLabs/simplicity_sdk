/***************************************************************************//**
 * @file
 * @brief  Simulate serial I/O for linux EZSP UART
 *          applications.
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

#define _GNU_SOURCE 1  // This turns on certain non-standard GNU extensions
// to the C library (i.e. strnlen()).
// Include before PLATFORM_HEADER since that also
// includes 'string.h'

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "stack/include/sl_zigbee_types.h"
#include <stdio.h>      /* printf */
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

#include "hal/hal.h"
#include "serial/serial.h"
#include "platform/service/legacy_printf/inc/sl_legacy_printf.h"
sl_status_t sli_legacy_serial_write_data(uint8_t port, uint8_t *data, uint8_t length);
#define NO_READLINE // disable readline in UC
//#include "serial/ember-printf.h"
#include "cli.h"

#include <sys/types.h>         // for fstat()
#include <sys/select.h>        // for fd_set
#include <sys/stat.h>          // ""
#include <fcntl.h>             // for fcntl()
#include <stdlib.h>
#include <unistd.h>            // for pipe(), fork()

#if defined NO_READLINE
  #define READLINE_SUPPORT 0
#else
  #define READLINE_SUPPORT 1
  #include <readline/readline.h> // for readline()
  #include <readline/history.h>  // ""

#endif

#include <signal.h>            // for trapping SIGTERM
#include <errno.h>             // for strerror() and errno
#include <stdarg.h>            // for vfprintf()
#include <sys/wait.h>          // for wait()

#if defined(EZSP_ASH)
// All needed For ashSerialGetFd()
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/ezsp-host/ash/ash-host.h"
  #include "app/ezsp-host/ezsp-host-io.h"
  #include "app/ezsp-host/ash/ash-host-ui.h"
#endif

#include "linux-serial.h"

#include "ember-printf-convert.h"

// Don't like readline and the GPL requirements?  Use 'libedit'.
// It is a call-for-call compatible with the readline library but is
// released under the BSD license.
//   http://www.thrysoee.dk/editline/

//------------------------------------------------------------------------------
// Globals

#define NUM_PORTS              2
#define INVALID_FD             -1
#define INVALID_PID            0
#define MAX_PROMPT_LENGTH      20
#define MAX_NUMBER_OF_COMMANDS 500
#define LINE_FEED              0x0A
#define EOF_CHAR               0x04
#define MAX_STRING_LENGTH      250  // arbitrary limit
#define SL_ZIGBEE_COMMAND_BUFFER_LENGTH 100

static int STDIN = 0;

// The first index is the port, the second
// is the pipe number.  The first two are the data pipes,
// the second two are the control pipes.
static int pipeFileDescriptors[NUM_PORTS][4] = {
  { INVALID_FD, INVALID_FD, INVALID_FD, INVALID_FD },
  { INVALID_FD, INVALID_FD, INVALID_FD, INVALID_FD }
};
#define DATA_READER(__port) pipeFileDescriptors[(__port)][0]
#define DATA_WRITER(__port) pipeFileDescriptors[(__port)][1]
#define CONTROL_READER(__port) pipeFileDescriptors[(__port)][2]
#define CONTROL_WRITER(__port) pipeFileDescriptors[(__port)][3]

static bool amParent = true;
static bool useControlChannel = true;
static bool debugOn = false;
static char prompt[MAX_PROMPT_LENGTH];
static pid_t childPid[NUM_PORTS] = { INVALID_PID, INVALID_PID };

static bool usingCommandInterpreter = false;

#if READLINE_SUPPORT

// for readline() command completion
static const sl_zigbee_command_entry_t* allCommands = NULL;

#ifdef __APPLE__
#define strnlen(string, n) strlen((string))
#define rl_completion_matches(a, b) completion_matches((a), (b))
#define strsignal(x) NULL   // This is a _GNU_SOURCE extension.
#endif

// This will be prefixed with $HOME
static const char readlineHistoryFilename[] = ".linux-serial.history";
static char readlineHistoryPath[MAX_STRING_LENGTH];

#endif // READLINE_SUPPORT

static int childProcessPort = -1;
static bool debugChildProcess = 0;

//------------------------------------------------------------------------------
// Forward Declarations

static sl_status_t serialInitInternal(uint8_t port);
static void childRun(uint8_t port);
static void setNonBlockingFD(int fd);
static void debugPrint(const char* formatString, ...);
static void processSerialInput(uint8_t port);

#if READLINE_SUPPORT
static char** commandCompletion(const char *text, int start, int end);
static char* singleCommandCompletion(const char *text, int start);
static char* filenameCompletion(const char* text, int state);
static void initializeHistory(void);
static void writeHistory(void);
static char* duplicateString(const char* source);
#else  // use stubs
  #define initializeHistory()
  #define writeHistory()
  #define add_history(x)
#endif

static void parentCleanupAfterChildDied(void);
static void installSignalHandler(void);
static void childCleanupAndExit(void);

static bool sendGoAhead = true;

//------------------------------------------------------------------------------
// Initialization Functions

// In order to handle input more effeciently and cleanly,
// we use readline().  This is a blocking call, so we create a second
// process to handle the serial input while the main one can simply
// loop executing sl_zigbee_ezsp_tick() and other functionality.

sl_status_t sli_legacy_serial_init(uint8_t port,
                                   SerialBaudRate rate,
                                   SerialParity parity,
                                   uint8_t stopBits)
{
  (void) rate;
  (void) parity;
  (void) stopBits;
  static bool sl_zigbee_serial_init_called = false;

  debugPrint("sli_legacy_serial_init()\n");
  if (port > 1) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (childPid[port] != INVALID_PID) {
    debugPrint("Serial port %d already initialized.\n", port);
    return SL_STATUS_OK;
  }

  // there is only one serial port available (STDIN).
  if (sl_zigbee_serial_init_called) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status = serialInitInternal(port);

  if (status == SL_STATUS_OK) {
    installSignalHandler();
    sl_zigbee_serial_init_called = true;
  }

  return status;
}

static sl_status_t serialInitInternal(uint8_t port)
{
  // Create two pipes, control and data.
  // Fork
  // Parent closes the write end of the data pipe and read end of the control,
  //   child closes the read end of the data pipe and write end of the control.
  // Child sits and waits for input via readline() (or read())
  //   When it receives input it writes it to the data pipe.  It then blocks
  //   on a read call to the control pipe (if it is running the CLI).
  // Parent loops normally, calling sli_legacy_serial_read_byte() or emberSeriaReadLine()
  //   to get the input.  These calls map to reads of the data pipe, the former
  //   would NOT be blocking while the latter would be.
  //   Parent calls indirectly readyForSerialInput() after reading an entire
  //   line of data (via the next sli_legacy_serial_read_byte()).

  int status;
  int i;
  pid_t pid;

  for (i = 0; i < 2; i++) {
    char* string = (i == 0
                    ? "data"
                    : "control");
    int* pipePtr = (i == 0
                    ? &(pipeFileDescriptors[port][0])
                    : &(pipeFileDescriptors[port][2]));
    status = pipe(pipePtr);
    if (status != 0) {
      fprintf(stderr,
              "FATAL: Could not create %s pipe (%d): %s",
              string,
              status,
              strerror(status));
      assert(false);
    }
    debugPrint("Serial Port %d, Created %s Pipe, Reader: %d, Writer %d\n",
               port,
               string,
               pipePtr[0],
               pipePtr[1]);
  }

  // Change the data reader to non-blocking so that we will
  // continue to be able to execute sl_zigbee_ezsp_tick().
  setNonBlockingFD(DATA_READER(port));

  pid = fork();
  if (pid == 0) {  // Child
    amParent = false;
    while (debugChildProcess) {
      // block indefinitely waiting for debugger to attach
      sleep(1);
    }
    childRun(port);
    return SL_STATUS_FAIL;  // should never get here
  } else if (pid == -1) {
    fprintf(stderr, "FATAL: Could not fork! (%d): %s\n",
            errno,
            strerror(errno));
    assert(false);
  }
  // Parent
  debugPrint("fork(): My child's pid is %d\n", pid);

  // BugzId:12928 Parent retains allCommands for other children it might spawn

  close(DATA_WRITER(port));
  close(CONTROL_READER(port));
  DATA_WRITER(port) = INVALID_FD;
  CONTROL_READER(port) = INVALID_FD;

  // BugzId:12928 Leave STDIN open for subsequent forked children to use

  childPid[port] = pid;

  setMicroRebootHandler(&sli_legacy_serial_cleanup);
  return SL_STATUS_OK;
}

static void childRun(uint8_t port)
{
  childProcessPort = port;

  close(DATA_READER(port));
  close(CONTROL_WRITER(port));

  // This fixes a bug on Cygwin where a process that
  // holds onto an open file descriptor after a fork()
  // can prevent the other parent process from performing a
  // close() and re-open() on it.  Specifically this only seems
  // to affect serial port FDs, which is what ASH uses.

#ifdef EZSP_ASH
  if (INVALID_FD != sl_zigbee_ezsp_serial_get_fd()) {
    int port = sl_zigbee_ezsp_serial_get_fd();
    if (0 != close(port)) {
      fprintf(stderr, "FATAL: Could not close EZSP file descriptor %d: %s\n",
              port,
              strerror(errno));
      assert(false);
    }
  }
#endif

  installSignalHandler();
  processSerialInput(port);

  // Normally the above function NEVER returns
  // If we get here it is an error.
  assert(0);
}

static void setNonBlockingFD(int fd)
{
  int flags = fcntl(fd, F_GETFL);
  int status = fcntl(fd, F_SETFD, flags | O_NONBLOCK);
  if (status != 0) {
    fprintf(stderr,
            "FATAL: Could not set pipe reader to non-blocking (%d): %s\n",
            errno,
            strerror(errno));
    assert(false);
  }
}

// It is expected this is only called within the parent process.
void sli_legacy_serial_cleanup(void)
{
  uint8_t port;
  for (port = 0; port < 2; port++) {
    if (childPid[port] != INVALID_PID) {
      int status;
      close(DATA_READER(port));
      close(CONTROL_WRITER(port));
      DATA_READER(port) = INVALID_FD;
      CONTROL_WRITER(port) = INVALID_FD;
      debugPrint("Waiting for child on port %d to terminate.\n", port);
      kill(childPid[port], SIGTERM);
      wait(&status);
      debugPrint("Child on port %d exited with status: %d\n", port, status);
      childPid[port] = INVALID_PID;
    }
  }
}

static void parentCleanupAfterChildDied(void)
{
  // First we need to determine which child died.
  // We assume that a child has really died, otherwise the
  // wait() call will block.
  int status;
  pid_t pid = wait(&status);
  int i;
  int childPort = -1;
  for (i = 0; i < NUM_PORTS; i++) {
    if (childPid[i] == pid) {
      childPort = i;
      break;
    }
  }
  if (childPort == -1) {
    // A child process died that we didn't know about!
    assert(0);
  }
  childPid[childPort] = INVALID_PID;
  close(DATA_READER(childPort));
  close(CONTROL_WRITER(childPort));
  DATA_READER(childPort) = INVALID_FD;
  CONTROL_WRITER(childPort) = INVALID_FD;
}

static void childCleanupAndExit(void)
{
  if (childProcessPort == SERIAL_PORT_CLI) {
    writeHistory();
  }
  if (childProcessPort != -1) {
    close(DATA_WRITER(childProcessPort));
    close(CONTROL_READER(childProcessPort));
  }

  exit(0);
}

// This is for QA's cli
// We no longer support command completion in this case.
void sli_legacy_serial_command_completion_init_cli(sli_zigbee_cli_serial_cmd_entry_t cliCmdList[],
                                                   int cliCmdListLength)
{
#if READLINE_SUPPORT
  allCommands = NULL;
#else
  (void) cliCmdList;
  (void) cliCmdListLength;
#endif
}

//------------------------------------------------------------------------------
// Serial Input

int sli_legacy_serial_get_input_fd(uint8_t port)
{
  if (port > (NUM_PORTS - 1)) {
    return INVALID_FD;
  }
  return DATA_READER(port);
}

static bool readyForSerialInput(uint8_t port)
{
  if (useControlChannel) {
    char data = '1';
    if (1 == write(CONTROL_WRITER(port), &data, 1)) {
      debugPrint("Sent reader process 'go-ahead' signal.\n");
    } else {
      // We assume this function is only used for CLI input.
      // If the CLI process died, then the parent should also
      // go away.
      sli_legacy_serial_cleanup();
      exit(-1);
    }
  }
  return false; // go-ahead delivered ok
}

// returns # bytes available for reading
uint16_t sli_legacy_serial_read_available(uint8_t port)
{
  // I am not aware of a good way to get the exact number
  // of bytes available for reading from an anonymous pipe.
  // We use select() to see if there is any data available for
  // reading and that means there is at least one byte present
  // or EOF (pipe closed) has happened.
  fd_set readSet;
  struct timeval timeout = { 0, 0 }; // return immediately
  int fdsWithData;

  if (DATA_READER(port) == INVALID_FD) {
    return 0;
  }

  FD_ZERO(&readSet);
  FD_SET(DATA_READER(port), &readSet);

  fdsWithData = select(DATA_READER(port) + 1,   // per the man page
                       &readSet,
                       NULL,
                       NULL,
                       &timeout);
  if (fdsWithData < 0) {
    fprintf(stderr,
            "Fatal: select() returned error: %s\n",
            strerror(errno));
    assert(false);
  }
  return (fdsWithData > 0 ? 1 : 0);
}

void sli_legacy_serial_send_ready_to_read(uint8_t port)
{
  if (port == SERIAL_PORT_CLI && sendGoAhead) {
    sendGoAhead = readyForSerialInput(port);
  }
}

// This should only be called by the parent process (i.e. the main app)
sl_status_t sli_legacy_serial_read_byte(uint8_t port, uint8_t *dataByte)
{
  static bool waitForEol = false;
  size_t bytes;

  // The command interpreter reads bytes until it gets an EOL.
  // The CLI reads bytes until it gets a "\r\n".
  // The latter is easily supported since the data is sitting
  // in our data pipe.  The former requires a little extra work.

  // For the command interpreter We don't want to send the 'go-ahead' until
  // the parent reads a byte of data after it has read the EOL.
  if (waitForEol) {
    waitForEol = false;
    return SL_STATUS_EMPTY;
  }

  sli_legacy_serial_send_ready_to_read(port);

  if (0 == sli_legacy_serial_read_available(port)) {
    return SL_STATUS_EMPTY;
  }

  //  debugPrint("Parent: Data is ready.\n");
  bytes = read(DATA_READER(port), (void*)dataByte, 1);

  // We have read the entire line of input, the serial input process will
  // be blocking until we tell it to go ahead and read more input.
  // BugzId:12928 Treat EOF as an EOL
  if (port == SERIAL_PORT_CLI && (bytes == 0 || *dataByte == '\n')) {
    sendGoAhead = true;
    if (usingCommandInterpreter) {
      waitForEol = true;
    }
  }

  if (bytes == 1) {
//    debugPrint("sli_legacy_serial_read_byte(): %c\n", (char)*dataByte);
    return SL_STATUS_OK;
  } else if (bytes == 0) {
    // BugzId:12928 clean up for new client
    // read() only returns 0 on EOF; a non-blocking
    // read() returns -1/EAGAIN on no data but that shouldn't
    // happen per the sli_legacy_serial_read_available() check above.
    kill(childPid[port], SIGTERM); // Should be superfluous, but make sure
    parentCleanupAfterChildDied();
    *dataByte = '\n';
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FAIL;
  }
}

#if !READLINE_SUPPORT
// Support for those systems without the readline library.
static char* readline(const char* prompt)
{
  uint8_t i = 0;
  char* data = malloc(SL_ZIGBEE_COMMAND_BUFFER_LENGTH + 1);  // add 1 for '\0'
  bool done = false;

  debugPrint("Allocated %d bytes for readline()\n", SL_ZIGBEE_COMMAND_BUFFER_LENGTH + 1);

  if (data == NULL) {
    return NULL;
  }

  fprintf(stdout, "%s", prompt);
  fflush(stdout);

  while (!done && i < SL_ZIGBEE_COMMAND_BUFFER_LENGTH) {
    ssize_t bytes = read(STDIN, &(data[i]), 1);
    if (bytes == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        fprintf(stderr, "Attempt to read from STDIN failed!\n");
        assert(false);
      }
    } else if (bytes == 0) {
      // BugzId:12928 EOF encountered, return NULL like real readline() would
      free(data);
      return(NULL);
    }

//    debugPrint("char: 0x%02x\n", (int)data[i]);
    if (data[i] == LINE_FEED        // line feed
        || data[i] == EOF_CHAR) {
      done = true;
    }
    i++;
  }
  if (data[i - 1] == LINE_FEED) {
    // Don't need the LF delineator, it is implied when this function
    // returns.
    i--;
  }
  data[i] = '\0';
  if (data[0] == EOF_CHAR) {
    // BugzId:12928 EOF encountered, return NULL to caller
    free(data);
    return(NULL);
  }
  return data;
}
#endif

// This should only be called by the parent process (i.e. the main app)
sl_status_t sli_legacy_serial_read_line(uint8_t port, char *data, uint8_t max)
{
  uint8_t count = 0;
  sl_status_t status;
  while (count < max) {
    status = sli_legacy_serial_read_byte(port, (uint8_t*)&(data[count]));
    if (SL_STATUS_OK == status) {
      count++;
    } else if (SL_STATUS_EMPTY == status) {
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_OK;
}

// Read a line of data and write it to our pipe.
// Wait for the other process to give us the go-ahead.

static void processSerialInput(uint8_t port)
{
  int length;
  char* readData = NULL;
  char newLine[] = "\r\n";  // The CLI code requires \r\n as the final bytes.
  char goAhead;
  char singleByte[2];

  if (port == SERIAL_PORT_CLI) {
    initializeHistory();
  }

  debugPrint("Processing input for port %d.\n", port);

  while (1) {
    if (port == SERIAL_PORT_CLI && useControlChannel) {
      // Wait for the parent to give the "go ahead"
      debugPrint("Child process waiting for parent's 'go-ahead'.\n");
      if (read(CONTROL_READER(port), &goAhead, 1) < 0) {
        fprintf(stderr, "Attempt to read from port failed");
        assert(false);
      }
    }

    if (port == SERIAL_PORT_CLI) {
      // This performs a malloc()
      readData = readline(prompt);
    } else {
      // Raw data
      readData = &(singleByte[0]);
      bool success = (1 != read(STDIN, readData, 1));

      if (!success) {
        childCleanupAndExit();
      }
      singleByte[1] = '\0';
    }
    if (readData == NULL) {
      // BugzId:12928 On disconnect, let child die; parent will deal with it
      fprintf(stderr,
              "Child process for serial input got EOF. Exiting\n");
      childCleanupAndExit();
    }
    length = (port == SERIAL_PORT_CLI
              ? strnlen(readData, 255)  // 255 is an arbitrary maximum
              : 1);
    assert(-1 < write(DATA_WRITER(port), readData, length));

    if (port == SERIAL_PORT_CLI) {
      debugPrint("readline() input (%d bytes): %s\n", length, readData);
      assert(2 == write(DATA_WRITER(port), &newLine, 2));
      if (length > 0) {
        add_history(readData);
      }
    }

    if (port == SERIAL_PORT_CLI) {
      free(readData);
    }
  }
}

//------------------------------------------------------------------------------
// Serial Output

sl_status_t sli_legacy_serial_guaranteed_printf(uint8_t port, const char * format, ...)
{
  va_list vargs = { 0 };
  sl_status_t stat = SL_STATUS_OK;

  va_start(vargs, format);
  stat = sli_legacy_serial_printf_var_arg(port, format, vargs);
  va_end(vargs);
  return stat;
}

// This is implemented because a few applications count on this internal
// function.  This is essentially a 'sprintf()' where the application wants
// formatted text output but uses its own mechanism to print.  We will ignore
// that use since it doesn't make much sense in the case of a gateway
// application.  If the application really wants to do something special for
// printing formatted output it should call sprintf() on its own.
uint8_t sli_util_printf_internal(emPrintfFlushHandler handler,
                                 uint8_t port,
                                 const char * buff,
                                 va_list list)
{
  (void) handler;

  return (sl_status_t)sli_legacy_serial_printf_var_arg(port, buff, list);
}

// Main printing routine.
// Calls into normal C 'vprintf()'
sl_status_t sli_legacy_serial_printf_var_arg(uint8_t port, const char * formatString, va_list ap)
{
  sl_status_t stat = SL_STATUS_INVALID_PARAMETER;
  char* newFormatString = transformEmberPrintfToStandardPrintf(formatString,
                                                               true);
  if (newFormatString == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  stat = (0 == vprintf(newFormatString, ap)
          ? SL_STATUS_FAIL
          : SL_STATUS_OK);
  fflush(stdout);
  free(newFormatString);
  return stat;
}

sl_status_t sli_legacy_serial_printf(uint8_t port, const char * format, ...)
{
  va_list vargs = { 0 };
  sl_status_t stat = SL_STATUS_OK;

  va_start(vargs, format);
  stat = sli_legacy_serial_printf_var_arg(port, format, vargs);
  va_end(vargs);
  return stat;
}

sl_status_t sli_legacy_serial_printf_line(uint8_t port, const char * formatString, ...)
{
  sl_status_t stat = SL_STATUS_OK;
  va_list ap = { 0 };
  va_start(ap, formatString);
  stat = sli_legacy_serial_printf_var_arg(port, formatString, ap);
  va_end(ap);
  sli_legacy_serial_print_carriage_return(port);
  return stat;
}

sl_status_t sli_legacy_serial_print_carriage_return(uint8_t port)
{
  return sli_legacy_serial_printf(port, "\r\n");
}

sl_status_t sli_legacy_serial_write_byte(uint8_t port, uint8_t dataByte)
{
  return sli_legacy_serial_write_data(port, &dataByte, 1);
}

sl_status_t sli_legacy_serial_write_hex(uint8_t port, uint8_t dataByte)
{
  uint8_t hex[3];
  sprintf((char*)hex, "%2x", dataByte);
  return sli_legacy_serial_write_data(port, hex, 2);
}

sl_status_t sli_legacy_serial_write_data(uint8_t port, uint8_t *data, uint8_t length)
{
  sl_status_t stat = SL_STATUS_FAIL;
  // Normal IO
  if (fwrite(data, length, 1, stdout) == 1) {
    stat = SL_STATUS_OK;
  }
  return stat;
}

sl_status_t sli_legacy_serial_wait_send(uint8_t port)
{
  (void) port;

  fflush(stdout);
  return SL_STATUS_OK;
}

//------------------------------------------------------------------------------
// Serial buffer maintenance

void sli_legacy_serial_flush_rx(uint8_t port)
{
  char buf;
  while (0 < sli_legacy_serial_read_available(port)) {
    if (read(DATA_READER(port), &buf, 1) < 0 ) {
      fprintf(stderr, "Attempt to read from port failed");
      assert(false);
    }
  }
}

//------------------------------------------------------------------------------
// Command Completion support

#if READLINE_SUPPORT

// See readline manual for additional details:
//   http://tiswww.case.edu/php/chet/readline/readline.html#SEC45

// We can complete the command and all complex commands (i.e. zcl global read ...)
// Complex commands require us to traverse the sl_zigbee_command_entry_t tree and
// go into each sub-menu.  We will also display (though not complete)
// the syntax for each command.

// Per readline integration:
// Attempt to complete on the contents of TEXT.  START and END bound the
// region of rl_line_buffer that contains the word to complete.  TEXT is
// the word to complete.  We can use the entire contents of rl_line_buffer
// in case we want to do some simple parsing.  Return the array of matches,
// or NULL if there aren't any.

static sl_zigbee_command_entry_t* subMenuPtr = NULL;
static const char* commandArgumentPtr = NULL;

static const char* findNextNonSpaceChar(const char* line)
{
  while (*line != '\0' && *line == ' ') {
    line++;
  }
  return line;
}

static const char* findNextSpaceChar(const char* line)
{
  while (*line != '\0' && *line != ' ') {
    line++;
  }
  return line;
}

// Recursive function.  Traverses the command-tree
// to find our current sub-menu so we can complete the
// commands for that branch.  If we are at a command-leaf,
// then for completing arguments we figure out where in the arguments
// we are (i.e. the command 'zcl global read <uint16_t> <uint16_t>'
// has two arguments, and we can determine if the line has the first or second
// one in it.
// This function returns true when it has sub-menus/elements that can be parsed.
// It returns false otherwise.
static void setCurrentCommandPtr(const char* nextToken)
{
  const char* wordStart = findNextNonSpaceChar(nextToken == NULL
                                               ? rl_line_buffer
                                               : nextToken);
  const char* wordEnd   = findNextSpaceChar(wordStart);

  uint8_t i = 0;

  if (nextToken == NULL) {
    subMenuPtr = allCommands;
    commandArgumentPtr = NULL;
  }

  if (wordEnd == wordStart) {
    // Nothing to parse.
    return;
  }

  if (commandArgumentPtr && *wordEnd == ' ') {
    if (*commandArgumentPtr != '\0') {
      commandArgumentPtr++;
      setCurrentCommandPtr(wordEnd);
    }
    return;
  }

  while (subMenuPtr != NULL && subMenuPtr[i].name != NULL) {
    // A token is only really complete when there is a space
    // at the end, which is appended by the readline() library
    // code.  In other words, when the user types the first
    // piece of command 'foo' as 'fo' a TAB will complete this
    // as 'foo '.  If the user types the complete 'foo' WITHOUT
    // a space then we want the readline code to validate and
    // complete this by appending a space.
    // This only really matters for complex commands such as 'foo bar'.
    // In order for the 'bar' to be presented as an option when only
    // 'foo' is typed, we need to make sure that a space is appended
    // after that token.
    // This long winded explanation is intended for why we have
    // the "&& *wordEnd == ' '" portion of the if statement.
    if (0 == strncmp(subMenuPtr[i].name,
                     wordStart,
                     strlen(subMenuPtr[i].name))
        && *wordEnd == ' ') {
      if (subMenuPtr[i].action == NULL) {
        // This must be a sub-menu.
        subMenuPtr = (sl_zigbee_command_entry_t*)subMenuPtr[i].argumentTypes;
        setCurrentCommandPtr(wordEnd);
        return;
      } else {
        // We have parsed all the known sub-menus and are now at an actual
        // command.  We will no longer walk down the command-tree.  But
        // we will continue to parse the command tokens.
        commandArgumentPtr = subMenuPtr[i].argumentTypes;
        subMenuPtr = NULL;
        setCurrentCommandPtr(wordEnd);
      }

      if (subMenuPtr == NULL) {
        return;
      }
    }
    i++;
  }
}

static char** commandCompletion(const char* text, int start, int end)
{
  char** matches;

  matches = (char **)NULL;

  debugPrint("commandCompletion(): %s, start: %d, end: %d\n",
             text,
             start,
             end);

  // If this word is at the start of the line, then it is the first
  // token in a command.  Just complete the command.
  // Otherwise we need to find where we are in the current
  // command tree and use that for our search.

  setCurrentCommandPtr(NULL);
  matches = rl_completion_matches(text, singleCommandCompletion);

  return (matches);
}

// Generator function for command completion.  STATE lets us know whether
// to start from scratch; without any state (i.e. STATE == 0), then we
// start at the top of the list.

static char* singleCommandCompletion(const char *text, int state)
{
  static int listIndex;
  static int length;

  if (!state) {
    listIndex = 0;
    length = strnlen(text, SL_ZIGBEE_COMMAND_BUFFER_LENGTH + 1);
    // SL_ZIGBEE_COMMAND_BUFFER_LENGTH needs to be adjusted if this assert() is hit.
    assert(length <= SL_ZIGBEE_COMMAND_BUFFER_LENGTH);
  }

  debugPrint("singleCommandCompletion(): listIndex: %d\n", listIndex);

  if (commandArgumentPtr
      && *commandArgumentPtr != '\0'
      && !state) {
    const char* argumentHelp[] = {
      "uint8_t",
      "uint16_t",
      "uint32_t",
      "int8_t",
      "quoted-string-or-hex-array"
    };
    int argumentIndex = -1;

    switch (*commandArgumentPtr) {
      case 'u':
        argumentIndex = 0;
        break;
      case 'v':
        argumentIndex = 1;
        break;
      case 'w':
        argumentIndex = 2;
        break;
      case 's':
        argumentIndex = 3;
        break;
      case 'b':
        argumentIndex = 4;
        break;
    }

    if (argumentIndex >= 0) {
      printf("\n%s <%s>\n",
             rl_line_buffer,
             argumentHelp[argumentIndex]);
      rl_on_new_line();
      rl_redisplay();
    }

    // Always returning NULL prevents command-completion from working,
    // which is what we want.  We can't complete the arbitrary uint16_t
    // and uint8_t types, but we do want to display what's next.
    return (char*)NULL;
  }

  while (subMenuPtr != NULL
         && subMenuPtr[listIndex].name != NULL) {
    const char* command = subMenuPtr[listIndex].name;
    listIndex++;

    // The trick here is that an empty command line has a length of 0,
    // and thus will match in our strncmp()
    if (0 == strncmp(command, text, length)) {
//      debugPrint("singleCommandCompeletion(): found %s\n", command);

      // It is expected that the readline library will free the data.
      return duplicateString(command);
    }
  }

  debugPrint("singleCommandCompeletion(): no more completions\n");
  return (char*)NULL;
}

static char* filenameCompletion(const char* text, int state)
{
  // We don't do filename completion.

  // Implementing this function prevents readline() from using its default
  // implementation, which displays filenames from the CWD.  Not very helpful.
  return (char*)NULL;
}

static char* duplicateString(const char* source)
{
  char* returnData;
  returnData = malloc(strnlen(source, SL_ZIGBEE_COMMAND_BUFFER_LENGTH) + 1);

  assert(returnData != NULL);

  strcpy(returnData, source);
  return returnData;
}

static void initializeHistory(void)
{
  int myErrno;
  char* homeDirectory;

  readlineHistoryPath[0] = '\0';

  using_history();  // initialize readline() history.

  homeDirectory = getenv("HOME");
  if (homeDirectory == NULL) {
    debugPrint("Error: HOME directory env variable is not defined.\n");
    return;
  } else {
    debugPrint("Home Directory: %s\n", homeDirectory);
  }

  snprintf(readlineHistoryPath,
           MAX_STRING_LENGTH - 1,
           "%s/%s",
           homeDirectory,
           readlineHistoryFilename);
  readlineHistoryPath[MAX_STRING_LENGTH - 1] = '\0';

  myErrno = read_history(readlineHistoryPath);
  if (myErrno != 0) {
    debugPrint("Could not open history file '%s': %s\n",
               readlineHistoryPath,
               strerror(myErrno));
  } else {
    debugPrint("%d history entries read from file '%s'\n",
               history_length,         // readline global
               readlineHistoryPath);
  }
}

static void writeHistory(void)
{
  int myErrno;
  if (readlineHistoryPath[0] == '\0') {
    debugPrint("Readline history path is empty, not writing history.\n");
    return;
  }
  myErrno = write_history(readlineHistoryPath);
  if (myErrno != 0) {
    debugPrint("Failed to write history file '%s': %s\n",
               readlineHistoryPath,
               strerror(myErrno));
  } else {
    debugPrint("Wrote %d entries to history file '%s'\n",
               history_length,       // readline global
               readlineHistoryPath);
  }
}

#endif // #if READLINE_SUPPORT

//------------------------------------------------------------------------------

/*
   Ideally we want the parent process to catch SIGCHLD and then
   cleanup after the child.  If the CLI terminated, the parent
   process will also quit.

   The question is, how do I determine which child quit?
   Presumably this can be done with wait(), which returns the
   child's pid and its associated status.

   The one tricky part is the issue with handling the signal
   SIGCHLD during normal shutdown.  I presume that a signal
   handler cannot be fired within a signal handel, but I am
   not certain.  I need a way of telling whether this
   is a graceful shutdown or not.  Basically if the parent
   initiated the shutdown the children should just go away
   and the parent would ignore notification that they did that
   (SIGCHLD).  If the child died unexpectedly then the parent
   will not ignore that and should nicely cleanup
   and not necessarily exit (in the case of the CLI we presume
   it would).
 */
static void signalHandler(int signal)
{
  static bool reportSigttin = true;
  const char* signalName = strsignal(signal);

  if (signal == SIGTTOU) {
    debugOn = false;
    return;
  }

  if (signal != SIGTTIN || reportSigttin) {
    debugPrint("%s caught signal %s (%d)\n",
               (amParent ? "Parent" : "Child"),
               (signalName == NULL ? "???" : signalName),
               signal);
  }

  if (signal == SIGPIPE) {
    // Ignore this.
    // In general SIGPIPE is useless because it doesn't
    //   give you any context for understanding what read/write from
    //   a pipe caused the error.
    // Instead the send(), receive(), read(), and write() primitives
    //  will return with an error and set errno to EPIPE indicating
    //  that failure.
    return;
  }

  if (signal == SIGTTIN) {
    reportSigttin = false;

    return;
  }

  /*
     if (signal == SIGCHLD) {
     if (amParent) {
      parentCleanupAfterChildDied();
     }
     return;
     }
   */

  // Assume that this is only called for SIGTERM and SIGINT
  if (amParent) {
    sli_legacy_serial_cleanup();
    exit(-1);
  } else {
    childCleanupAndExit();
  }
}

static void installSignalHandler(void)
{
  int i = 0;
  int signals[] = { SIGTERM, SIGINT, SIGPIPE, SIGTTIN, SIGTTOU, 0 };

  struct sigaction sigAction;
  memset(&sigAction, 0, sizeof(struct sigaction));
  sigAction.sa_handler = signalHandler;

  while (signals[i] != 0) {
    if (-1 == sigaction(signals[i], &sigAction, NULL)) {
      debugPrint("Could not install signal handler.\n");
    } else {
      const char* signalName = strsignal(signals[i]);
      debugPrint("%s installed signal handler for %s (%d).\n",
                 (amParent ? "Parent" : "Child"),
                 (signalName == NULL
                  ? "???"
                  : signalName),
                 signals[i]);
    }
    i++;
  }
}

//------------------------------------------------------------------------------
// Debug

static void debugPrint(const char* formatString, ...)
{
  if (debugOn) {
    va_list ap = { 0 };
    fprintf(stderr, "[linux-serial debug] ");
    va_start(ap, formatString);
    vfprintf(stderr, formatString, ap);
    va_end(ap);
    fflush(stderr);
  }
}
