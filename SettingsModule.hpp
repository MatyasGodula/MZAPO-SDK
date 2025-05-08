#pragma once

#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"

constexpr int selection_height = 32;

class SettingsModule : public Module {
    public:
        SettingsModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr,
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        SettingsModule(const SettingsModule &) = delete;
        SettingsModule &operator=(const SettingsModule &) = delete;

        ~SettingsModule() override;

        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

    private:
        int setting_selected = 0;
        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *main_theme;
        StateFlag *current_type;
};