################################################################################
# BLE Peer Manager Filter component                                            #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/filter \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/filter/inc \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/filter/config

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/ble_peer_manager/filter/src/ble_peer_manager_filter.c

override CFLAGS += -DSL_CATALOG_BLE_PEER_MANAGER_FILTER_PRESENT
