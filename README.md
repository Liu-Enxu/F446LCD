# STM32 Linux-Inspired Handheld System

> **BARE-METAL. EMPOWERED. OPTIMISED.**

A Linux-inspired handheld system built from scratch on an STM32F446RE Nucleo-64 — purpose-built to extract maximum capability from a microcontroller not designed for this workload.

![Project Banner](banner.png)

---

## The Challenge

Running entirely within **128KB SRAM** and **512KB flash** — with a resistive touch panel sharing pins with the parallel display bus, and no FMC or external PSRAM available on the LQFP64 package — every architectural decision is a resource negotiation. Every byte of RAM is accounted for. Every flash sector is a negotiation.

---

## Hardware

| Component | Part | Interface |
|---|---|---|
| MCU | STM32F446RE Nucleo-64 | — |
| Display | ILI9486 3.5" TFT | 8080 Parallel Bus |
| Touch | Resistive (ILI9486 integrated) | Shared 8080 pins |
| Storage | microSD card | SPI |

---

## Current Implementation

- **ILI9486 parallel 8080 TFT display driver** — written from datasheet, no third-party display library
- **Resistive touchscreen** with custom calibration routine and persistent calibration storage
- **LVGL graphics engine** — UI framework handling all rendering, widgets, and font display
- **FatFS SD card filesystem** — full read/write access via SPI
- **Persistent window manager** — header/footer chrome always visible, content area per app
- **Splash screen + boot sequence**
- **Unified FreeRTOS ucHeap** — a single allocator shared across the FreeRTOS kernel, LVGL, and FatFS simultaneously, eliminating fragmentation from competing heap implementations and maximising usable SRAM across all three subsystems

---

## Architecture

```
┌─────────────────────────────────────────────────┐
│  Header: [⚙] [?] [+]  │  [Tab1] [Tab2] ...      │  32px
├─────────────────────────────────────────────────┤
│                                                 │
│              Content Area                       │  258px
│         (active app renders here)               │
│                                                 │
├─────────────────────────────────────────────────┤
│  [RET:NONE] [Pos:x,y] [RAM:%] [SD●][UART○]      │  30px
└─────────────────────────────────────────────────┘

Memory Layout:
  FreeRTOS kernel   ┐
  LVGL              ├── unified ucHeap (128KB SRAM)
  FatFS             ┘
```

---

## Roadmap

| Feature | Status |
|---|---|
| Dynamic tab switching + suspend/resume app lifecycle | 🔲 Next |
| Bash shell simulation | 🔲 Planned |
| btop-inspired system monitor (FreeRTOS + live heap telemetry) | 🔲 Planned |
| Image viewer (BMP/JPEG from SD) | 🔲 Planned |
| Pong | 🔲 Planned |
| UART / I2C / SPI protocol analyzer | 🔲 Planned |
| Runtime script engine — load and execute apps from SD card (no reflashing) | 🔲 Planned |
| micro-ROS2 node (via W25Q NOR external flash) | 🔲 Planned |
| MQTT-driven live data (time, weather) via ESP co-processor | 🔲 Planned |
| External W25Q NOR flash via QSPI (code overflow + asset storage) | 🔲 Planned |
| AT24 EEPROM (persistent config + calibration) | 🔲 Planned |

---

## Toolchain

- **IDE:** VSCode + Keil uVision
- **Library:** Standard Peripheral Library (SPL) — chosen for direct register-level control without HAL abstraction overhead
- **RTOS:** FreeRTOS
- **UI:** LVGL
- **Filesystem:** FatFS

> Note: SPL is ST's legacy peripheral library, predating HAL/LL. It was chosen deliberately here for tighter control over peripheral configuration and lower abstraction cost — a valid trade-off in a resource-constrained, performance-critical project.

---

## Why This Project

Most embedded demos stop at blinking LEDs or basic display output. This project asks a harder question: how far can a single STM32F446RE be pushed when you treat every constraint as a design problem rather than a limitation?

The answer is being built one feature at a time.

---

## Status

🟢 **Active development** — updates posted on [LinkedIn](#) as features ship.
