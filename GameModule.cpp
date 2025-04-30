#include "GameModule.hpp"

#include "AlienSprites.hpp"
#include "DisplayDriver.hpp"
#include "Entity.hpp"
#include "Color.hpp"
#include "ShieldSprite.hpp"
#include "ShotSprite.hpp"
#include "SpiledDriver.hpp"
#include "AudioDriver.hpp"
#include "Theme.hpp"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <ostream>
#include <unistd.h>
#include <ctime>
#include <algorithm>
#include <iostream>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480
#define TURRET_POS 0

GameModule::GameModule(
    DisplayDriver* screen_ptr,
    AudioDriver* buzzer_ptr,
    SpiledDriver* spiled_ptr,
    Theme* main_theme_ptr,
    ModuleType* current_type_ptr
) :
    screen(screen_ptr),
    buzzer(buzzer_ptr),
    spiled(spiled_ptr),
    main_theme(main_theme_ptr),
    current_type(current_type_ptr)
{
    if (!screen || !buzzer || !spiled || !main_theme || !current_type) {
        throw std::invalid_argument("Invalid argument passed to GameModule constructor");
    }

    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 50 * 1000;

    int baseY = SCREEN_HEIGHT - base.height - 5;
    turret_x = (SCREEN_WIDTH - base.width) / 2;

    entities = {
        {turret_x, baseY, &base},
        {20, 30, &invA}, {60, 30, &invA}, {100, 30, &invA}, {140, 30, &invA}, {180, 30, &invA}, {220, 30, &invA}, {260, 30, &invA},
        {20, 80, &invB}, {60, 80, &invB}, {100, 80, &invB}, {140, 80, &invB}, {180, 80, &invB}, {220, 80, &invB}, {260, 80, &invB},
        {20, 130, &invC, true}, {60, 130, &invC, true}, {100, 130, &invC, true}, {140, 130, &invC, true}, {180, 130, &invC, true}, {220, 130, &invC, true}, {260, 130, &invC, true},
    };

    shields = {
        {&shield_1, {20, 375, &shield_1}},
        {&shield_2, {100, 375, &shield_2}},
        {&shield_3, {180, 375, &shield_3}},
        {&shield_4, {260, 375, &shield_4}},
    };
}

GameModule::~GameModule() {
    std::cout << "Game module ending!..." << std::endl;
}

void GameModule::update() {
    
    if (spiled->read_knob_press(KnobColor::Red)) {
        *current_type = ModuleType::Menu;
        std::cout << "Returning to menu module..." << std::endl;
        return;
    }

    uint8_t knob_val = spiled->read_knob_val(KnobColor::Green);
    turret_x = update_base_position(knob_val);
    entities[TURRET_POS].pos_x = turret_x;

    if (spiled->read_knob_press(KnobColor::Blue) && shot_cooldown == 0) {
        Entity new_shot;
        new_shot.pos_x = entities[TURRET_POS].pos_x + base.width / 2 - shotSprite.width / 2;
        new_shot.pos_y = entities[TURRET_POS].pos_y;
        new_shot.sprite = &shotSprite;
        shots.push_back(new_shot);
        shot_cooldown = shot_cooldown_time;
    }

    update_shots();
    if (shot_cooldown > 0) --shot_cooldown;

    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, nullptr);
}

void GameModule::redraw() {
    screen->fill_screen(main_theme->background);
    for (auto& [shield, shield_entity] : shields) {
        for (int y = 0; y < shield->height; ++y) {
            for (int x = 0; x < shield->width; ++x) {
                if (shield->at(x, y)) {
                    screen->draw_pixel(shield_entity.pos_x + x, shield_entity.pos_y + y, main_theme->shield);
                }
            }
        }
    }

    for (auto& entity : entities) {
        if (entity.is_shooter) {
            screen->draw_sprite(entity.pos_x, entity.pos_y, *(entity.sprite), Color::Red);
        } else {
            screen->draw_sprite(entity.pos_x, entity.pos_y, *(entity.sprite), main_theme->turret);
        }
    }

    for (auto& shot : shots) {
        screen->draw_sprite(shot.pos_x, shot.pos_y, *(shot.sprite), main_theme->selection);
    }

    screen->flush();
}

void GameModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
}

void GameModule::switch_to(ModuleType new_mod) {
    *current_type = new_mod;
}

int GameModule::update_base_position(uint8_t knob_val) {
    prev_knob = knob_val;
    int new_x = turret_x + spiled->read_knob_change(KnobColor::Green) * 2;
    if (new_x < 0) new_x = 0;
    if (new_x > SCREEN_WIDTH - base.width) new_x = SCREEN_WIDTH - base.width;
    return new_x;
}

void GameModule::update_shots() {
    for (auto shot = shots.begin(); shot != shots.end();) {
        shot->pos_y -= 15;
        bool destroyed = false;

        for (auto& [shield, shield_entity] : shields) {
            int local_x = shot->pos_x - shield_entity.pos_x;
            int local_y = shot->pos_y - shield_entity.pos_y;
            bool collision = false;
            for (int sy = 0; sy < shot->sprite->height; ++sy) {
                for (int sx = 0; sx < shot->sprite->width; ++sx) {
                    int check_x = local_x + sx;
                    int check_y = local_y + sy;
                    if (check_x >= 0 && check_x < shield->width &&
                        check_y >= 0 && check_y < shield->height &&
                        shield->at(check_x, check_y) != 0) {
                        collision = true;
                        break;
                    }
                }
                if (collision) break;
            }
            if (collision) {
                shield->damage_area(local_x, local_y, 10);
                destroyed = true;
                break;
            }
        }

        if (!destroyed) {
            for (size_t i = 1; i < entities.size(); ++i) {
                Entity& target = entities[i];

                if (target.sprite == &blank_sprite) continue;

                bool collide_x = shot->pos_x + shot->sprite->width > target.pos_x &&
                                shot->pos_x < target.pos_x + target.sprite->width;
                bool collide_y = shot->pos_y + shot->sprite->height > target.pos_y &&
                                shot->pos_y < target.pos_y + target.sprite->height;

                if (collide_x && collide_y) {
                    if (i >= 8 && entities[i - 7].sprite != nullptr) {
                        entities[i - 7].is_shooter = true;
                    }
                    entities[i].sprite = &blank_sprite;
                    destroyed = true;
                    break;
                }
            }
        }


        if (destroyed || shot->pos_y + shot->sprite->height < 0) {
            shot = shots.erase(shot);
        } else {
            ++shot;
        }
    }
}
