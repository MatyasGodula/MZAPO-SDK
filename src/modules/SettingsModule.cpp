#include "internal/modules/SettingsModule.hpp"

#include "include/utils/Color.hpp"
#include "include/utils/Theme.hpp"

#include "include/drivers/AudioDriver.hpp"
#include "include/drivers/DisplayDriver.hpp"
#include "include/drivers/SpiledDriver.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>

SettingsModule::SettingsModule(
    DisplayDriver *screen_ptr, 
    AudioDriver *buzzer_ptr,                    
    SpiledDriver *spiled_ptr, 
    Theme *main_theme_ptr,                  
    StateFlag *current_type_ptr
): 
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

void SettingsModule::switch_to(StateFlag new_mod) {
    *current_type = new_mod;
}

void SettingsModule::redraw() {
    int vertical_index = 0;
    screen->fill_screen(main_theme->background);
    screen->draw_rectangle(
        0, 
        setting_selected * selection_height, 
        screen->get_width(),
        selection_height, 
        main_theme->selection
    );

    for (std::string_view theme_name : ThemeNames) {
        screen->draw_text(
            10, 
            vertical_index + 8, 
            main_theme->font, 
            theme_name,
            main_theme->text
        );

        vertical_index += selection_height;
    }
    screen->flush();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void SettingsModule::update() {
    if (spiled->read_knob_press(KnobColor::Red)) {
        switch_to(StateFlag::Menu);
        return;
    }
    if (spiled->read_knob_press(KnobColor::Green)) {
        *main_theme = ThemeList[setting_selected];
        return;
    }
    int selected_copy = setting_selected;
    delta = spiled->read_knob_change(KnobColor::Green);
    aretation += delta;
    if (aretation <= (- selection_aretation)) {
        setting_selected += aretation / selection_aretation;
        aretation = aretation % selection_aretation;
    } else if (aretation >= selection_aretation) {
        setting_selected += aretation / selection_aretation;
        aretation = aretation % selection_aretation;
    }
    if (setting_selected >= static_cast<int>(ThemeCount)) {
        setting_selected = static_cast<int>(ThemeCount) - 1;
    } else if (setting_selected < 0) {
        setting_selected = 0;
    }
    if (selected_copy != setting_selected) {
        buzzer->play_tone(Tone::Selection, 100);
    }
}