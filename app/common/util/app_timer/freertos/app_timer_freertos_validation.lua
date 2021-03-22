-- app_timer validation
local cfg_value = autonumber_common.autonumber(slc.config("configUSE_TIMERS").value)
local enable_static = (slc.config('configSUPPORT_STATIC_ALLOCATION').value)
local enable_dynamic = autonumber_common.autonumber(slc.config('configSUPPORT_DYNAMIC_ALLOCATION').value)

if cfg_value == nil or cfg_value == 0 then
        validation.error("Kernel configUSE_TIMERS config needs to be enabled",
        validation.target_for_defines({"configUSE_TIMERS"}),
        nil,
        nil)
end

if (enable_static == nil or enable_static == 0) and (enable_dynamic == nil or enable_dynamic == 0) then
        validation.error("Kernel configSUPPORT_STATIC_ALLOCATION or configSUPPORT_DYNAMIC_ALLOCATION config needs to be enabled in FreeRTOS config (FreeRTOSConfig.h)",
        validation.target_for_defines({"configSUPPORT_STATIC_ALLOCATION"}),
        nil,
        nil)
end

