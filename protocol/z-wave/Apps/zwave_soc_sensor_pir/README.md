# Sensor PIR

The Z-Wave certified Sensor PIR application shows a presence/movement detector implementation for controlling other devices and for sending notifications.

| <!-- -->                  | <!-- -->                                  |
|---------------------------|-------------------------------------------|
| Role Type                 | Reporting Sleeping End device (RSS)       |
| Supporting Device Type    | Data reporting                            |
| Device Type               | Notification sensor                       |
| Generic Type              | Sensor Notification                       |
| Specific Type             | Notification Sensor                       |
| Requested security keys   | S2_UNAUTHENTICATED, and S2_AUTHENTICATED  |

Sensor PIR transmits the following notification types/events:

-   Home Security
    -   Motion detection (unknown location)

Sensor PIR supports the "push mode" only of Notification CC.

## Supported Command Classes

Sensor PIR implements mandatory and some optional command classes. The table below lists the supported Command Classes, their version, and their required Security class, if any.

| Command Class             | Version | Required Security Class        |
| ------------------------- |:-------:| ------------------------------ |
| Association               |    2    | Highest granted Security Class | 
| Association Group Info    |    3    | Highest granted Security Class |
| Battery                   |    1    | Highest granted Security Class |
| Device Reset Locally      |    1    | Highest granted Security Class |
| Firmware Update Meta Data |    5    | Highest granted Security Class |
| Indicator                 |    3    | Highest granted Security Class |
| Manufacturer Specific     |    2    | Highest Granted Security Class |
| Multi-Channel Association |    3    | Highest granted Security Class |
| Notification              |    8    | Highest granted Security Class |
| Powerlevel                |    1    | Highest granted Security Class |
| Security 2                |    1    | None                           |
| Supervision               |    1    | None                           |
| Transport Service         |    2    | None                           |
| Version                   |    3    | Highest granted Security Class |
| Wake Up                   |    2    | Highest granted Security Class |
| Z-Wave Plus Info          |    2    | None                           |

## Basic Command Class mapping

Basic Command Class is not mapped to any of the supported command classes

## Association Group configuration

Application Association Group configuration

<table>
<tr>
    <th>ID</th>
    <th>Name</th>
    <th>Node Count</th>
    <th>Description</th>
</tr><tr>
    <td>1</td>
    <td>Lifeline</td>
    <td>X</td>
    <td>
        <p>Supports the following command classes:</p>
        <ul>
            <li>Device Reset Locally: triggered upon reset.</li>
            <li>Battery: triggered upon low battery.</li>
            <li>
                Notification: triggered upon a movement detection (simulated by
                button BTN2). After a while, a cancel notification will be issued.
            </li>
            <li>Indicator Report: Triggered when LED1 changes state.</li>
        </ul>
    </td>
</tr><tr>
    <td>2</td>
    <td>Basic Set</td>
    <td>Y</td>
    <td>
        Upon a movement detection (simulated by button BTN2), nodes
        associated in this group will first receive a Basic Set with 0xFF (turn on)
        and after a while receive a Basic Set with 0x00 (turn off).
    </td>
</tr>
</table>

X: For Z-Wave node count is equal to 5 and for Z-Wave Long Range it is 1.
Y: For Z-Wave node count is equal to 5 and for Z-Wave Long Range it is 0.

## Usage of Buttons and LED Status

<table>
<tr>
    <th rowspan="2">Button</th>
    <th rowspan="2">Action</th>
    <th colspan="2">Description</th>
</tr><tr>
    <th>Radio Board <sup>1</sup></th>
    <th>Thunderboard <sup>1</sup></th>
</tr><tr>
    <td>RST</td>
    <td>Press</td>
    <td colspan="2">
        Resets the firmware of an application (like losing power). All volatile memory will be cleared.<br>
        Sends Wake Up Notification.
    </td>
</tr><tr>
    <td>BTN0<sup>2</sup></td>
    <td>Press</td>
    <td colspan="2">Sends Battery Report (only if the device is not sleeping)</td>
</tr><tr>
    <td rowspan="3">BTN1<sup>2</sup></td>
    <td>Press</td>
    <td colspan="2">
        Enter "learn mode" (sending node info frame) to add/remove the device.<br>
        Removing the device from a network will reset it.
    </td>
</tr><tr>
    <td>Hold for at least 1 second and release</td>
    <td></td>
    <td>Simulates a "Motion detected" event.</td>
</tr><tr>
    <td>Hold for at least 5 seconds and release</td>
    <td colspan="2">Perform a reset to factory default operation of the device, and a Device Reset Locally Notification Command is sent via Lifeline.</td>
</tr><tr>
    <td>BTN2</td>
    <td>Hold for at least 1 second and release</td>
    <td>Simulates a "Motion detected" event.</td>
    <td></td>
</tr>
</table>

<table>
<tr>
    <th>LED</th>
    <th>Description</th>
</tr><tr>
    <td>LED1</td>
    <td>
        Blinks with 1 Hz when learn mode is active.<br>
        Used for Indicator Command Class.
    </td>
</tr>
</table>

<sup>1</sup>: A Radio Board is plug-in board for the Wireless Pro Kit Mainboard.
A Thunderboard is a stand-alone kit with a direct USB Type-C connection.

<sup>2</sup>: The application needs to be woken up for these operations.
It is not possible to wake up the BRD4400C and BRD4401C radio boards using BTN0
and BTN1, and the BRD2603A Thunderboard using BTN1.\
To support these functionalities, the affected buttons must be remapped to pins
that support waking up the chip from EM4 sleep mode.
Please refer to your hardware's documentation for further information.

## Firmware Update

This section will describe backward compatibility when upgrading the Switch On/Off application from one SDK to a newer version.


## CLI Support

In case CLI support is needed pelase install zw_cli_common component to the project. Please note the zw_cli_common component will modify the power consumption in case of sleeping applications. Like door lock keypad, sensor pir or multilevel sensor. CLI cannot work with sleep mode, after a reset the application stays awake until the user issues the enable_sleeping command. From that point CLI won't work and sleep mode will be reached until the next reset.

<table>
<tr>
    <th>Command</th>
    <th>Arguments</th>
    <th>Description</th>
</tr>
<tr>
    <th>set_learn_mode</th>
    <td>It can be start or stop</td>
    <td>Starting or stopping the learn mode</td>
</tr>
<tr>
    <th>factory_reset</th>
    <td>-</td>
    <td>Executing factory reset</td>
</tr>
<tr>
    <th>get_dsk</th>
    <td>-</td>
    <td>Printing out the generated DSK of the device</td>
</tr>
<tr>
    <th>get_region</th>
    <td>-</td>
    <td>Printing out the set region of the application</td>
</tr>
<tr>
    <th>battery_report</th>
    <td>-</td>
    <td>Sending a battery report</td>
</tr>
<tr>
    <th>motion_detected</th>
    <td>-</td>
    <td>Triggering a motion detected event</td>
</tr>
<tr>
    <th>enable_sleeping</th>
    <td>-</td>
    <td>Lets the application go into sleep mode. After this command the CLI won't work until the next reset</td>
</tr>
</table>