#include "Module.hpp"
#include "DisplayDriver.hpp"
#include "SpiledDriver.hpp"
#include "AudioDriver.hpp"
#include "Theme.hpp"

namespace MenuConstants {
    constexpr int selection_height = 32;
    constexpr int selection_width = 100;
    constexpr int selection_pos_x = 50;
    constexpr int selection_pos_y = 82; // change later
}

namespace MenuModuleTypes {
    constexpr ModuleType SelectionTypes[] = {
        ModuleType::Game,
        ModuleType::Settings,
        ModuleType::Exit
    };
    constexpr std::string_view SelectionNames[] = {
        "Play",
        "Settings",
        "Exit"
    };
    constexpr std::size_t SelectionCount = sizeof(MenuModuleTypes::SelectionTypes) / sizeof(MenuModuleTypes::SelectionTypes[0]);
}


class MenuModule : public Module {
    public:
        MenuModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr, 
            Theme *main_theme_ptr, 
            ModuleType *current_type_ptr
        );

        MenuModule(const MenuModule&) = delete;
        MenuModule& operator=(const MenuModule&) = delete;

        ~MenuModule() override;

       
        void update() override;

        void redraw() override;

        void switch_setup() override;

        void switch_to(ModuleType new_mod) override;

    private:
        int setting_selected = 0;
        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *main_theme;
        ModuleType *current_type;
};