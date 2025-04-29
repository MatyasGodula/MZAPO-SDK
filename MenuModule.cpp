#include "MenuModule.hpp"

#include "Theme.hpp"
#include "DisplayDriver.hpp"
#include "AudioDriver.hpp"
#include "SpiledDriver.hpp"
#include "Module.hpp"

#include <string_view>
#include <iostream>
#include <stdexcept>


MenuModule::MenuModule(
    DisplayDriver *screen_ptr, 
    AudioDriver *buzzer_ptr, 
    SpiledDriver *spiled_ptr, 
    Theme *main_theme_ptr, 
    ModuleType *current_type_ptr
) 
:
    screen(screen_ptr),
    buzzer(buzzer_ptr),
    spiled(spiled_ptr),
    main_theme(main_theme_ptr),
    current_type(current_type_ptr)
{
    if (!screen || !buzzer || !spiled || !main_theme || !current_type) {
        throw std::invalid_argument("Invalid argument passed to SettingsModule constructor");
    }
}

MenuModule::~MenuModule() {
    std::cout << "Menu module ending!..." << std::endl;
}

void MenuModule::update() {
    if (spiled->read_knob_press(KnobColor::Green)) {
        *current_type = MenuModuleTypes::SelectionTypes[setting_selected];
        if (*current_type == ModuleType::Settings) {
            std::cout << "Changing to settings module\n";
        } else if (*current_type == ModuleType::Menu) {
            std::cout << "Changing to menu module\n";
        }
        std::cout << "Changing module\n";
        return;
    }
    int delta = spiled->read_knob_change(KnobColor::Green);
    if (delta > 2) {
        setting_selected++;
    } else if (delta < -2) {
        setting_selected--;
    }
    if (setting_selected < 0) setting_selected = 0;
    if (setting_selected >= MenuModuleTypes::SelectionCount) setting_selected = MenuModuleTypes::SelectionCount - 1;
}

void MenuModule::redraw() {
    screen->fill_screen(main_theme->background);
    int vertical_index = MenuConstants::selection_pos_y;
    screen->draw_text(
        MenuConstants::selection_pos_x + 10, 
        vertical_index - MenuConstants::selection_height + 8, 
        FontType::WinFreeSystem14x16, 
        "SPACE INVADERS", 
        main_theme->text
    );

    screen->draw_rectangle(
        MenuConstants::selection_pos_x, 
        MenuConstants::selection_pos_y + setting_selected * MenuConstants::selection_height, 
        MenuConstants::selection_width, 
        MenuConstants::selection_height, 
        main_theme->selection
    );

    for (std::string_view selection_name : MenuModuleTypes::SelectionNames) {
        screen->draw_text(
            MenuConstants::selection_pos_x + 10, 
            vertical_index + 8, 
            FontType::WinFreeSystem14x16, 
            selection_name, 
            main_theme->text
        );
        vertical_index += MenuConstants::selection_height;
    }

    screen->flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void MenuModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Landscape);
}

void MenuModule::switch_to(ModuleType new_mod) {
    *current_type = new_mod;
}
