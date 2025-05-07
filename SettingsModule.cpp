#include "SettingsModule.hpp"
#include "DisplayDriver.hpp"
#include "AudioDriver.hpp"
#include "SpiledDriver.hpp"
#include "Color.hpp"
#include "Theme.hpp"

#include <string_view>
#include <iostream>
#include <stdexcept>

SettingsModule::SettingsModule(
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

SettingsModule::~SettingsModule() {
    std::cout << "Settings module ending!..." << std::endl;
}

void SettingsModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Landscape);
}

void SettingsModule::switch_to(ModuleType new_mod) {
    *current_type = new_mod;
}

void SettingsModule::redraw() {
    int vertical_index = 0;
    screen->fill_screen(main_theme->background);
    screen->draw_rectangle(0, setting_selected * selection_height, screen->get_width(), selection_height, main_theme->selection);
    for (std::string_view theme_name : ThemeNames) {
        screen->draw_text(10, vertical_index + 8, FontType::WinFreeSystem14x16, theme_name, main_theme->text);
        vertical_index += selection_height;
    }
    screen->flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void SettingsModule::update() {
    if (spiled->read_knob_press(KnobColor::Red)) {
        switch_to(ModuleType::Menu);
        return;
    }
    if (spiled->read_knob_press(KnobColor::Green)) {
        *main_theme = ThemeList[setting_selected];
        return;
    }
    int selected_copy = setting_selected;
    int delta = spiled->read_knob_change(KnobColor::Green);
    if (delta > 2) {
        setting_selected++;
    } else if (delta < -2) {
        setting_selected--;
    }
    if (setting_selected < 0) setting_selected = 0;
    if (setting_selected >= ThemeCount) setting_selected = ThemeCount - 1;
    if (selected_copy != setting_selected) {
        buzzer->play_tone(Tone::Selection, 100);
    }

    //std::cout << "Setting selected: " << setting_selected << "\n";
}