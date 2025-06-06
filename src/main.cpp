#include "include/core/Color.hpp"
#include "include/core/Theme.hpp"

#include "include/drivers/AudioDriver.hpp"
#include "include/drivers/DisplayDriver.hpp"
#include "include/drivers/SpiledDriver.hpp"

#include "app_space_invaders/include/app/GameModule.hpp"

#include "include/modules/Module.hpp"

#include "internal/modules/MenuModule.hpp"
#include "internal/modules/SettingsModule.hpp"
#include "internal/modules/TutorialModule.hpp"
#include "internal/modules/GameEndModule.hpp"

#include "third_party/mzapo/mzapo_parlcd.h"
#include "third_party/mzapo/mzapo_phys.h"
#include "third_party/mzapo/mzapo_regs.h"

#include <algorithm>
#include <thread>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <vector>

#define _POSIX_C_SOURCE 200112L

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
    StateFlag current_flag = StateFlag::Menu;
    spiled.init_knobs();

    SettingsModule settings = SettingsModule(&screen, &buzzer, &spiled, &main_theme, &current_flag);
    MenuModule menu = MenuModule(&screen, &buzzer, &spiled, &main_theme, &current_flag);
    GameModule game = GameModule(&screen, &buzzer, &spiled, &main_theme, &current_flag);
    TutorialModule space_invaders_tutorial = TutorialModule(&screen, &buzzer, &spiled, &main_theme, &current_flag);
    GameEndModule game_end_screen = GameEndModule(&screen, &buzzer, &spiled, &main_theme, &current_flag);

    Module *current_module = &menu;

    current_module->switch_setup();

    while (true) {
        StateFlag temp_type = current_flag;
        current_module->update();
        // The type changed inside of the update function so change to a different module
        if (current_flag == StateFlag::Exit) {
            std::cout << "Exiting the main thread\n";
            break;
        }
        if (temp_type != current_flag) {
            switch (current_flag) {
            case StateFlag::Settings:
                current_module = &settings;
                break;
            case StateFlag::Menu:
                current_module = &menu;
                break;
            case StateFlag::Tutorial:
                current_module = &space_invaders_tutorial;
                break;
            case StateFlag::Game:
                current_module = &game;
                break;
            case StateFlag::Win:
                game_end_screen.set_game_end_state(GameEndState::Win);
                current_module = &game_end_screen;
                break;
            case StateFlag::Loss:
                game_end_screen.set_game_end_state(GameEndState::Loss);
                current_module = &game_end_screen;
                break;
            case StateFlag::ResetGame:
                game.reset_game();
                game_end_screen.set_game_end_state(GameEndState::Ongoing);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                current_module = &menu;
                current_flag = StateFlag::Menu;
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
    screen.flush();                   // Flush the screen
    spiled.clear_led_line();          // Clear the LED line

    std::cout << "Ending the main thread\n";

    return 0;
}
