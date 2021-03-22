################################################################################
# CS application logger component                                              #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/cs_log

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/cs_log/cs_log.c
