# CS legacy host application
---

This is an initiator host Channel Sounding (CS) application

## Features

* CS Initiator functionality
* Distance calculation is done on the host side, in this application
* Displaying measurement results
* Writing log data from the RTL

## Limitations, known issues

- Only one initiator instance is supported

## Getting started

### Prerequisites

To compile the sources, you need the following tools:

* GCC and libc
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
* -F <reflector_address>
  Reflector filter address.
* -r  Enable RSSI distance measurement
* -w  Use wired antenna offset
* -o  Object tracking mode, default: 1
  1 : moving object tracking (up to 5 km/h) (REAL_TIME_BASIC)
  2 : stationary object tracking (STATIC_HIGH_ACCURACY)
* -h  Print help message.

### Running the application

To run the application with the default options, use the following command line. Replace the serial device with your tty/COM device.

`./exe/bt_cs_host_legacy -u /dev/tty.usbmodem0004402717881`

This will create an initiator instance.

The application starts scanning for usable reflectors, and initiates a connection to it. When connected, it starts measuring distance.
