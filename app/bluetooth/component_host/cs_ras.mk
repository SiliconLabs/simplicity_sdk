################################################################################
# CS RAS component                                                             #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/cs_ras

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/cs_ras/cs_ras.c
