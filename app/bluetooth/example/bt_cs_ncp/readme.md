# NCP - CS

This is a Network Co-Processor (NCP) based target application. It serves as a target application for Channel Sounding (CS) applications. The corresponding host sample applications can be found in the following folder: app/bluetooth/example_host/. This application is suitable for the bt_cs_host and the bt_cs_host_initiator_legacy host applications. The NCP integrates the initiator and reflector components, as well as a full-featured NCP. The application can be used as an NCP with the CS component.

In legacy mode, the host application operates as a CS initiator. In this mode, the RTL is active on the host side, and this application acts as a traditional NCP with the CS component active. CS procedure data is transferred via BGAPI.

In normal mode, the distance estimation is performed at the NCP side, and only the measurement results are sent to the host using special CS-specific BGAPI commands. In this mode, the host and the NCP can also act as a reflector.

## Getting Started with NCP

The NCP Host and Target communicate via a serial interface (UART), which can be tunneled either via USB or via Ethernet if you use a development kit. The communication between the NCP Host and Target is defined in the Silicon Labs proprietary protocol called BGAPI. BGLib is the C reference implementation of the BGAPI protocol, which is to be used on the NCP Host side.

[AN1259: Using the v3.x Silicon Labs Bluetooth Stack in Network Co-Processor Mode](https://www.silabs.com/documents/public/application-notes/an1259-bt-ncp-mode-sdk-v3x.pdf) provides a detailed description how NCP works and how to configure it for your custom hardware.

## Interface

The NCP provides a high level interface to host applications. To configure and start a reflector or initiator instance in the NCP, the host application shall use the

sl_bt_user_cs_service_message_to_target() calls.

Any asynchronous event triggers a sl_bt_evt_user_cs_service_message_to_host_id event at the host side. These events must be further parsed by the host application to get the exact event. These events can be

* CS results
* RTL library log segments
* Error events

All interface related data types are defined in cs_acp.h.

## Usage

Build and flash the application. Use the "bt_cs_host_initiator_legacy" or the "bt_cs_host" Host sample application to connect to it. In legacy mode, the Host application will scan for a device running the "CS RFLCT" sample application. When found, the initiator will create a connection between them and will start the distance measurement process. The initiator estimates the distance, and display them in the command line terminal.

## Troubleshooting

### Bootloader Issues

Note that this Example Project does not include a bootloader. However, this Example Project expects a bootloader to be present on the device in order to support device firmware upgrade (DFU). To get your application to work, you should either
- flash the proper bootloader or
- remove the DFU functionality from the project.

**If you do not wish to add a bootloader**, then remove the DFU functionality by uninstalling the *Bootloader Application Interface* software component -- and all of its dependants. This will automatically put your application code to the start address of the flash, which means that a bootloader is no longer needed, but also that you will not be able to upgrade your firmware.

**If you want to add a bootloader**, then either
- Create a *BGAPI UART DFU* bootloader project, build it and flash it to your device.

- or run a precompiled Demo on your device from the Launcher view before flashing your application. Precompiled demos flash both bootloader and application images to the device. Flashing your own application image after the demo will overwrite the demo application but leave the bootloader in place.
  - For NCP and RCP projects, flash the *Bluetooth - NCP* demo.
  - For SoC projects, flash the *Bluetooth - SoC Thermometer* demo.

**Important Notes:**
- when you flash your application image to the device, use the *.hex* or *.s37* output file. Flashing *.bin* files may overwrite (erase) the bootloader.

This sample application does need a bootloader.

### Programming the Radio Board

Before programming the radio board mounted on the mainboard, make sure the power supply switch the AEM position (right side) as shown below.

![](./image/readme_img0.png)

## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth SDK v3.x Quick-Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[AN1259: Using the v3.x Silicon Labs Bluetooth Stack in Network Co-Processor Mode](https://www.silabs.com/documents/public/application-notes/an1259-bt-ncp-mode-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).