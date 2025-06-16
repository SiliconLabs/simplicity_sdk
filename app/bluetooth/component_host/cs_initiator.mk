################################################################################
# CS Initiator component                                                       #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/src/cs_initiator.c \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/src/cs_initiator_client.c \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/src/cs_initiator_estimate.c \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/src/cs_initiator_error.c \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/src/cs_initiator_extract.c \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/src/cs_initiator_state_machine.c

override CFLAGS += \
-DSL_CATALOG_CS_INITIATOR_PRESENT

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/inc \
$(SDK_DIR)/app/bluetooth/common/cs_initiator/config