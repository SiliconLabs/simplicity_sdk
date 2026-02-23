################################################################################
# General project settings                                                     #
################################################################################

PROJECTNAME = bt_cs_host
PROJ_DIR := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
SDK_DIR := $(realpath $(PROJ_DIR)/../../../..)


################################################################################
# Components                                                                   #
#                                                                              #
# After setting up the toolchain, components can contribute to the project by  #
# appending items to the project variables like INCLUDEPATHS or C_SRC.         #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/toolchain.mk
include $(SDK_DIR)/app/bluetooth/component_host/sl_core_mock.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_log.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_assert.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_signal.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_timer.mk
include $(SDK_DIR)/app/bluetooth/component_host/memory_manager.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_gatt.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_host_bt.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_version.mk
include $(SDK_DIR)/app/bluetooth/component_host/ble_peer_manager_common.mk
include $(SDK_DIR)/app/bluetooth/component_host/ble_peer_manager_central.mk
include $(SDK_DIR)/app/bluetooth/component_host/ble_peer_manager_peripheral.mk
include $(SDK_DIR)/app/bluetooth/component_host/ble_peer_manager_filter.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_reflector.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_initiator_client.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_acp.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_antenna.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_ras_server_hybrid.mk
include $(SDK_DIR)/app/bluetooth/component_host/cs_result.mk

################################################################################
# Include paths                                                                #
################################################################################

override INCLUDEPATHS += $(PROJ_DIR) \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/util/silicon_labs/rtl/inc \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc \
$(SDK_DIR)/protocol/bluetooth/config \
$(SDK_DIR)/protocol/bluetooth/bglib \
$(PROJ_DIR)/config \
$(PROJ_DIR)/autogen


################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/system/system.c \
$(PROJ_DIR)/autogen/gatt_db.c \
$(PROJ_DIR)/app.c \
$(PROJ_DIR)/extended_result.c \
$(PROJ_DIR)/main.c

GATT_XML_FILES += $(PROJ_DIR)/config/btconf/gatt_configuration.btconf

################################################################################
# Configuration                                                                #
################################################################################
override CFLAGS += \
-DBLE_PEER_MANAGER_CENTRAL_CONFIG_DEFAULT_SCAN_INTERVAL=21 \
-DBLE_PEER_MANAGER_CENTRAL_CONFIG_DEFAULT_SCAN_WINDOW=21

################################################################################
# Target rules                                                                 #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/targets.mk
