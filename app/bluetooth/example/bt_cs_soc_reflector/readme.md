# SoC - CS Reflector

The Bluetooth SoC-CS Reflector is a project that can be used to test the Channel Sounding (CS) feature. In the provided CS setup, the reflector measures the phase (in PBR mode) or flying time (RTT) of the received signal and sends this information back to the initiator using the Ranging Service (RAS). The reflector has the RAS Server included which can operate in two different modes: real-time and on-demand. The setting of this mode is available in the initiator config. The initiator will estimate the distance, the estimated values are not available on the reflector side.

## Getting Started
After startup it will advertise the name "CS RFLCT", and will provide the Ranging Service (RAS).
Measurement data is collected on the initiator side.
The Reflector can connect to multiple initiators (maximum 4).
Reflector specific default values can be found and modified in the component configuration file (config/cs_reflector_config.h).
CS mode and object tracking mode is configured on the initiator side.

## Usage
- Generate the project
- Configure values if needed
- Build and flash the sample application

## Resource optimization
- Flash usage can be reduced by
  - turning off some of the "Supported features" in "CS Ranging Service Server" component. Note that "Real-Time Ranging Data" feature is used by default on the Initiator,
  - turning off "Logging" in "CS Reflector" component or completely in "Application"- "Utility" -"Log" component,
- RAM usage can be reduced by 
  - decreasing the "Max number of connections reserved for user" in Bluetooth "Connection" component configuration to the required amount.
  - decreasing "Procedure maximum length" or "Procedure per connection" "CS Ranging Service Server" component configuration. Note that reducing "Procedure maximum length" also affects the maximum number of procedures that can be stored in the buffer in case of RAS On-Demand mode.


## Troubleshooting

This sample application does need a bootloader (AppLoader OTA DFU).

## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).