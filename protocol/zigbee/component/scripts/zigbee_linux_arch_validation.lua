--[[
    This validation script checks following:
    Only one type of architecture is allowed to be included in Zigbee applications
--]]
local i386_enabled   = slc.is_selected("zigbee_i386")
local arm32_enabled  = slc.is_selected("zigbee_arm32")
local x86_64_enabled = slc.is_selected("zigbee_x86_64")
local arm64_enabled  = slc.is_selected("zigbee_arm64")
local aarch64_cortex_a72_gcc_12_3_0_musl_enabled  = slc.is_selected("zigbee_aarch64_cortex_a72_gcc_12_3_0_musl")

if (i386_enabled == true and arm32_enabled == true) or (x86_64_enabled == true and arm64_enabled == true) then
    validation.error("Multiple architectures are not supported.",
                     validation.target_for_project(),
                     "Remove the \"Recommended Linux architecture\" component first.",
                     nil)
end