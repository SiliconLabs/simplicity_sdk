################################################################################
# MQTT component based on the Mosquitto implementation                         #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/mqtt

override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/mqtt/mqtt.c

REQUIRED_PACKAGES += libmosquitto
