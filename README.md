# 🛡️ PinCracker

Welcome to **PinCracker**∠( ᐛ 」∠)＿, a low-level hardware-software integration project designed for the Raspberry Pi 4! This project demonstrates the power of combining high-level C logic with high-performance ARM Assembly to create a robust PIN-cracking and hardware-interfacing system.

---

## Developing process
[Develop record](https://foremost-garage-64a.notion.site/PinCracker-3333024d4443801b9008ff40d39973c9)

## 📖 Project Overview

PinCracker is a systems programming coursework project (F28HS-CW2) that bridges the gap between software and hardware. It interfaces with physical components—LEDs, buttons, and a 1602 LCD display—to simulate a secure PIN entry and cracking environment. Whether you're interested in memory-mapped I/O, ARM assembly optimization, or real-time signal handling, PinCracker has it all! ( ´▽｀)

---

## 📂 Core Files & Project Structure

The project is structured into modular components, combining high-level application logic with low-level hardware drivers and performance-critical assembly code. This section is aim to help you understand the project's organization and how the components interact.(^ー゜)

### 🏗️ Application Logic
- **[cw2.c](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/cw2.c)**: The main entry point of the application. It orchestrates the PIN cracking process, handles user input via buttons.
- **[cw2-aux.c](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/cw2-aux.c)**: Contains auxiliary utility functions for high-precision timing, delays, and standardized error handling.

### 📺 Hardware Drivers (LCD & GPIO)
- **[lcd-binary.c](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/lcd-binary.c)**: The low-level foundation for GPIO interaction. It implements core pin manipulation (read/write/mode) using **inline ARM assembly** for maximum performance and direct register access.
- **[lcd-fcts.c](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/lcd-fcts.c)**: A high-level driver for the 1602 LCD display. It implements the HD44780U 4-bit protocol, providing an easy-to-use API for string output, clearing the screen, and initialization.

### 🧮 Algorithmic Core
- **[hamming.s](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/hamming.s)**: A pure **ARM assembly** implementation of the Hamming distance algorithm. This file handles the performance-critical task of comparing PIN codes to identify the best match during the cracking process.

### ⚙️ Configuration & Hardware Mapping
- **[cw2-config.h](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/cw2-config.h)**: Centralized hardware configuration, defining the physical GPIO pin mapping for LEDs, buttons, and the LCD control lines.
- **[gpio.h](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/gpio.h)**: Low-level definitions for the Raspberry Pi 4's BCM2711 GPIO registers, including base addresses and functional offsets.

### 🛠️ Build System
- **[Makefile](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/Makefile)**: Automates the compilation process, correctly linking the C source files with the ARM assembly modules to produce the final `cw2` executable.

- Pay attention that we use Conditional Compilation to handle different situations. If you want to use a **c hamming function** please change the Makefile. Also we now using **a much more fast search logic**, if you want to see the original logic please change the Makefile.

- In sepcific, delete -DHAMM_ASN to use the c hamming function. Add #TASK4 to use the full search logic.
---

## 🛠️ Tech Stack

- **Languages**: 
  - 🖥️ **C**: System logic, hardware initialization, and state management.
  - 🚂 **ARM Assembly**: Performance-critical algorithms and inline register manipulation.
- **Linux APIs**: Memory mapping (`mmap`), Signal Handling (`sigaction`, `setitimer`).
- **Hardware**: Raspberry Pi 4 (BCM2711), 1602 LCD, GPIO components.
- **Build System**: `GNU Make`.

---

## 🚀 Getting Started

Follow these steps to get PinCracker up and running on your Raspberry Pi:

### 📥 Installation

1. Clone the repository to your Raspberry Pi:
   ```bash
   git clone https://github.com/your-repo/pincracker.git
   cd pincracker/f28hs-2025-26-cw2-sys
   ```

2. Compile the project using the provided Makefile:
   ```bash
   make
   ```

### 🎮 Running the Application

Since the application requires direct access to `/dev/mem`, it must be run with `sudo` privileges.

```bash
sudo ./cw2 [OPTIONS]
```

#### Command Line Options (´･Д･)」

PinCracker provides various command line options to configure its behavior and test functions:

-   **`-h`**: Display help information and exit.
-   **`-v`**: Enable verbose output mode.
-   **`-d`**: Enable debug mode.
-   **`-e`**: Enable "explore" mode. In this mode, the program will continue to try all possible combinations even after the PIN is found.
-   **`-l`**: Run LCD test only.
-   **`-u`**: Enable unit test mode. Requires use with the `-s` and `-r` options.
-   **`-S <delay_ms>`**: Set the delay (in milliseconds) when submitting a PIN. The default value is defined in `cw2-config.h`.
-   **`-s <secret_seq>`**: Set the secret PIN sequence. For example, `-s 123` sets the secret PIN to 1-2-3.
-   **`-r <ref_seq>`**: In unit test mode (`-u`), set the reference sequence for comparison.
-   **`-m <max_val>`**: Set the maximum possible value for each digit (e.g., if set to 9, the digit range is 1-9). The default value is defined in `cw2-config.h`.
-   **`-n <seq_len>`**: Set the length of the PIN sequence. The default value is defined in `cw2-config.h`.

#### Running Examples

-   **Basic Run**:
    ```bash
    sudo ./cw2
    ```
-   **Set a custom secret PIN and enable debug mode**:
    ```bash
    sudo ./cw2 -d -s 112
    ```
-   **Run a unit test, comparing secret sequence 112 with reference sequence 121**:
    ```bash
    sudo ./cw2 -u -s 112 -r 121
    ```
-   **Set sequence length to 4 and maximum value per digit to 5**:
    ```bash
    sudo ./cw2 -n 4 -m 5
    ```
-   **Run LCD test only**:
    ```bash
    sudo ./cw2 -l
    ```

---

## ⚙️ Configuration

The project is highly configurable. You can modify hardware pin assignments and system constants in the configuration header:

- 📝 **[cw2-config.h](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/cw2-config.h)**: Change GPIO pins for LEDs, buttons, and LCD connections.
- ⚙️ **[gpio.h](file:///Users/traveler/OUC/code/F28HS-CW2/f28hs-2025-26-cw2-sys/gpio.h)**: Low-level GPIO register offsets and base addresses.

---


## 📜 License

Distributed under the Heriot-Watt License. 

---

*Happy Cracking!* (・ω・)ノ
