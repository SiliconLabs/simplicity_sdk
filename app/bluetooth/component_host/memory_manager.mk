################################################################################
# Memory manager Wrapper                                                       #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/platform/service/memory_manager/inc

override C_SRC += \
$(SDK_DIR)/platform/service/memory_manager/src/sl_memory_manager_redirect.c

override CFLAGS += -DSL_CATALOG_MEMORY_MANAGER_PRESENT