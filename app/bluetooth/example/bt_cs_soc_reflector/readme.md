# SoC - CS reflector

The Bluetooth SoC-CS reflector is a project that can be used to test the Channel Sounding (CS) feature. In the provided CS setup, the reflector measures the phase (in PBR mode) or flying time (RTT) of the received signal and sends this information back to the initiator using RAS. The initiator will estimate the distance, the measured values are not available on the reflector side.

## Getting Started
Build and flash the sample application.
After startup it will advertise the name "CS RFLCT", and will provide the "CS Test Service" over GATT.

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