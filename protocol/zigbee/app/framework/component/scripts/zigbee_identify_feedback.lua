local is_host = slc.is_selected("zigbee_ezsp")
local led_feedback_enabled = slc.config("SL_ZIGBEE_AF_PLUGIN_IDENTIFY_FEEDBACK_LED_FEEDBACK").value == "1"
local simple_led_enabled = slc.is_selected("simple_led")

if not is_host and not simple_led_enabled and led_feedback_enabled then
    validation.warning("Simple LED should be enable to use the Identify Feedback LED feature.",
        validation.target_for_defines({"SL_ZIGBEE_AF_PLUGIN_IDENTIFY_FEEDBACK_LED_FEEDBACK"}),
        nil,
        nil)
end