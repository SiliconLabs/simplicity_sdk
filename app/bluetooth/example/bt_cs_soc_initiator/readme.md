# SoC - CS Initiator

The Bluetooth SoC-CS initiator is a project that can be used to test the Channel Sounding (CS) feature. In the provided CS setup, the application creates a connection to a reflector (Running on the host or SoC), measures the phase (PBR) or timing (RTT) and estimates the distance.

## Usage
Build and flash the sample application.
After startup it will scan for a device running the "CS RFLCT"" sample application. When found, the initiator will create a connection between them and will start the distance measurement process. The initiator will calculate the distance, display them on the LCD and also send them via UART.

![](./image/cs_lcd.png)

## Known issues and limitations

* Only one initiator instance created

## Troubleshooting

This sample application does need a bootloader (AppLoader OTA DFU).

## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

