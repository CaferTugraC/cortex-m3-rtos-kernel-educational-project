# Cortex-M3 RTOS Project
This project is a lightweight, layered Real-Time Operating System (RTOS) kernel developed specifically for the Cortex-M3 architecture.

## Overview
The RTOS utilizes a Round-Robin scheduling algorithm to manage tasks in ready and blocked states. The primary goal of this project is to build a modular design that can be easily adapted to various microcontroller platforms while deepening practical knowledge of the Cortex-M3 architecture.

## Features
- **Round-Robin Scheduler:** Ensures fair time distribution between tasks.

- **Task Management:** Core management supporting Ready and Blocked states.

- **Modular Design:** A layered architecture that minimizes hardware dependencies.

- **High Portability:** Structured for easy porting to different Cortex-M3-based microcontrollers.

- **Clean & Readable Code:** Emphasis on code clarity for educational and learning purposes.

# Architecture
The project is built upon the following layered architecture:

- **Application Layer**: Contains user applications and task definitions. (app/)

- **Kernel Layer:** The core of the RTOS, including the scheduler and task management. (middleware/)

- **Port Layer:** Contains architecture-specific code for the target processor (Cortex-M3). (middleware/portable/)

- **BSP (Board Support Package):** Includes startup code and hardware drivers specific to the target board (e.g., STM32F103C8T6). (bsp/)

- **Drivers Layer:** Contains drivers for peripherals such as LEDs. (drivers/)

```mermaid
graph TD
    A[Application (main.c)] --> B{RTOS API (scheduler.h)};
    B --> C[RTOS Kernel (scheduler.c)];
    C --> D{Port Layer (port.h)};
    D --> E[Arch-Specific Code (port.c, port_asm.s)];
    C --> F{Drivers (led.h)};
    F --> G[Hardware Drivers (led.c)];
    E --> H[Hardware (Cortex-M3)];
    G --> H;
```

## 🛠️ Setup and Compilation
Follow these steps to compile the project:

1. Prerequisites
    - `arm-none-eabi-gcc (C compiler for ARM)`

    - `make`

2. Compilation
Open your terminal in the project root directory and run:

    ```Bash
    make
    ```
This command will compile the project and generate the output files in the `build` directory.

## 💻 Supported Hardware
STMicroelectronics STM32F103C8T6 ("Blue Pill")

## 🤝 Contributing
While this project was created for personal development, contributions are welcome. Feel free to open an issue or submit a pull request.

## 📄 License
This project is licensed under the MIT License.