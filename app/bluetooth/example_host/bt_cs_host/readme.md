# CS host application
---

This is the host application for the Channel Sounding (CS) NCP target application.

## Features

* Control of the multi role CS NCP
  * Creating initiator instance
  * Creating reflector instances
* Distance estimation is done on the NCP target side (On the Wireless MCU)
* Displaying measurement results
* Writing log data from the RTL

In two way CS, both the initiator and the reflector instances collects IQ samples. The initiator starts and orchestrate the whole procedure. The reflector component responds to the initiator, and sends its measurement data to the initiator. When all data collected, the initiator estimates the instance.

## Limitations, known issues

- Only one initiator instance is supported (-I1)
- In case of high data flow from the RTL logging functionality, the VCOM interface can experience congestion. The best way to sort this out is to increase the throughput of the NCP VCOM as described in [this article](https://community.silabs.com/s/article/wstk-virtual-com-port-baudrate-setting?language=en_US). The baudrate is configured using the SL_UARTDRV_USART_VCOM_BAUDRATE macro. After changing the WSTK VCOM speed, please don't forget to update the VCOM Baud rate configuration of the CS NCP accordingly, then re-build and re-flash the target with the new firmware.

## Getting started

### Prerequisites

To compile the sources, you need the following tools:

* GCC and libc, or other suitable C compiler for the host
* make utility

### Building the application
Build the application by issuing the following command:

`make`

in the project's root directory. This will build your executable, and place it in the exe folder. You can clean the build products by issuing:

`make clean`

### Command line arguments

* -u \<serial_port\>
  Serial port assigned to the dev board by the host system. (COM# on Windows, /dev/tty# on POSIX)
* -b \<baud_rate\>
  Baud rate of the serial connection.
  default: 115200
* -m \<cs_mode\>
  Number corresponding to the selected CS mode, default: 2, PBR
  1 : RTT
  2 : PBR
* -t \<tcp_address\>
  TCP/IP connection option.
  TCP/IP address of the dev board.
* -f
  Disable flow control (RTS/CTS), default: enabled
* -l <level>
  Application log level filter.
  Integer representing the log level, default: 3.
  0 : Critical.
  1 : Critical, error.
  2 : Critical, error, warning.
  3 : Critical, error, warning, info.
  4 : Critical, error, warning, info, debug.
* -R  <max_reflector_instances>
  Maximum number of reflector instances, default: 1
* -I  <max_initiator_instances>
    Maximum number of initiator instances, default: 1

* -F <reflector_ble_address>
  Enable reflector BLE address filtering
* -r  Enable RSSI distance measurement
* -w  Use wired antenna offset
* -o
  Object tracking setting, default: 1
  1: Use moving object tracking (up to 5 km/h) setting
  2: Use stationary object tracking settings
* -h  Print help message.

### Running the application

To run the application with the default options, use the following command line. Replace the serial device with your tty/COM device.

`./exe/bt_cs_host -u /dev/tty.usbmodem0004402717881`

This will create one initiator and one reflector instance on the NCP.

The initiator instance starts to scan for usable reflectors, and initiates a connection to it. When connected, the Initiator component starts measuring distance.

The reflector instance starts advertising.
