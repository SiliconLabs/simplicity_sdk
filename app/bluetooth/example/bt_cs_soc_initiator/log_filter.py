#!/usr/bin/env python3
"""
Log filter tool
"""

# Copyright 2024 Silicon Laboratories Inc. www.silabs.com
#
# SPDX-License-Identifier: Zlib
#
# The licensor of this software is Silicon Laboratories Inc.
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

import argparse
import os
import re
import socket
import sys
import time
from contextlib import contextmanager
import serial.tools.list_ports

# Filter regex pattern with exactly one group
FILTER_PATTERN = r"\[RTL\] (.+)"

def get_serial_device():
    """Find Segger J-Link device based on USB vendor ID."""
    devices = [com.device for com in serial.tools.list_ports.comports() if com.vid == 0x1366]
    if not devices:
        return None
    if len(devices) == 1:
        return devices[0]
    # Let the user select a serial device
    while True:
        for i, device in enumerate(devices):
            print(f"{i + 1}: {device}")
        try:
            num = int(input(f"Select a device from the list [1 - {len(devices)}]: "))
            if num not in range(1, len(devices) + 1):
                raise ValueError
            return devices[num - 1]
        except (ValueError, IndexError):
            print("Invalid input")
        except KeyboardInterrupt:
            print()
            return None

@contextmanager
def open_any(source):
    """Open regular file, serial port or TCP port.
    
    The returned object is guaranteed to implement the readline() method returning bytes object."""
    client = None
    io_stream = None

    if os.path.exists(source):
        # Treat source as a regular file
        io_stream = open(source, mode="rb")
    else:
        try:
            # Check for a valid IPv4 address
            socket.inet_aton(source)
            client = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        except OSError:
            # Assume serial port
            io_stream = serial.Serial(source)
        if client:
            client.connect((source, 4901))
            io_stream = client.makefile(mode="rb")

    try:
        yield io_stream
    finally:
        io_stream.close()
        if client:
            client.close()


def main():
    examples = (
        "examples:"
        "\n  %(prog)s                 Try to autodetect serial port"
        "\n  %(prog)s COM4            Open serial port on Windows"
        "\n  %(prog)s /dev/ttyACM0    Open serial port on POSIX"
        "\n  %(prog)s 192.168.1.10    Open TCP port"
        "\n  %(prog)s path/to/logfile Open log file")
    parser = argparse.ArgumentParser(description=__doc__,
                                     epilog=examples,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        "source",
        nargs="?",
        help="Log file, serial port or TCP port. See the examples for details."
    )
    parser.add_argument(
        "-o", "--stdout",
        choices=["all", "nortl", "rtlonly"],
        default="all",
        help="Standard output mode"
    )
    args = parser.parse_args()
    source = args.source
    if not source:
        source = get_serial_device()
        if not source:
            parser.print_usage()
            print(f"{parser.prog}: error: No serial device found."
                   " Please specify source explicitly.")
            sys.exit(-1)
        print(f"Serial device selected: {source}")

    # Log file names contain the actual date time info in the format YYYY-MM-DDThhmmss
    timestamp = time.strftime("%Y-%m-%dT%H%M%S")
    rtl_log_file = f"rtl_log_{timestamp}.jsonl"
    full_log_file = f"log_{timestamp}.txt"

    with open_any(source) as src, \
         open(rtl_log_file, mode="w", encoding="utf-8") as rtl_log, \
         open(full_log_file, mode="w", encoding="utf-8") as full_log:
        while True:
            try:
                line = src.readline().decode()
            except KeyboardInterrupt:
                break
            if not line:
                break
            full_log.write(line)
            if args.stdout == "all":
                print(f"{line}", end="")
            match = re.match(FILTER_PATTERN, line)
            if match:
                rtl_log.write(match.group(1) + "\n")
                if args.stdout == "rtlonly":
                    print(f"{match.group(1)}")
            else:
                if args.stdout == "nortl":
                    print(f"{line}", end="")

if __name__ == "__main__":
    main()
