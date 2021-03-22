################################################################################
# BLE Peer Manager Common component                                            #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/common \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/common/config \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/common/template

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/common/ble_peer_manager_callbacks.c \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/common/ble_peer_manager_connections.c

override CFLAGS += -DSL_CATALOG_BLE_PEER_MANAGER_COMMON_PRESENT
