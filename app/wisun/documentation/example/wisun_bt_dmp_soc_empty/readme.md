# Wi-SUN Bluetooth DMP - SoC Empty

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
  - [Materials](#materials)
- [Project Setup](#project-setup)
  - [Wi-SUN state machine](#wi-sun-state-machine)
  - [Bluetooth Low Energy state machine](#bluetooth-low-energy-state-machine)
- [Development](#development)
  - [Designing the GATT Database](#designing-the-gatt-database)
  - [Responding to Wi-SUN Events](#responding-to-wi-sun-events)
  - [Responding to Bluetooth Events](#responding-to-bluetooth-events)
  - [Implementing Application Logic](#implementing-application-logic)
- [Push-button actions](#push-button-actions)
- [LED status indications](#led-status-indications)
- [Troubleshooting](#troubleshooting)
- [Resources](#resources)
  - [Wi-SUN documents](#wi-sun-documents)
  - [Bluetooth Low Energy documents](#bluetooth-low-energy-documents)
- [Report Bugs & Get Support](#report-bugs--get-support)

## Introduction

The Wi-SUN Bluetooth DMP - SoC Empty example project serves as a starting point for developing Wi-SUN and Bluetooth Low Energy (BLE) Dynamic Multiprotocol (DMP) applications.

This example can be tested using the **Simplicity Connect** smartphone application, available on [iOS](https://apps.apple.com/us/app/simplicity-connect/id1030932759) and [Android](https://play.google.com/store/apps/details?id=com.siliconlabs.bledemo&hl=en&gl=US).

> Note: This example **does not** expect a specific Gecko Bootloader to be present on your device.

## Prerequisites

The provided method is only applicable for **EFR32xG28** devices which support Bluetooth Low Energy and Wi-SUN protocols.

## Getting Started

The principle is the same for all Bluetooth DMP (Dynamic Multiprotocol) applications: the device is able to communicate via Bluetooth and via Wi-SUN at the same time, making it possible to control the DMP device with different type of devices.

### Materials

- To get started with Wi-SUN and Simplicity Studio, see: [QSG181: Wi-SUN SDK Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf).

- To get started with Silicon Labs Bluetooth and Simplicity Studio, see: [QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf).

- To learn more about programming an SoC application, see: [UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf).

## Project Setup

![Project Setup](readme_img1.png)

As the name implies, the example is an **almost** empty template (with additional button press functionality and status LED indication) that has the bare minimum to make a working Wi-SUN - BLE DMP application. For this purpose, the example includes the platform button and LED drivers, Wi-SUN and BLE stack components and its dependencies (RTOS, cryptographic library...).

### Wi-SUN state machine

In this example, the Wi-SUN stack is initialized with only the necessary components, and it automatically joins a Wi-SUN network upon startup.

![Wi-SUN state machine](readme_img3.png)

### Bluetooth Low Energy state machine

On the BLE side the stack is initialized, basic functions are implemented and user interaction is provided through buttons to enable/disable advertisement with additional status LED indications.

![BLE state machine](readme_img4.png)

## Development

This example implements a basic project with Bluetooth Low Energy and Wi-SUN protocols running in parallel on the device. The project is based on a Real Time Operating System (Micrium OS or FreeRTOS according to your choice). Callbacks for Bluetooth and Wi-SUN event handling are added, but not implemented (except a very basic functionalities). The developer can add any functionality.

The development of a Wi-SUN - BLE DMP application consists of four main steps:

- [Designing the GATT Database](#designing-the-gatt-database)
- [Responding to the events raised by the Wi-SUN stack](#responding-to-wi-sun-events)
- [Responding to the events raised by the Bluetooth stack](#responding-to-bluetooth-events)
- [Implementing additional application logic](#implementing-application-logic)

These steps are covered in the following sections.

For more information about using Real Time Operating Systems with Bluetooth, see [AN1260: Integrating v3.x Silicon Labs Bluetooth Applications with Real-Time Operating Systems](https://www.silabs.com/documents/public/application-notes/an1260-integrating-v3x-bluetooth-applications-with-rtos.pdf).

### Designing the GATT Database

This example implements a basic GATT database. GATT definitions (services/characteristics) can be extended using the GATT Configurator, which can be found under Advanced Configurators in the Software Components tab of the Project Configurator. To open the Project Configurator, open the .slcp file of the project.

![Opening GATT Configurator](readme_img2.png)

To learn how to use the GATT Configurator, see [UG438: GATT Configurator User’s Guide for Bluetooth SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug438-gatt-configurator-users-guide-sdk-v3x.pdf).

### Responding to Wi-SUN Events

A Wi-SUN application is event-driven. The Wi-SUN stack generates events when a connection is successful, data has been sent or an IP packet is received. The application has to handle these events in the `sl_wisun_on_event()` function. The prototype of this function is implemented in *app.c*. To handle more events, the switch-case statement of this function must be created and extended. For the list of Wi-SUN events, see [Wi-SUN API Reference](https://docs.silabs.com/wisun/latest/).

### Responding to Bluetooth Events

A Bluetooth application is event driven. The Bluetooth stack generates events e.g., when a remote device connects or disconnects or when it writes a characteristic in the local GATT database. The application has to handle these events in the `sl_bt_on_event()` function. The prototype of this function is implemented in *app.c*. To handle more events, the switch-case statement of this function is to be extended. For the list of Bluetooth events, see the online [Bluetooth API Reference](https://docs.silabs.com/bluetooth/latest/).

### Implementing Application Logic

Additional application logic can be implemented in the *app_task()* function. Find the definition of this function in *app.c*. The *app_task()* function is called once when the device is booted and after the Wi-SUN stack initialization.

The first step in most Wi-SUN applications is to call *sl_wisun_join()* to connect the Wi-SUN device to a Wi-SUN border router. The remaining implementation is up to the developer. Refer to the [Wi-SUN API Reference](https://docs.silabs.com/wisun/latest/) to check the list of Wi-SUN APIs available to the application.

## Push-button actions

| Description | Main Board Button |
|---|---|
| Toggle BLE advertisement | PB0/BTN0 **or** PB1/BTN1 |

## LED status indications

| Description | Main Board LED |
|---|---|
| LED blinks to indicate that BLE advertisement is enabled | LED0 |

## Troubleshooting

Before programming the radio board mounted on the WSTK, ensure the power supply switch is in the AEM position (right side), as shown.

![Radio Board Power Supply Switch](readme_img0.png)

## Resources

### Wi-SUN documents

- [Wi-SUN Stack API documentation](https://docs.silabs.com/wisun/latest)
- [AN1330: Wi-SUN Mesh Network Performance](https://www.silabs.com/documents/public/application-notes/an1330-wi-sun-network-performance.pdf)
- [AN1332: Wi-SUN Network Setup and Configuration](https://www.silabs.com/documents/public/application-notes/an1332-wi-sun-network-configuration.pdf)
- [AN1364: Wi-SUN Network Performance Measurement Application](https://www.silabs.com/documents/public/application-notes/an1364-wi-sun-network-performance-measurement-app.pdf)
- [QSG181: Wi-SUN Quick-Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg181-wi-sun-sdk-quick-start-guide.pdf)
- [UG495: Wi-SUN Developer's Guide](https://www.silabs.com/documents/public/user-guides/ug495-wi-sun-developers-guide.pdf)

### Bluetooth Low Energy documents

- [Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)
- [UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)
- [QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)
- [UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)
- [AN1260: Integrating v3.x Silicon Labs Bluetooth Applications with Real-Time Operating Systems](https://www.silabs.com/documents/public/application-notes/an1260-integrating-v3x-bluetooth-applications-with-rtos.pdf)
- [AN1269: Dynamic Multiprotocol Development with Bluetooth and Proprietary Protocols on RAIL in GSDK v3.x](https://www.silabs.com/documents/public/application-notes/an1269-bluetooth-rail-dynamic-multiprotocol-gsdk-v3x.pdf)
- [UG305: Dynamic Multiprotocol User’s Guide](https://www.silabs.com/documents/public/user-guides/ug305-dynamic-multiprotocol-users-guide.pdf)
- [Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).
