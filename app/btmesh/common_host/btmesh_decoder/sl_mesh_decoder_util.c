/***************************************************************************//**
 * @file sl_mesh_decoder_util.c
 * @brief Bluetooth Mesh Decode component utility source file
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
// -----------------------------------------------------------------------------
// Includes
#include "app_log.h"
#include "sl_btmesh_sensor_model_capi_types.h"
#include "sl_btmesh_device_properties.h"

// -----------------------------------------------------------------------------
// Public function definitions

const char* sl_mesh_decoder_util_get_sampling_function_str(uint8_t sampling_function)
{
  switch (sampling_function) {
    case SAMPLING_UNSPECIFIED:
      return "Unspecified";
    case SAMPLING_INSTANTANEOUS:
      return "Instantaneous";
    case SAMPLING_ARITHMETIC_MEAN:
      return "Arithmetic mean";
    case SAMPLING_RMS:
      return "RMS";
    case SAMPLING_MAXIMUM:
      return "Maximum";
    case SAMPLING_MINIMUM:
      return "Minimum";
    case SAMPLING_ACCUMULATED:
      return "Accumulated";
    case SAMPLING_COUNT:
      return "Count";
    default:
      return "Reserved for Future Use";
  }
}

const char* sl_mesh_decoder_util_get_sensor_setting_access_str(uint8_t setting_access)
{
  switch (setting_access) {
    case 0x01:
      return "The device property can be read";
      break;
    case 0x03:
      return "The device property can be read and written";
      break;
    default:
      return "Prohibited";
      break;
  }
}

uint8_t sl_mesh_decoder_util_get_sensor_data_size(uint16_t property_id)
{
  // Size of data in bytes
  uint8_t data_size = 0;
  switch (property_id) {
    case PRESENT_AMBIENT_TEMPERATURE:
    case PRESENT_INDOOR_AMBIENT_TEMPERATURE:
    case PRESENT_OUTDOOR_AMBIENT_TEMPERATURE:
    case COLOR_RENDERING_INDEX_R9:
    case COLOR_RENDERING_INDEX_RA:
    case PRESENCE_DETECTED:
    case CHROMATICITY_TOLERANCE:
    case INPUT_VOLTAGE_RIPPLE_SPECIFICATION:
    case LIGHT_CONTROL_REGULATOR_ACCURACY:
    case LUMEN_MAINTENANCE_FACTOR:
    case MOTION_SENSED:
    case MOTION_THRESHOLD:
    case OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION:
    case PRESENT_DEVICE_OPERATING_EFFICIENCY:
    case PRESENT_INPUT_RIPPLE_VOLTAGE:
    case PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE:
    {
      data_size = 1;
      break;
    }
    case INITIAL_PLANCKIAN_DISTANCE:
    case PRESENT_PLANCKIAN_DISTANCE:
    case INITIAL_CORRELATED_COLOR_TEMPERATURE:
    case PRESENT_CORRELATED_COLOR_TEMPERATURE:
    case PEOPLE_COUNT:
    case DEVICE_COUNTRY_OF_ORIGIN:
    case PRESENT_INPUT_CURRENT:
    case PRESENT_OUTPUT_CURRENT:
    case LUMINOUS_EFFICACY:
    case INITIAL_LUMINOUS_FLUX:
    case PRESENT_LUMINOUS_FLUX:
    case CENTER_BEAM_INTENSITY_AT_FULL_POWER:
    case LIGHT_CONTROL_LIGHTNESS_ON:
    case LIGHT_CONTROL_LIGHTNESS_PROLONG:
    case LIGHT_CONTROL_LIGHTNESS_STANDBY:
    case PRESENT_DEVICE_OPERATING_TEMPERATURE:
    case TIME_SINCE_PRESENCE_DETECTED:
    case PRESENT_INPUT_VOLTAGE:
    case PRESENT_OUTPUT_VOLTAGE:
    {
      data_size = 2;
      break;
    }
    case AVERAGE_INPUT_CURRENT:
    case AVERAGE_OUTPUT_CURRENT:
    case AVERAGE_INPUT_VOLTAGE:
    case AVERAGE_OUTPUT_VOLTAGE:
    case AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY:
    case TOTAL_DEVICE_OFF_ON_CYCLES:
    case TOTAL_DEVICE_POWER_ON_CYCLES:
    case DEVICE_DATE_OF_MANUFACTURE:
    case DEVICE_ENERGY_USE_SINCE_TURN_ON:
    case TOTAL_DEVICE_ENERGY_USE:
    case DEVICE_RUNTIME_SINCE_TURN_ON:
    case DEVICE_RUNTIME_WARRANTY:
    case TOTAL_DEVICE_POWER_ON_TIME:
    case TOTAL_DEVICE_RUNTIME:
    case TOTAL_LIGHT_EXPOSURE_TIME:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY:
    case LUMINOUS_ENERGY_SINCE_TURN_ON:
    case RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE:
    case TOTAL_LUMINOUS_ENERGY:
    case LUMINOUS_EXPOSURE:
    case PRESENT_DEVICE_INPUT_POWER:
    case LIGHT_CONTROL_TIME_FADE:
    case LIGHT_CONTROL_TIME_FADE_ON:
    case LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO:
    case LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL:
    case LIGHT_CONTROL_TIME_OCCUPANCY_DELAY:
    case LIGHT_CONTROL_TIME_PROLONG:
    case LIGHT_CONTROL_TIME_RUN_ON:
    case PRESENT_AMBIENT_LIGHT_LEVEL:
    case PRESENT_ILLUMINANCE:
    case TIME_SINCE_MOTION_SENSED:
    {
      data_size = 3;
      break;
    }
    case LIGHT_CONTROL_REGULATOR_KID:
    case LIGHT_CONTROL_REGULATOR_KIU:
    case LIGHT_CONTROL_REGULATOR_KPD:
    case LIGHT_CONTROL_REGULATOR_KPU:
    case PRECISE_TOTAL_DEVICE_ENERGY_USE:
    case INITIAL_CIE_1931_CHROMATICITY_COORDINATES:
    case PRESENT_CIE_1931_CHROMATICITY_COORDINATES:
    case OUTPUT_CURRENT_RANGE:
    case RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY:
    case DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION:
    {
      data_size = 4;
      break;
    }
    case INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES:
    case OUTDOOR_STATISTICAL_VALUES:
    {
      data_size = 5;
      break;
    }
    case INPUT_CURRENT_RANGE_SPECIFICATION:
    case DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS:
    case DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS:
    case INPUT_OVER_CURRENT_EVENT_STATISTICS:
    case INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS:
    case INPUT_OVER_VOLTAGE_EVENT_STATISTICS:
    case INPUT_UNDER_CURRENT_EVENT_STATISTICS:
    case INPUT_UNDER_VOLTAGE_EVENT_STATISTICS:
    case OPEN_CIRCUIT_EVENT_STATISTICS:
    case OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS:
    case SHORT_CIRCUIT_EVENT_STATISTICS:
    case INPUT_VOLTAGE_RANGE_SPECIFICATION:
    case OUTPUT_VOLTAGE_RANGE:
    {
      data_size = 6;
      break;
    }
    case INPUT_CURRENT_STATISTICS:
    case OUTPUT_CURRENT_STATISTICS:
    case DEVICE_POWER_RANGE_SPECIFICATION:
    case DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES:
    case INPUT_VOLTAGE_STATISTICS:
    case OUTPUT_VOLTAGE_STATISTICS:
    {
      data_size = 9;
      break;
    }
    default:
      break;
  }
  return data_size;
}

const char* sl_mesh_decoder_util_get_property_str(uint16_t property_id)
{
  const char* property_str = NULL;
  switch (property_id) {
    case AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY:
      property_str = "Average Ambient Temperature In a Period Of Day";
      break;
    case AVERAGE_INPUT_CURRENT:
      property_str = "Average Input Current";
      break;
    case AVERAGE_INPUT_VOLTAGE:
      property_str = "Average Input Voltage";
      break;
    case AVERAGE_OUTPUT_CURRENT:
      property_str = "Average Output Current";
      break;
    case AVERAGE_OUTPUT_VOLTAGE:
      property_str = "Average Output Voltage";
      break;
    case CENTER_BEAM_INTENSITY_AT_FULL_POWER:
      property_str = "Center Beam Intensity At Full Power";
      break;
    case CHROMATICITY_TOLERANCE:
      property_str = "Chromaticity Tolerance";
      break;
    case COLOR_RENDERING_INDEX_R9:
      property_str = "Color Rendering Index R9";
      break;
    case COLOR_RENDERING_INDEX_RA:
      property_str = "Color Rendering Index RA";
      break;
    case DEVICE_APPEARANCE:
      property_str = "Device Appearance";
      break;
    case DEVICE_COUNTRY_OF_ORIGIN:
      property_str = "Device Country Of Origin";
      break;
    case DEVICE_DATE_OF_MANUFACTURE:
      property_str = "Device Date Of Manufacture";
      break;
    case DEVICE_ENERGY_USE_SINCE_TURN_ON:
      property_str = "Device Energy Use Since Turn On";
      break;
    case DEVICE_FIRMWARE_REVISION:
      property_str = "Device Firmware Revision";
      break;
    case DEVICE_GLOBAL_TRADE_ITEM_NUMBER:
      property_str = "Device Global Trade Item Number";
      break;
    case DEVICE_HARDWARE_REVISION:
      property_str = "Device Hardware Revision";
      break;
    case DEVICE_MANUFACTURER_NAME:
      property_str = "Device Manufacturer Name";
      break;
    case DEVICE_MODEL_NUMBER:
      property_str = "Device Model Number";
      break;
    case DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION:
      property_str = "Device Operating Temperature Range Specification";
      break;
    case DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES:
      property_str = "Device Operating Temperature Statistical Values";
      break;
    case DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS:
      property_str = "Device Over Temperature Event Statistics";
      break;
    case DEVICE_POWER_RANGE_SPECIFICATION:
      property_str = "Device Power Range Specification";
      break;
    case DEVICE_RUNTIME_SINCE_TURN_ON:
      property_str = "Device Runtime Since Turn On";
      break;
    case DEVICE_RUNTIME_WARRANTY:
      property_str = "Device Runtime Warranty";
      break;
    case DEVICE_SERIAL_NUMBER:
      property_str = "Device Serial Number";
      break;
    case DEVICE_SOFTWARE_REVISION:
      property_str = "Device Software Revision";
      break;
    case DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS:
      property_str = "Device Under Temperature Event Statistics";
      break;
    case INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES:
      property_str = "Indoor Ambient Temperature Statistical Values";
      break;
    case INITIAL_CIE_1931_CHROMATICITY_COORDINATES:
      property_str = "Initial CIE 1931 Chromaticity Coordinates";
      break;
    case INITIAL_CORRELATED_COLOR_TEMPERATURE:
      property_str = "Initial Correlated Color Temperature";
      break;
    case INITIAL_LUMINOUS_FLUX:
      property_str = "Initial Luminous Flux";
      break;
    case INITIAL_PLANCKIAN_DISTANCE:
      property_str = "Initial Planckian Distance";
      break;
    case INPUT_CURRENT_RANGE_SPECIFICATION:
      property_str = "Input Current Range Specification";
      break;
    case INPUT_CURRENT_STATISTICS:
      property_str = "Input Current Statistics";
      break;
    case INPUT_OVER_CURRENT_EVENT_STATISTICS:
      property_str = "Input Over Current Event Statistics";
      break;
    case INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS:
      property_str = "Input Over Ripple Voltage Event Statistics";
      break;
    case INPUT_OVER_VOLTAGE_EVENT_STATISTICS:
      property_str = "Input Over Voltage Event Statistics";
      break;
    case INPUT_UNDER_CURRENT_EVENT_STATISTICS:
      property_str = "Input Under Current Event Statistics";
      break;
    case INPUT_UNDER_VOLTAGE_EVENT_STATISTICS:
      property_str = "Input Under Voltage Event Statistics";
      break;
    case INPUT_VOLTAGE_RANGE_SPECIFICATION:
      property_str = "Input Voltage Range Specification";
      break;
    case INPUT_VOLTAGE_RIPPLE_SPECIFICATION:
      property_str = "Input Voltage Ripple Specification";
      break;
    case INPUT_VOLTAGE_STATISTICS:
      property_str = "Input Voltage Statistics";
      break;
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON:
      property_str = "Light Control Ambient LuxLevel On";
      break;
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG:
      property_str = "Light Control Ambient LuxLevel Prolong";
      break;
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY:
      property_str = "Light Control Ambient LuxLevel Standby";
      break;
    case LIGHT_CONTROL_LIGHTNESS_ON:
      property_str = "Light Control Lightness On";
      break;
    case LIGHT_CONTROL_LIGHTNESS_PROLONG:
      property_str = "Light Control Lightness Prolong";
      break;
    case LIGHT_CONTROL_LIGHTNESS_STANDBY:
      property_str = "Light Control Lightness Standby";
      break;
    case LIGHT_CONTROL_REGULATOR_ACCURACY:
      property_str = "Light Control Regulator Accuracy";
      break;
    case LIGHT_CONTROL_REGULATOR_KID:
      property_str = "Light Control Regulator Kid";
      break;
    case LIGHT_CONTROL_REGULATOR_KIU:
      property_str = "Light Control Regulator Kiu";
      break;
    case LIGHT_CONTROL_REGULATOR_KPD:
      property_str = "Light Control Regulator Kpd";
      break;
    case LIGHT_CONTROL_REGULATOR_KPU:
      property_str = "Light Control Regulator Kpu";
      break;
    case LIGHT_CONTROL_TIME_FADE:
      property_str = "Light Control Time Fade";
      break;
    case LIGHT_CONTROL_TIME_FADE_ON:
      property_str = "Light Control Time Fade On";
      break;
    case LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO:
      property_str = "Light Control Time Fade Standby Auto";
      break;
    case LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL:
      property_str = "Light Control Time Fade Standby Manual";
      break;
    case LIGHT_CONTROL_TIME_OCCUPANCY_DELAY:
      property_str = "Light Control Time Occupancy Delay";
      break;
    case LIGHT_CONTROL_TIME_PROLONG:
      property_str = "Light Control Time Prolong";
      break;
    case LIGHT_CONTROL_TIME_RUN_ON:
      property_str = "Light Control Time Run On";
      break;
    case LUMEN_MAINTENANCE_FACTOR:
      property_str = "Lumen Maintenance Factor";
      break;
    case LUMINOUS_EFFICACY:
      property_str = "Luminous Efficacy";
      break;
    case LUMINOUS_ENERGY_SINCE_TURN_ON:
      property_str = "Luminous Energy Since Turn On";
      break;
    case LUMINOUS_EXPOSURE:
      property_str = "Luminous Exposure";
      break;
    case LUMINOUS_FLUX_RANGE:
      property_str = "Luminous Flux Range";
      break;
    case MOTION_SENSED:
      property_str = "Motion Sensed";
      break;
    case MOTION_THRESHOLD:
      property_str = "Motion Threshold";
      break;
    case OPEN_CIRCUIT_EVENT_STATISTICS:
      property_str = "Open Circuit Event Statistics";
      break;
    case OUTDOOR_STATISTICAL_VALUES:
      property_str = "Outdoor Statistical Values";
      break;
    case OUTPUT_CURRENT_RANGE:
      property_str = "Output Current Range";
      break;
    case OUTPUT_CURRENT_STATISTICS:
      property_str = "Output Current Statistics";
      break;
    case OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION:
      property_str = "Output Ripple Voltage Specification";
      break;
    case OUTPUT_VOLTAGE_RANGE:
      property_str = "Output Voltage Range";
      break;
    case OUTPUT_VOLTAGE_STATISTICS:
      property_str = "Output Voltage Statistics";
      break;
    case OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS:
      property_str = "Over Output Ripple Voltage Event Statistics";
      break;
    case PEOPLE_COUNT:
      property_str = "People Count";
      break;
    case PRESENCE_DETECTED:
      property_str = "Presence Detected";
      break;
    case PRESENT_AMBIENT_LIGHT_LEVEL:
      property_str = "Present Ambient Light Level";
      break;
    case PRESENT_AMBIENT_TEMPERATURE:
      property_str = "Present Ambient Temperature";
      break;
    case PRESENT_CIE_1931_CHROMATICITY_COORDINATES:
      property_str = "Present CIE 1931 Chromaticity Coordinates";
      break;
    case PRESENT_CORRELATED_COLOR_TEMPERATURE:
      property_str = "Present Correlated Color Temperature";
      break;
    case PRESENT_DEVICE_INPUT_POWER:
      property_str = "Present Device Input Power";
      break;
    case PRESENT_DEVICE_OPERATING_EFFICIENCY:
      property_str = "Present Device Operating Efficiency";
      break;
    case PRESENT_DEVICE_OPERATING_TEMPERATURE:
      property_str = "Present Device Operating Temperature";
      break;
    case PRESENT_ILLUMINANCE:
      property_str = "Present Illuminance";
      break;
    case PRESENT_INDOOR_AMBIENT_TEMPERATURE:
      property_str = "Present Indoor Ambient Temperature";
      break;
    case PRESENT_INPUT_CURRENT:
      property_str = "Present Input Current";
      break;
    case PRESENT_INPUT_RIPPLE_VOLTAGE:
      property_str = "Present Input Ripple Voltage";
      break;
    case PRESENT_INPUT_VOLTAGE:
      property_str = "Present Input Voltage";
      break;
    case PRESENT_LUMINOUS_FLUX:
      property_str = "Present Luminous Flux";
      break;
    case PRESENT_OUTDOOR_AMBIENT_TEMPERATURE:
      property_str = "Present Outdoor Ambient Temperature";
      break;
    case PRESENT_OUTPUT_CURRENT:
      property_str = "Present Output Current";
      break;
    case PRESENT_OUTPUT_VOLTAGE:
      property_str = "Present Output Voltage";
      break;
    case PRESENT_PLANCKIAN_DISTANCE:
      property_str = "Present Planckian Distance";
      break;
    case PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE:
      property_str = "Present Relative Output Ripple Voltage";
      break;
    case RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY:
      property_str = "Relative Device Energy Use In A Period Of Day";
      break;
    case RELATIVE_DEVICE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE:
      property_str = "Relative Device Runtime In A Generic Level Range";
      break;
    case RELATIVE_EXPOSURE_TIME_IN_AN_ILLUMINANCE_RANGE:
      property_str = "Relative Exposure Time In An Illuminance Range";
      break;
    case RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE:
      property_str = "Relative Runtime In A Correlated Color Temperature Range";
      break;
    case RELATIVE_RUNTIME_IN_A_DEVICE_OPERATING_TEMPERATURE_RANGE:
      property_str = "Relative Runtime In A Device Operating Temperature Range";
      break;
    case RELATIVE_RUNTIME_IN_AN_INPUT_CURRENT_RANGE:
      property_str = "Relative Runtime In An Input Current Range";
      break;
    case RELATIVE_RUNTIME_IN_AN_INPUT_VOLTAGE_RANGE:
      property_str = "Relative Runtime In An Input Voltage Range";
      break;
    case SHORT_CIRCUIT_EVENT_STATISTICS:
      property_str = "Short Circuit Event Statistics";
      break;
    case TIME_SINCE_MOTION_SENSED:
      property_str = "Time Since Motion Sensed";
      break;
    case TIME_SINCE_PRESENCE_DETECTED:
      property_str = "Time Since Presence Detected";
      break;
    case TOTAL_DEVICE_ENERGY_USE:
      property_str = "Total Device Energy Use";
      break;
    case TOTAL_DEVICE_OFF_ON_CYCLES:
      property_str = "Total Device Off On Cycles";
      break;
    case TOTAL_DEVICE_POWER_ON_CYCLES:
      property_str = "Total Device Power On Cycles";
      break;
    case TOTAL_DEVICE_POWER_ON_TIME:
      property_str = "Total Device Power On Time";
      break;
    case TOTAL_DEVICE_RUNTIME:
      property_str = "Total Device Runtime";
      break;
    case TOTAL_LIGHT_EXPOSURE_TIME:
      property_str = "Total Light Exposure Time";
      break;
    case TOTAL_LUMINOUS_ENERGY:
      property_str = "Total Luminous Energy";
      break;
    case DESIRED_AMBIENT_TEMPERATURE:
      property_str = "Desired Ambient Temperature";
      break;
    case PRECISE_TOTAL_DEVICE_ENERGY_USE:
      property_str = "Precise Total Device Energy Use";
      break;
    default:
      property_str = "Unknown Property ID";
      break;
  }
  return property_str;
}
