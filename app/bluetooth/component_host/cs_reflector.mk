################################################################################
# CS reflector component                                                       #
################################################################################

override CFLAGS += \
-DSL_CATALOG_CS_REFLECTOR_PRESENT

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/cs_reflector