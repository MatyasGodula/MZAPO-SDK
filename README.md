# Space Invaders for MZ-APO (Modular SDK + Game)

A modern C++ game and hardware abstraction SDK for the MZ-APO embedded board.  
Built from scratch as a semestral project at CTU FEE (B0B35APO), this codebase implements a modular state machine architecture, fully custom hardware drivers, and a playable Space Invaders clone.

Designed with scalability in mind, this SDK can be easily extended to support new apps and games.


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
- `app_space_invaders` - **Space Invaders** implementation
- `include/` – Public headers
  - `drivers/` - **Hardware drivers** for the MZ-APO board
  - `modules/` - The **Module system** interface
  - `utils/` - Data types and other utilities
- `internal/` - Internal header files
  - `modules` - Different SDK module implementations, eg. `SettingsModule.hpp` or `MenuModule.hpp`
- `src/` – Source code for drivers and modules
  - `drivers/` - **Hardware driver** implementations
  - `modules/` - **Modules** implementation
- `third_party/` – Provided hardware-related `.c/.h` files
- `assets/` – Fonts
- `docs/` – Architecture diagrams and supporting docs
- `LICENSE` – MIT license
- `Makefile` – Cross-compilation and deployment rules


## Build Instructions

### Prerequisities
An ARM cross-compiler ('arm-linux-gnueabihf-gcc' and 'g++')
 - compiler needs to be compatible with c++23 and c99 features

Need to have a private `mzapo-root-key`, this key can be copied using
```bash
scp cvut_login@postel.felk.cvut.cz:/opt/zynq/ssh-connect/mzapo-root-key ~/.ssh/
chmod 600 ~/.ssh/mzapo-root-key
```

Once the key is copied ot your PC you can then connect to the board using
```bash
ssh -i ~/.ssh/mzapo-root-key -o 'ProxyJump=cvut_login@postel.felk.cvut.cz' root@192.168.223.xxx
```

### Build and Deploy

Run
```bash
make run TARGET_IP=192.168.xxx.xxx
```

To run with debug
```bash
make debug TARGET_IP=192.168.xxx.xxx
```

Automatic connection to the board requires the previously defined private SSH key
```bash
ssh-add /opt/apo/zynq/ssh-connect/mzapo-root-key
```

**If not on the MZ-APO board**
```bash
make
```
**Though without the board itself the code is not runnbale**

## Game Controls

- Green knob: navigate menus / move turret
- Blue knob: shoot / select choice
- Red knob: return
(Input implemented through the SPILED driver)


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
  - Add a custom keyboard and a code editor or an e-reader (DisplayDriver supports newlines!)
  - Multiple games (gameboy-like)
  - A calculator, logic puzzle or even a mini-shell
  - Doom (of course) or Tetris
- For more info about the architecture implementation for SPACE INVADERS see `docs/SPACE_INVADERS_diagram.svg`

**[Open Diagram (SVG)](docs/SPACE_INVADERS_diagram.svg)**

![Architecture Diagram Preview](docs/SPACE_INVADERS_diagram.svg)


## Authors

- Matyas Godula
- Albert Bastl


## License

This project is licensed under the MIT License. See LICENSE for full terms.


## Important Notes

If you are currently enrolled in the APO course at CTU FEE, please consult with your instructors before using or copying any part of this repository. The evaluation system may flag such usage as plagiarism, even if your intent is educational.

I originally developed this project on GitLab, where it was reviewed by my instructor, [Karel Koci](https://github.com/Cynerd). Due to a botched migration, the original GitLab history (including merge approvals) isn't reflected here. Apologies for the incomplete attribution — and thank you for the feedback!
