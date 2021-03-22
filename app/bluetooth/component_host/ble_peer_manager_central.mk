################################################################################
# BLE Peer Manager Central component                                           #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/central \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/central/inc \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/central/config

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/central/src/ble_peer_manager_central.c

override CFLAGS += -DSL_CATALOG_BLE_PEER_MANAGER_CENTRAL_PRESENT
