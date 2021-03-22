################################################################################
# BLE Peer Manager Peripheral component                                        #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/peripheral \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/peripheral/inc \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/peripheral/config

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/peripheral/src/ble_peer_manager_peripheral.c

override CFLAGS += -DSL_CATALOG_BLE_PEER_MANAGER_PERIPHERAL_PRESENT
