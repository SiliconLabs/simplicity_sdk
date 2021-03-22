################################################################################
# General project settings                                                     #
################################################################################

PROJECTNAME = esl_key_lib
SDK_DIR = ../../../..

################################################################################
# Components                                                                   #
#                                                                              #
# After setting up the toolchain, components can contribute to the project by  #
# appending items to the project variables like INCLUDEPATHS or C_SRC.         #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/toolchain.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_log.mk

################################################################################
# Include paths                                                                #
################################################################################

override INCLUDEPATHS += . \
$(SDK_DIR)/util/third_party/sqlite \
$(SDK_DIR)/util/third_party/mbedtls/include \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc \
$(SDK_DIR)/app/bluetooth/common/esl_tag_core/inc \
$(SDK_DIR)/app/bluetooth/common/esl_tag_core/config \
$(SDK_DIR)/app/bluetooth/common_host/ncp_host \
$(SDK_DIR)/app/bluetooth/common/ead_core \

################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
esl_key_lib.c \
$(SDK_DIR)/util/third_party/sqlite/sqlite3.c

################################################################################
# Target rules                                                                 #
################################################################################

EXE_DIR = lib

override CFLAGS += -fPIC

ifneq ($(UNAME), darwin)
  override CFLAGS += -shared
endif

override LDFLAGS += -shared -fPIC

ifeq ($(UNAME), linux)
  EXT := .so
  override LDFLAGS += -lpthread -ldl -lc
else ifeq ($(UNAME), darwin)
  EXT := .dylib
  override LDFLAGS += -lpthread -ldl -lc
else
  EXT := .dll
  override CFLAGS += -static-libgcc -DWIN_DLL
endif

CFLAGS_RELEASE ?= -DNDEBUG \
-DAPP_LOG_CONFIG_H -DAPP_LOG_ENABLE=0 \
-DAPP_LOG_LEVEL_FILTER_ENABLE=0 \
-DAPP_LOG_LEVEL_FILTER_THRESHOLD=0 \
-DAPP_LOG_LEVEL_MASK_ENABLE=0 \
-DAPP_LOG_LEVEL_MASK_DEBUG=0 \
-DAPP_LOG_LEVEL_MASK_INFO=0 \
-DAPP_LOG_LEVEL_MASK_WARNING=0 \
-DAPP_LOG_LEVEL_MASK_ERROR=0 \
-DAPP_LOG_LEVEL_MASK_CRITICAL=0

include $(SDK_DIR)/app/bluetooth/component_host/targets.mk

$(PROJECTNAME)_wrapper.py: $(PROJECTNAME).h
	@echo "Generating Python wrapper"
	ctypesgen --no-gnu-types --allow-gnu-c --no-macro-warnings $(INCFLAGS) $(PROJECTNAME).h -o $(PROJECTNAME)_wrapper.py 2> /dev/null
	@echo "Fixing up Python wrapper"
	../../script/ctypesgen_wrapper_fix.py $(PROJECTNAME)_wrapper.py -v

clean: clean_wrapper

clean_wrapper:
ifeq ($(filter $(MAKECMDGOALS),all debug release export),)
	rm -f $(PROJECTNAME)_wrapper.py
endif

HELP_MESSAGE += "  clean_keep_sql - remove build directories but keep sqlite3 files\n"

clean_keep_sql: clean_export
ifeq ($(filter $(MAKECMDGOALS),all debug release export),)
	find $(OBJ_DIR) ! \( -type f -and -iname "sqlite3.*" \) -delete || true
	rm -rf $(EXE_DIR)
	rm -f $(PROJECTNAME)_wrapper.py
endif

