################################################################################
# CS Result                                                                    #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/cs_result/src/cs_result.c

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/cs_result/inc \
$(SDK_DIR)/app/bluetooth/common/cs_result/config
