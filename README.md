# Tower Defense

A 2D Tower Defense simulation written in C using the SDL2 library. 

<img width="300" alt="image" src="https://github.com/user-attachments/assets/93898d21-a36d-4e5d-a03a-ec1c2ae01a7e" />


## Overview

This project is an automated simulation where defending towers protect a King against waves of enemies along a random generated path. The core mechanics are built from scratch without external game engines, focusing on standard C data structures and memory management.

## Features

* **Custom Data Structures:** Entities (towers, enemies) are dynamically managed via singly linked lists.
* **Algorithmic Logic:** Includes procedural path generation and automated tower placement based on range and grid coverage.
* **Game State Persistence:** Support for saving and loading matches using both binary (`.tdb`) and text/sequential (`.tds`) file formats.

## Build & Run

**Dependencies:** SDL2

**Linux:**
Install the SDL2 development libraries via your package manager:

```bash
sudo apt install libsdl2-dev   # Debian / Ubuntu
sudo pacman -S sdl2            # Arch Linux
```

*(Note: A Makefile can be added, otherwise compile via your standard C toolchain, e.g., `gcc *.c -o game -I/usr/include/SDL2 -lSDL2`)*

Windows:
Open the provided projetTowerDefend.cbp workspace with Code::Blocks. Ensure your compiler and linker paths point to your local SDL2 installation.

Developed by Clarent HEMERY FAY
