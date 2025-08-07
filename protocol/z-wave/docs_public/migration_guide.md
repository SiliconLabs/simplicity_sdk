# Migration Guide {#migration-guide}

This guide shows how to migrate projects from previous SDKs to a newer one.

# 7.23.3 {#migrate-section-7-23-3}

In order to reduce the flash memory usage, the following changes were applied to newly generated projects. Below are the changes need to be made in an existing project in order to align with the updates.

## Multilevel Sensor

In `MultilevelSensor.cc_config` file, remove all acceleration sensors, namely `motion_axis_x`, `motion_axis_y` and `motion_axis_z`.

## Removing CLI command descriptions

CLI command descriptions were removed in sample applications' project files. In an existing project, remove the `help` fields from the CLI commands in the `template_contribution` section.

### Example on Power Strip project:

`zwave_soc_power_strip.slcp` created with SDK version 7.23.2:

```
...
template_contribution:
- condition: [zw_cli_common]
  name: cli_command
  priority: 0
  value:
    name: toggle_endpoint
    handler: cli_toggle_endpoint
    help: Toggle the endpoint 1 or 2. Possible values are 1 or 2
    argument:
    - {type: uint8, help: Endpoint number (1 or 2)}
...
```

Manually modified `zwave_soc_power_strip.slcp` after upgrading to SDK version 7.23.3:

```
...
template_contribution:
- condition: [zw_cli_common]
  name: cli_command
  priority: 0
  value:
    name: toggle_endpoint
    handler: cli_toggle_endpoint
    argument:
    - {type: uint8}
...
```

# 7.23.0 {#migrate-section-7-23-0}

## How to migrate legacy HMIs to Silicon Labs based solution

### General Information
In the 7.23 Z-Wave release, Silicon Labs changed the legacy button and LED handling to the Silicon Labs platform provided solution. This document describes how to migrate the legacy human-machine interfaces (from 7.19) to the Silicon Labs based solution. This document can be divided into two parts: one will describe the new components that have been introduced in the 7.23 release; and the other part will describe how to migrate a 7.22 project to the 7.23 release.

### New components

#### SL Simple Button
The Simple Button Driver provides an implementation of the Generic Button API. This driver supports both active high and low buttons and configurable debouncing. This component is instantiable, meaning that several named instances can be created. For each instance a configuration is generated along with an initialization struct set according to this configuration. These instance defines and variables are available in a generated header file, **sl_simple_button_instances.h**. Selecting this component will also include the Simple Button Core component, which is the implementation of the Simple Button driver itself. [API Reference](https://docs.silabs.com/gecko-platform/latest/platform-driver/simple-button)

#### SL Simple LED
The Simple LED Driver provides an implementation of the Generic LED API. This driver supports controlling GPIO based on/off type LEDs. This component is instantiable, meaning that several named instances can be created. For each instance a configuration is generated along with an initialization struct set according to this configuration. These instance defines and variables are available in a generated header file, **sl_simple_led_instances.h**. Selecting this component will also include the Simple LED Core component, which is the implementation of the Simple LED driver itself. [API Reference (5.0.2)](https://docs.silabs.com/gecko-platform/5.0.2/platform-driver/simple-led)

#### SL App Button Press
The Button Press component is used for handling the button state, which can be:
- Button is not pressed,
- Button is pressed for a short duration,
- Button is pressed for a medium duration,
- Button is pressed for a long duration,
- Button is pressed for a very long duration and
- Button has just been pressed down.

This component is built on SL Simple Button. Button press duration for various lengths is configurable and each end user can configure it according to their needs.

### Differences in the project structure
In this document, the most complex project will be used as an example, which is the Z-Wave SoC Door Lock Keypad project. In the following table, the differences in the source files can be seen between the 7.22 and 7.23 projects.

| 7.22 Top Project Stucture | 7.23 Top Project Stucture      |
|---------------------------|--------------------------------|
|                           |   app_btn_handler.c            |
|                           |   app_cc_battery_callbacks.c   |
|                           |   app_cc_door_lock_callbacks.c |
|   app_cc_event_handlers   |   app_cc_event_handlers        |
|   app_cli.c               |   app_cli.c                    |
|   app_cli.h               |   app_cli.h                    |
|   app_credentials.h       |   app_credentials.h            |
|   app.c                   |   app.c                        |
|   events.h                |   events.h                     |
|   main.c                  |   main.c                       |

The main differences are:
- app_btn_handler.c (API change)
- app_cc_battery_callbacks.c
- app_cc_door_lock_callbacks.c (API change)

Crucially, with this release, application related sources files like `platform\SiliconLabs\AppsHw\src\DoorLockKeyPad\DoorLockKeyPad_hw.c` were removed. All application related logic was moved to the application project's directory. In this case, **DoorLockKeyPad_hw.c** was split into three separated sources: **app_btn_handler.c**, **app_cc_battery_callbacks.c** and **app_cc_door_lock_callbacks.c**. 

#### Migrating the app_btn_handler.c file

In previous releases, button and LED configurations were spread out across multiple files. From the 7.23 release, Silicon labs switched to the standard SiSDK components to handle those peripherals, such as sl_simple_button and sl_simple_led. Because of those components, we can use Simplicity Studio's Pin Tool to remap a button or LED pin in the application. There is no need to do any additional configuration regarding these peripherals.

##### Buttons
Previously, button handling was implemented in `platform\SiliconLabs\AppsHw\src\<application>\<application>_hw.c` source files, in this case **DoorLockKeyPad_hw.c**. Now, this logic can be found in the application's directory, in **app_btn_handler.c**. A helper SiSDK component, called app_button_press, is also used to generate events on button pressing, such as short, medium or long press events. The duration of these events can be configured in **app_button_press_config.h** header file, in the config directory inside of the application project's directory. In case you had any custom logic regarding the button handling in `platform\SiliconLabs\AppsHw\src\<application>\<application>_hw.c`, in this example, **DoorLockKeyPad_hw.c**, please move them to the app_btn_handler.c file in the application project's directory.

In app_btn_handler.c, only the **app_button_press_btn_0_handler** or **app_button_press_btn_1_handler** functions need to be handled, which take the duration as an input parameter. Those callbacks are called automatically when a button event is fired.

##### LEDs

LED handling was simplified, which means LEDs can be manipulated via sl_simple_led API calls. They can also be configured using the Pin Tool. No additional configuration is required. In case you are using functions prefixed with **Board_** to manipulate LEDs or GPIOs, please use the sl_simple_led and em_gpio components' API from 7.23 release.

#### Migrating the app_cc_<command class>_callbacks.c files

There are new source files in every application, named in the pattern `app_cc_<command class name>_callback.c`. In the new structure, each of those sources contain callbacks for a specific command class. These callbacks can be different in every application, e.g. battery report callback, which can be different in a real life application compared to a sample application. From the 7.23 release, such callbacks are exposed on the application level. In this example, battery command class related callbacks are in the application project's directory, named **app_cc_battery_callback.c**. If you had any custom implementation of command class specific callbacks in `platform\SiliconLabs\AppsHw\src\<application>\<application>_hw.c` files, in this case **DoorLockKeyPad_hw.c**, please move those implementations next to the application, into the related source file.
