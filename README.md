# Space Invaders for MZ-APO (Modular SDK + A game)

A modular C++ game framework for the MZ-APO board, developed as a semestral project at CTU FEE for the Computer Architectures class (B0B35APO)
This project implements a Space Invaders clone and a modular driver-based SDK suitable for bulding other games or applications for the MZ-APO board.


## Features

- Modular architecture with trait-based `Module` interface
- Custom heapless hardware drivers:
    - `DisplayDriver` with screen rotation and double-buffering
    - `AudioDriver` with non-blocking jthread tone control
    - `SpiledDriver` for RGB knob and LED bar control
- Menu, Settings, Tutorial, and Game states
- Theme system with predefined famous themes
- Font rendering and sprite system
- Screen scrolling and dynamic orientation switching


## Project Structure

include/ # Public headers
src/ # Source code for drivers and modules
third_party/ # Provided code
assets/ # Fonts
docs/ # Documentation
LICENSE # MIT project license
Makefile # Compilation


## Build Instructions

### Prerequsities
- ARM cross-compiler ('arm-linux-gnueabihf-gcc' and 'g++')
    - compiler needs to be compatible with c++23 and c99 features

### Build and Deploy
```bash
make run TARGET_IP=192.168.xxx.xxx
```

To run with debug
```bash
make debug TARGET_IP=192.168.xxx.xxx
```


## Game Controls

- Green knob: navigate menus / move turret
- Blue knob: shoot / select choice
- Red knob: return
(input implemented through the SPILED driver)


## Architectural Overview

This project was designed as a reusable SDK for apps on the MZ-APO board
- A shared `Module` trait interface
- Hardware interaction is encapsulated in reusable drivers
- Every module implements 
    - `redraw()` to draw data on the screen
    - `update()` to take inputs and update the state of the current Module
    - `switch_setup()` to set up all important module data eg. change the display orientation
    - `switch_to(StateFlag)` which changes the module to another one or signals info to the main loop
- You can extend this project with any Module/s, for example:
    - Add a custom keybord and a code editor or an e-reader (DisplayDriver supports newlines!)
    - Multiple games (gameboy-like)
    - A calculator, logic puzzle or even a mini-shell
    - Doom (of course) or Tetris
- For more info about the architecture implementation for SPACE INVADERS see `docs/SPACE_INVADERS_diagram.svg`


## Authors

- Matyas Godula
- Albert Bastl


## License

This project is licensed under the MIT License. See LICENSE for full terms.