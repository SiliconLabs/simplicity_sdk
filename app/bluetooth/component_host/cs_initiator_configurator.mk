################################################################################
# CS initiator configurator component                                          #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/src/cs_initiator_configurator.c

override CFLAGS += \
-DSL_CATALOG_CS_INITIATOR_PRESENT

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/inc \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/config
