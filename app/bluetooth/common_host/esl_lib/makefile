################################################################################
# General project settings                                                     #
################################################################################

PROJECTNAME = esl_lib
SDK_DIR = ../../../..

# Enable security for ESL C LIB NCP by default
# It can be disabled by assigning zero value to the SECURITY variable
# e.g. via command line like 'make SECURITY=0'.
# If enabled, make sure that openssl is available in your environment.
SECURITY ?= 1

ifneq (, $(filter $(MAKECMDGOALS), export))
# Collect all resources when exporting.
MEMCHECK_DEFAULT = 1
else
MEMCHECK_DEFAULT = 0
endif

# Memory leak check is disabled by default. Enable it like this: make MEMCHECK=1
MEMCHECK ?= $(MEMCHECK_DEFAULT)

# Security related settings
ifneq ($(SECURITY), 0)
  # Override some secure NCP host component default settings
  override CFLAGS += -DPEEK_US_SLEEP=1
  override CFLAGS += -DRECV_FUNC_US_SLEEP=1
  override CFLAGS += -DMSG_RECV_TIMEOUT_MS=2
endif

# NCP config override
override CFLAGS += -DDEFAULT_HOST_BUFLEN=16384

ifneq ($(MEMCHECK), 0)
override CFLAGS += -DESL_LIB_MEMORY_LEAK_CHECK
endif

################################################################################
# Components                                                                   #
#                                                                              #
# After setting up the toolchain, components can contribute to the project by  #
# appending items to the project variables like INCLUDEPATHS or C_SRC.         #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/toolchain.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_log.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_assert.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_host_bt.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_timer.mk
include $(SDK_DIR)/app/bluetooth/component_host/slist.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_queue.mk
include $(SDK_DIR)/app/bluetooth/component_host/l2cap_transfer.mk
include $(SDK_DIR)/app/bluetooth/component_host/ots_client.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_version.mk


################################################################################
# Include paths                                                                #
################################################################################

override INCLUDEPATHS += . \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc


################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/system/system.c \
esl_lib.c \
esl_lib_core.c \
esl_lib_image_transfer.c \
esl_lib_connection.c \
esl_lib_pawr.c \
simple_argparse.c \
esl_lib_command_list.c \
esl_lib_event_list.c \
esl_lib_ap_control.c \
esl_lib_storage.c \
esl_lib_log.c
ifneq ($(MEMCHECK), 0)
override C_SRC += esl_lib_memory.c
endif

################################################################################
# Target rules                                                                 #
################################################################################

EXE_DIR = lib

ifeq ($(UNAME), linux)
  EXT := .so
else ifeq ($(UNAME), darwin)
  EXT := .dylib
else
  EXT := .dll
endif

override CFLAGS += -fPIC

ifneq ($(UNAME), darwin)
  override CFLAGS += -shared
endif

override LDFLAGS += -shared \
-fPIC \
-lpthread

CFLAGS_RELEASE ?= -DNDEBUG

include $(SDK_DIR)/app/bluetooth/component_host/targets.mk

$(PROJECTNAME)_wrapper.py: $(PROJECTNAME).h
	@echo "Generating Python wrapper"
	ctypesgen --no-gnu-types --allow-gnu-c --no-macro-warnings -I$(SDK_DIR)/platform/common/inc $(PROJECTNAME).h $(SDK_DIR)/platform/common/inc/sl_status.h -x sl_status_get_string_n -x sl_status_print -o $(PROJECTNAME)_wrapper.py  2> /dev/null
	@echo "Fixing up Python wrapper"
	../../script/ctypesgen_wrapper_fix.py $(PROJECTNAME)_wrapper.py -v

clean: clean_wrapper

clean_wrapper:
ifeq ($(filter $(MAKECMDGOALS),all debug release export),)
	rm -f $(PROJECTNAME)_wrapper.py
endif
