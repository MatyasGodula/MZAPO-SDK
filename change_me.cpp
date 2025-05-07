#include "DisplayDriver.hpp"
#include "Color.hpp"
#include "SpiledDriver.hpp"
#include "AudioDriver.hpp"
#include "Theme.hpp"
#include "Module.hpp"
#include "SettingsModule.hpp"
#include "MenuModule.hpp"
#include "GameModule.hpp"
#include "TutorialModule.hpp"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <vector>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <algorithm>

#define _POSIX_C_SOURCE 200112L

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480

#define TURRET_POS 0

int main() {
    DisplayDriver screen(DisplayOrientation::Portrait);
    screen.fill_screen(Color::Black);
    void *spiled_mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (!spiled_mem_base) {
        throw std::runtime_error("Failed to map SPILED memory base!");
    }    
    SpiledDriver spiled = SpiledDriver(spiled_mem_base);
    AudioDriver buzzer = AudioDriver();
    Theme main_theme = DefaultTheme;
    ModuleType current_type = ModuleType::Menu;
    spiled.init_knobs();

    SettingsModule settings = SettingsModule(&screen, &buzzer, &spiled, &main_theme, &current_type);
    MenuModule menu = MenuModule(&screen, &buzzer, &spiled, &main_theme, &current_type);
    GameModule game = GameModule(&screen, &buzzer, &spiled, &main_theme, &current_type);
    TutorialModule space_invaders_tutorial = TutorialModule(&screen, &buzzer, &spiled, &main_theme, &current_type);

    Module *current_module = &menu;

    current_module->switch_setup();

    while (true) {
        ModuleType temp_type = current_type;
        current_module->update();
        // The type changed inside of the update function so change to a different module
        if (current_type == ModuleType::Exit) {
            std::cout << "Exiting the main thread\n";
            break;
        }
        if (temp_type != current_type) {
            switch (current_type) {
                case ModuleType::Settings:
                    current_module = &settings;
                    break;
                case ModuleType::Menu:
                    current_module = &menu;
                    break;
                case ModuleType::Tutorial:
                    current_module = &space_invaders_tutorial;
                    break;
                case ModuleType::Game:
                    current_module = &game;
                    break;
                default:
                    std::cout << "Invalid module type\n";
                    throw std::runtime_error("Invalid module type");
            }
            current_module->switch_setup();
        }
        current_module->redraw();
    }

    screen.fill_screen(Color::Black); // "Turn off" the screen 
    screen.flush(); // Flush the screen
    spiled.clear_led_line(); // Clear the LED line

    std::cout << "Ending the main thread\n";

    return 0;
}
