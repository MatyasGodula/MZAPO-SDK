#pragma once

#include "BlankSprite.hpp"
#include "Module.hpp"
#include "DisplayDriver.hpp"
#include "SpiledDriver.hpp"
#include "AudioDriver.hpp"
#include "Theme.hpp"
#include "ShieldSprite.hpp"
#include "ShotSprite.hpp"
#include "BaseSprite.hpp"
#include "AlienSprites.hpp"
#include "Entity.hpp"

#include <vector>
#include <utility>
#include <ctime>

class GameModule : public Module {
public:
    GameModule(
        DisplayDriver* screen_ptr,
        AudioDriver* buzzer_ptr,
        SpiledDriver* spiled_ptr,
        Theme* main_theme_ptr,
        ModuleType* current_type_ptr
    );

    ~GameModule() override;

    void update() override;
    void redraw() override;
    void switch_setup() override;
    void switch_to(ModuleType new_mod) override;

private:
    int update_base_position(uint8_t knob_val);
    void update_shots();

    DisplayDriver* const screen;
    AudioDriver* const buzzer;
    SpiledDriver* const spiled;
    Theme* const main_theme;
    ModuleType* const current_type;

    BaseSprite base;
    InvaderA invA;
    InvaderB invB;
    InvaderC invC;
    ShotSprite shotSprite;
    ShieldSprite shield_1, shield_2, shield_3, shield_4;
    BlankSprite blank_sprite;

    unsigned char* lcd_mem_base = nullptr;
    unsigned char* spi_mem_base = nullptr;

    std::vector<Entity> entities;
    std::vector<Entity> shots;
    std::vector<std::pair<ShieldSprite*, Entity>> shields;

    int turret_x = 0;
    uint8_t prev_knob = 0;
    int shot_cooldown = 0;
    const int shot_cooldown_time = 10;

    struct timespec loop_delay;
};
