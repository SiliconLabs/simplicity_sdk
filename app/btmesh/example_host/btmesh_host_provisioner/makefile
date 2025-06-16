################################################################################
# General project settings                                                     #
################################################################################

PROJECTNAME = btmesh_host_provisioner
PROJ_DIR := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
SDK_DIR := $(realpath $(PROJ_DIR)/../../../..)

# Flag to include Certificate-Based Provisioning
CBP ?= 0
RPR ?= 0
UI ?= 0

# Still add pthread because user input handling uses it
override LDFLAGS += -pthread

################################################################################
# Components                                                                   #
#                                                                              #
# After setting up the toolchain, components can contribute to the project by  #
# appending items to the project variables like INCLUDEPATHS or C_SRC.         #
# Note: CBP needs -lcrypto and -lws2_32, in this order.                        #
# This means btmesh_app_prov_cbp.mk must be included before ncp_host_btmesh.mk #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/toolchain.mk

ifneq (0,$(CBP))
  include $(SDK_DIR)/app/btmesh/component_host/btmesh_app_prov_cbp.mk
endif

include $(SDK_DIR)/app/bluetooth/component_host/app_signal.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_timer.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_version.mk
include $(SDK_DIR)/app/btmesh/component_host/ncp_host_btmesh.mk
include $(SDK_DIR)/app/btmesh/component_host/btmesh_app_prov.mk

ifneq (0,$(RPR))
  include $(SDK_DIR)/app/btmesh/component_host/btmesh_app_remote_prov.mk
endif

ifneq (0,$(UI))
  include $(SDK_DIR)/app/btmesh/component_host/btmesh_app_prov_ui.mk
  ifneq (0,$(RPR))
    include $(SDK_DIR)/app/btmesh/component_host/btmesh_app_remote_prov_ui.mk
  endif
endif

################################################################################
# Include paths                                                                #
################################################################################

override INCLUDEPATHS += $(PROJ_DIR) \
$(SDK_DIR)/app/bluetooth/common/ncp_evt_filter \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc \
$(SDK_DIR)/protocol/bluetooth/config


################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/system/system.c \
$(SDK_DIR)/app/bluetooth/common_host/system/sl_btmesh.c \
$(PROJ_DIR)/app.c \
$(PROJ_DIR)/app_conf.c \
$(PROJ_DIR)/main.c

PROJ_FILES += \
btmesh_host_provisioner.code-workspace \
README.txt

################################################################################
# Target rules                                                                 #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/targets.mk

HELP_MESSAGE += \
"  RPR=1    - add remote provisioning features to the host provisioner application\n" \
"  UI=1     - add the menu user interface to the host provisioner application\n" \
"  CBP=1    - add certificate-based provisioning features\n" \
