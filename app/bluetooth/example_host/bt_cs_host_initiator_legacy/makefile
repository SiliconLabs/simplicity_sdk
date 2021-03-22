################################################################################
# General project settings                                                     #
################################################################################

PROJECTNAME = bt_cs_host_initiator_legacy
SDK_DIR = ../../../..


################################################################################
# Components                                                                   #
#                                                                              #
# After setting up the toolchain, components can contribute to the project by  #
# appending items to the project variables like INCLUDEPATHS or C_SRC.         #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/toolchain.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_log.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_assert.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_signal.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_timer.mk
include $(SDK_DIR)/app/bluetooth/component_host/memory_manager.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_host_bt.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_reset.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_version.mk
include $(SDK_DIR)/app/bluetooth/component_host/rtl_lib.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_initiator.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_ras.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_antenna.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_log.mk
include $(SDK_DIR)/app/bluetooth/component_host/ble_peer_manager_common.mk
include $(SDK_DIR)/app/bluetooth/component_host/ble_peer_manager_central.mk
include $(SDK_DIR)/app/bluetooth/component_host/ble_peer_manager_filter.mk
include $(SDK_DIR)/app/bluetooth/component_host/sleeptimer.mk

################################################################################
# Include paths                                                                #
################################################################################

override INCLUDEPATHS += . \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc \
$(SDK_DIR)/protocol/bluetooth/bglib

################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/system/system.c \
app.c \
main.c \
rtl_log.c


################################################################################
# Target rules                                                                 #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/targets.mk
override CFLAGS += -DCS_INITIATOR_ERROR_TIMER_USED=1
