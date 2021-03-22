################################################################################
# Antenna configurator component for channel sounding                          #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/cs_antenna

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/cs_antenna/cs_antenna_ncp.c
