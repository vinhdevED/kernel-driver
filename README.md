# Linux Kernel Driver Implementation in both Raspberry Pi 3b+ and ZynqMP ZCU102(fpga)
`⬇️ The project has the spiritual support of NguyenThiKimAnh 💌 aka honey ⬇️`
**`#VHT`**
## Overview
This repository contains the implementation of Linux Kernel Drivers for both _**`Raspberry Pi 3B+ (Broadcom BCM2837)`**_ and _**`ZynqMP ZCU102 (Xilinx FPGA-based SoC)`**_. The project aims to provide a unified driver architecture that supports embedded platforms with different architectures: ARM Cortex-A53 (RPi 3B+) and ARM Cortex-A53/Cortex-R5 (ZynqMP).

The drivers in this repository are designed to interact with hardware peripherals and memory-mapped IO, focusing on:
- GPIO control for low-level hardware interaction
- I2C for slave-master control
- BRAM-based data exchange between kernel space and hardware accelerators
- User-space API for seamless communication with the driver
  
This implementation ensures modularity and portability, enabling developers to run the same driver codebase across different embedded platforms with minimal modifications.
