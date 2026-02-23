-- cs_initiator validation script for checking initiator configuration validity.
local modify_msg = "Modify cs_initiator_config.h!"

-- CS initiator max connection count must align with the BT max connection count
local bt_max_conn =
  autonumber_common.autonumber(slc.config('SL_BT_CONFIG_MAX_CONNECTIONS').value)
local cs_max_conn =
  autonumber_common.autonumber(slc.config('CS_INITIATOR_MAX_CONNECTIONS').value)

if bt_max_conn ~= nil and cs_max_conn ~= nil then
  local err_msg = "Invalid number of maximum initiator connections!"
  if bt_max_conn < cs_max_conn then
    validation.error(err_msg,
      validation.target_for_defines({'CS_INITIATOR_MAX_CONNECTIONS'}),
      [[Number of maximum initiator connections (]] .. cs_max_conn .. [[) is greater 
      than the number of allowed Bluetooth LE connections (]] .. bt_max_conn .. [[)! ]] .. modify_msg,
    nil)
  end
  if cs_max_conn > 4 then
    validation.error(err_msg,
      validation.target_for_defines({'CS_INITIATOR_MAX_CONNECTIONS'}),
      [[Number of maximum initiator connections (]] .. cs_max_conn .. [[) is greater 
      than the maximum value (4)! ]] .. modify_msg,
    nil)
  end
  if cs_max_conn == 0 then
    validation.error(err_msg,
      validation.target_for_defines({'CS_INITIATOR_MAX_CONNECTIONS'}),
      "Number of maximum initiator connections equals to 0! " .. modify_msg,
    nil)
  end
end

-- CS main mode RTT and CS algo mode real-time fast
-- combination is not supported!
local cs_main_mode = slc.config('CS_INITIATOR_DEFAULT_CS_MAIN_MODE').value
local cs_algo_mode = slc.config('CS_INITIATOR_DEFAULT_ALGO_MODE').value
err_msg = "Object tracking mode is incompatible with"

if cs_main_mode ~= nil and cs_algo_mode ~= nil then
  if cs_main_mode == 'sl_bt_cs_mode_rtt' and cs_algo_mode == 'SL_RTL_CS_ALGO_MODE_REAL_TIME_FAST' then
    validation.error(
      err_msg .. " CS main mode!",
      validation.target_for_defines({'CS_INITIATOR_DEFAULT_CS_ALGO_MODE'}),
      [[Object tracking mode (]] .. cs_algo_mode .. [[) is incompatible with 
      CS main mode (]] .. cs_main_mode .. [[)! ]] .. modify_msg,
    nil)
  end
end

-- CS main mode RTT and CS sub mode RTT combination is not supported!
local cs_main_mode = slc.config('CS_INITIATOR_DEFAULT_CS_MAIN_MODE').value
local cs_sub_mode = slc.config('CS_INITIATOR_DEFAULT_CS_SUB_MODE').value
err_msg = "CS main mode is incompatible with"

if cs_main_mode ~= nil and cs_sub_mode ~= nil then
  if cs_main_mode == 'sl_bt_cs_mode_rtt' and cs_sub_mode == 'sl_bt_cs_mode_rtt' then
    validation.error(
    err_msg .. " CS sub mode!",
    validation.target_for_defines({'CS_INITIATOR_DEFAULT_CS_SUB_MODE'}),
    [[CS main mode (]] .. cs_main_mode .. [[) is incompatible with 
    CS sub mode (]] .. cs_sub_mode .. [[)! ]] .. modify_msg,
    nil)
  end
end

-- CS algo mode real-time fast and synchronized procedure execution (max procedure count == 1)
-- combination is not supported!
local cs_max_proc_count =
  autonumber_common.autonumber(slc.config('CS_INITIATOR_DEFAULT_MAX_PROCEDURE_COUNT').value)

if cs_max_proc_count ~= nil then
  if cs_algo_mode == 'SL_RTL_CS_ALGO_MODE_REAL_TIME_FAST' and cs_max_proc_count == 1 then
    validation.error(
      err_msg .. " synchronized procedure execution!",
      validation.target_for_defines({'CS_INITIATOR_DEFAULT_MAX_PROCEDURE_COUNT'}),
      [[Object tracking mode (]] .. cs_algo_mode .. [[) is incompatible with 
      synchronized procedure execution! Change Procedure execution number to 0 
      or change to different object tracking mode! ]],
      nil)
  end
end
