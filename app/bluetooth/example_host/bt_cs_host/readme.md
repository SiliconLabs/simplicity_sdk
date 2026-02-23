# CS host application
---

This is the host application for the Channel Sounding (CS) NCP target application.

## Features

* Control of the multi role CS NCP
  * Create initiator and/or reflector instances
  * Configure wired or wireless antenna setup
  * Configure CS mode
  * Configure object tracking mode
  * Configure antenna selection for antenna switching
  * Configure connection PHY
  * Use reflector address filtering
* Display only measurement results
* Use BGAPI Trace to get logging data

In two way CS, both the initiator and the reflector instances are collecting IQ samples. The initiator starts and orchestrate the whole procedure. The reflector component responds to the initiator, and sends its measurement data to the initiator. When all data collected, the initiator estimates the distance.

The host application is extended with application specific messages that we refer to as ACP (Application Co-Processor) messages.
The following ACP commands are sent from the host to the target device:
* Create initiator instance
* Delete initiator instance
* Create reflector instance
* Delete reflector instance
* Configure antenna
* Enable BGAPI Trace

The following ACP events are sent from the target device to the host:
* CS results
* CS intermediate results
* CS extended results
* Error events

## Limitations, known issues

- Max 4 initiator/reflector instances are supported (-I4 -R4)

## Multiconnection

- Default setup is optimized for 1-1 connection, multiconnection setup requires modification of the timing parameters to operate as expected. Timing can be adjusted by the procedure_interval and connection_interval parameters.
- Use the following calculation for 1-N connection: procedure_time_1_N[ms] = connection_interval[ms] * procedure_interval * N
- Note that setting CS_INITIATOR_DEFAULT_MIN/MAX_CONNECTION_INTERVAL and CS_INITIATOR_DEFAULT_MIN/MAX_PROCEDURE_INTERVAL will only take effect if CS_INITIATOR_DEFAULT_PROCEDURE_SCHEDULING is set to CS_PROCEDURE_SCHEDULING_CUSTOM. Otherwise these parameters are managed by the application.
- If getting frequent measurement results is not priority, it's safe to use procedure_interval = 120 and connection_interval = 24 even with the maximum number of connections (4).

## Getting started

### Prerequisites

To compile the sources, you need the following tools:

* GCC and libc, or other suitable C compiler for the host
* make utility

### Building the application
On the target: Build and flash the "bt_cs_ncp" application.
On the host: Build the application by issuing the following command:

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
* -R <max_reflector_instances>
  Maximum number of reflector instances, default: 1
* -I <max_initiator_instances>
  Maximum number of initiator instances, default: 1
* -p <channel_map_preset>
  Pre-set parameters for channel map selection, default: 2
  0 : low (channel spacing: 1, number of channels: 20)
  1 : medium (channel spacing: 2, number of channels: 38)
  2 : high (channel spacing: 1, number of channels: 72)
  3 : load custom from configuration macro CS_CUSTOM_CHANNEL_MAP
* -a <antenna_configuration_index>
  Antenna configuration index for antenna switching, default: 7
  0 : Single antennas on both sides
  1 : Dual antenna initiator & single antenna reflector
  4 : Single antenna initiator & dual antenna reflector
  7 : Dual antennas on both sides
  Note: considered only with CS main mode: PBR!
* -q <cs_sync_antenna>
  Antenna usage for CS SYNC packets, default: 0xFE
  1 : use antenna ID1 only
  2 : use antenna ID2 only
  0xFE : Switching between antennas for each channel
  Note: considered only with CS main mode: RTT!
* -T <trace>
  Enable RTT trace including BGAPI messages and RTL log
  Note that the RTT blocks the target if no client is connected
* -P <connection_phy>
  Use 1M connection PHY
  Note: Default is 2M
* -F <reflector_ble_address>
  Enable reflector BLE address filtering
* -r  Enable RSSI distance measurement
* -w  Use wired antenna offset
* -o <object_tracking_mode>
  Object tracking setting, default: 0
  0 : Use moving object tracking (up to 5 km/h) setting
  1 : Use stationary object tracking settings
* -h  Print help message.

### Running the application

To run the application with the default options, use the following command line. Replace the serial device with your tty/COM device.

`./exe/bt_cs_host -u /dev/tty.usbmodem0004402717881`

This will create one initiator and one reflector instance on the NCP.

The initiator instances are scanning for usable reflectors, and initiate a connection to it. When connected, the Initiator component starts the CS procedure. The initiator estimates the distance, and displays them in the command line terminal. Reflector instances are advertising with device name "CS RFLCT".