# Secure Video Decoder - MITRE eCTF

This repository contains a secure video decoder implementation for the MAX78000 microcontroller, developed by Team Flinders for the MITRE Embedded Capture the Flag (eCTF) competition.

## Project Overview

The secure video decoder provides a platform for securely processing and displaying video content on the MAX78000 platform. This implementation focuses on security fundamentals such as:

- Secure boot with anti-timing attack measures
- Anti-tamper protection
- Secure data handling
- Hardware-based security features of MAX78000
- FreeRTOS integration for task management

## Team Flinders

This project was developed by Team Flinders for the MITRE eCTF competition. The implementation leverages the security features of the MAX78000 to create a robust and secure video decoder platform.

## Hardware Requirements

This implementation is designed exclusively for:
- MAX78000FTHR (FTHR_RevA)

## Prerequisites

To build and run this project, you'll need:

- [Analog Devices MSDK](https://analogdevicesinc.github.io/msdk/) installed
- ARM GCC Toolchain
- OpenOCD for programming
- Visual Studio Code with MSDK extensions (for IDE support)

## Security Features

### Secure Boot
- Implements randomized boot delay to prevent timing-based attacks
- Hardware RNG for cryptographic operations

### Runtime Protection
- FreeRTOS task isolation
- Secure I/O handling

## Hardware Connections

For MAX78000FTHR (FTHR_RevA):
- Connect a USB cable between the PC and the CN1 (USB/PWR) connector
- Open a terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1

## Project Structure

- `src/`: Source code files
  - `main.c`: Main application entry point, secure boot implementation
  - `FreeRTOS_hooks.c`: FreeRTOS hook implementations
  - Other security and driver implementation files
- `include/`: Header files
  - `FreeRTOSConfig.h`: FreeRTOS security configuration
- `.vscode/`: VS Code configuration files for build and debug

## MITRE eCTF

The MITRE Embedded Capture the Flag (eCTF) is a competition that challenges teams to design and implement a secure embedded system. This project represents Team Flinders' solution for the secure video decoder challenge.

## License

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Where other licensed code has been used, the licence relating to that
overrides the Apache License, Version 2.0 that applies to all code
written by Team Flinders.
