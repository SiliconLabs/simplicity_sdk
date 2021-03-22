################################################################################
# NCP host component with threading for Bluetooth                              #
################################################################################

# NCP security is disabled per default.
# It can be enabled by assigning a non-zero value to the SECURITY variable
# e.g. via command line like 'make SECURITY=1'.
# If enabled, make sure that openssl is available in your environment.

ifneq (, $(filter $(MAKECMDGOALS), export))
# Collect security resources when exporting.
SECURITY_DEFAULT = 1
else
SECURITY_DEFAULT = 0
endif

SECURITY ?= $(SECURITY_DEFAULT)

# CPC communication is disabled per default.
# It can be enabled by assigning a non-zero value to the CPC variable
# e.g. via command line like 'make CPC=1'.
# Be aware that CPC can only be enabled on Linux OS

ifneq (, $(filter $(MAKECMDGOALS), export))
# Collect CPC resources when exporting.
CPC_DEFAULT = 1
else
CPC_DEFAULT = 0
endif

CPC ?= $(CPC_DEFAULT)

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/app_sleep \
$(SDK_DIR)/app/bluetooth/common_host/host_comm \
$(SDK_DIR)/app/bluetooth/common_host/host_comm/config \
$(SDK_DIR)/app/bluetooth/common_host/ncp_host \
$(SDK_DIR)/app/bluetooth/common_host/ncp_host/config \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/app/bluetooth/common_host/tcp \
$(SDK_DIR)/app/bluetooth/common_host/uart \
$(SDK_DIR)/app/bluetooth/common/simple_com \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc

ifneq (, $(filter $(OS), posix))
  override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/named_socket
endif

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/app_sleep/app_sleep.c \
$(foreach OS_i, $(OS), $(SDK_DIR)/app/bluetooth/common_host/host_comm/host_comm_$(OS_i).c) \
$(SDK_DIR)/app/bluetooth/common_host/host_comm/host_comm_robust.c \
$(SDK_DIR)/app/bluetooth/common/simple_com/sl_simple_com_robust.c \
$(SDK_DIR)/app/bluetooth/common_host/ncp_host/ncp_host.c \
$(foreach OS_i, $(OS), $(SDK_DIR)/app/bluetooth/common_host/tcp/tcp_$(OS_i).c) \
$(foreach OS_i, $(OS), $(SDK_DIR)/app/bluetooth/common_host/uart/uart_$(OS_i).c) \
$(SDK_DIR)/protocol/bluetooth/src/sl_bt_ncp_host.c \
$(SDK_DIR)/protocol/bluetooth/src/sl_bt_ncp_host_api.c

ifneq (, $(filter $(OS), posix))
  override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/named_socket/named_socket.c
endif

override LDFLAGS += -lpthread

# Security related settings
ifneq ($(SECURITY), 0)
  override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/ncp_sec
  override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/ncp_sec/ncp_sec_host.c
  override CFLAGS += -DSECURITY
  # Suppress OpenSSL 3.0 warnings until proper update is made on ncp_sec
  override CFLAGS += -DOPENSSL_API_COMPAT=0x10101000L
  REQUIRED_PACKAGES += openssl
endif

# CPC related settings
ifneq ($(CPC), 0)
  ifeq (, $(filter $(MAKECMDGOALS), export))
    ifeq ($(OS), win)
      $(error CPC is not supported on Windows!)
    endif
    ifeq ($(UNAME), darwin)
      $(error CPC is not supported on macOS!)
    endif
  endif

  override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/cpc_bt
  override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/cpc_bt/cpc_bt.c
  override CFLAGS += -DCPC
  REQUIRED_PACKAGES += libcpc
endif

ifeq ($(OS), win)
  # Ws2_32: WinSock library
  override LDFLAGS += -lWs2_32
endif
