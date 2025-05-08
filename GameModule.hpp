#pragma once

#include "AlienShotSprite.hpp"
#include "AlienSprites.hpp"
#include "AudioDriver.hpp"
#include "BaseSprite.hpp"
#include "BlankSprite.hpp"
#include "DisplayDriver.hpp"
#include "Entity.hpp"
#include "Module.hpp"
#include "ShieldSprite.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"
#include "TurretShotSprite.hpp"

#include <ctime>
#include <utility>
#include <vector>

class GameModule : public Module {
    public:
        GameModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr,
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        ~GameModule() override;

        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

        // TODO: Please implement
        void reset_game();

    private:
        int update_base_position();
        void update_shots();
        void update_alien_positions();
        void aliens_shoot();

        bool handle_shield_collisions(Entity &shot);
        bool handle_entity_collisions(Entity &shot);

        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *const main_theme;
        StateFlag *const current_type;

        BaseSprite base;
        InvaderA invA;
        InvaderB invB;
        InvaderC invC;
        AlienShotSprite alien_shot;
        TurretShotSprite turret_shot;
        ShieldSprite shield_1, shield_2, shield_3, shield_4;
        BlankSprite blank_sprite;

        unsigned char *lcd_mem_base = nullptr;
        unsigned char *spi_mem_base = nullptr;

        std::vector<Entity> entities;
        std::vector<Entity> shots;
        std::vector<std::pair<ShieldSprite *, Entity>> shields;

        int turret_x = 0;
        uint8_t prev_knob = 0;
        int turret_shot_cooldown = 0;
        const int turret_shot_cooldown_time = 10;
        int alien_shot_cooldown = 0;
        const int alien_shot_cooldown_time = 50;

        int destroyed_aliens_nbr = 0;
        int alien_direction = 1;
        int alien_speed = 1;

        int turret_lives = 4;

        struct timespec loop_delay;
};
