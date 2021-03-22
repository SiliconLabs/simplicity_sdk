################################################################################
# BT Mesh Remote Provisioner Application Component                             #
################################################################################

include $(SDK_DIR)/app/btmesh/component_host/btmesh_app_prov.mk

# SL_CATALOG_APP_PROV_CBP_PRESENT flag indicates CBP component is present
override CFLAGS += -D SL_CATALOG_APP_PROV_CBP_PRESENT

override INCLUDEPATHS += \
$(SDK_DIR)/app/btmesh/common_host/btmesh_app_prov_cbp

override C_SRC += \
$(wildcard $(SDK_DIR)/app/btmesh/common_host/btmesh_app_prov_cbp/*.c)

REQUIRED_PACKAGES += libcrypto
