local changeset = {}

local zigbee_end_device_support_configuration = slc.is_provided('zigbee_end_device_support')

if (zigbee_end_device_support_configuration == true) then
  local config = slc.config('SL_ZIGBEE_AF_PLUGIN_END_DEVICE_SUPPORT_MOVE_DELAY_SECONDS')
  if (config == nil) then
    -- sisdk-2025.6.3 introduced this config for first time the config is introduced with a default value.
    table.insert(changeset, {
      ['option'] = 'SL_ZIGBEE_AF_PLUGIN_END_DEVICE_SUPPORT_MOVE_DELAY_SECONDS',
      ['value'] = tostring(10)
    })
  end
end

return changeset