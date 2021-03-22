################################################################################
# Sleeptimer component                                                         #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/sleeptimer

override C_SRC += $(foreach OS_i, $(OS), $(SDK_DIR)/app/bluetooth/common_host/sleeptimer/sl_sleeptimer_$(OS_i).c)
