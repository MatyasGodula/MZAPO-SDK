#include "GameModule.hpp"

#include "AlienShotSprite.hpp"
#include "AlienSprites.hpp"
#include "AudioDriver.hpp"
#include "BaseSprite.hpp"
#include "Color.hpp"
#include "DisplayDriver.hpp"
#include "Entity.hpp"
#include "ShieldSprite.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"
#include "TurretShotSprite.hpp"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <ostream>
#include <random>
#include <unistd.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480
#define TURRET_POS 0

GameModule::GameModule(
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
        throw std::invalid_argument("Invalid argument passed to GameModule constructor");
    }

    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 50 * 1000;

    int turret_y = SCREEN_HEIGHT - base.height - 5;
    turret_x = (SCREEN_WIDTH - base.width) / 2;

    entities = {
        {turret_x, turret_y, &base},
        {20, 80, &invA},
        {60, 80, &invA},
        {100, 80, &invA},
        {140, 80, &invA},
        {180, 80, &invA},
        {220, 80, &invA},
        {260, 80, &invA},
        {20, 130, &invB},
        {60, 130, &invB},
        {100, 130, &invB},
        {140, 130, &invB},
        {180, 130, &invB},
        {220, 130, &invB},
        {260, 130, &invB},
        {20, 180, &invC, true},
        {60, 180, &invC, true},
        {100, 180, &invC, true},
        {140, 180, &invC, true},
        {180, 180, &invC, true},
        {220, 180, &invC, true},
        {260, 180, &invC, true},
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

void GameModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
}

void GameModule::switch_to(StateFlag new_mod) {
    *current_type = new_mod;
}

void GameModule::update() {
    if (spiled->read_knob_press(KnobColor::Red)) {
        *current_type = StateFlag::Menu;
        std::cout << "Returning to menu module..." << std::endl;
        return;
    }

    turret_x = update_base_position();
    entities[TURRET_POS].pos_x = turret_x;

    if (spiled->read_knob_press(KnobColor::Blue) && turret_shot_cooldown == 0) {
        buzzer->play_tone(Tone::PlayerMissile, 250);
        Entity new_shot;
        new_shot.pos_x = entities[TURRET_POS].pos_x + base.width / 2 - turret_shot.width / 2;
        new_shot.pos_y = entities[TURRET_POS].pos_y;
        new_shot.sprite = &turret_shot;
        shots.push_back(new_shot);
        turret_shot_cooldown = turret_shot_cooldown_time;
    }

    update_shots();
    update_alien_positions();
    aliens_shoot();
    if (turret_shot_cooldown > 0)
        --turret_shot_cooldown;
    if (alien_shot_cooldown > 0)
        --alien_shot_cooldown;

    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, nullptr);
}

void GameModule::redraw() {
    screen->fill_screen(main_theme->background);

    // draw shields
    for (auto &[shield, shield_entity] : shields) {
        for (int y = 0; y < shield->height; ++y) {
            for (int x = 0; x < shield->width; ++x) {
                if (shield->at(x, y)) {
                    screen->draw_pixel(shield_entity.pos_x + x, shield_entity.pos_y + y,
                                       main_theme->shield);
                }
            }
        }
    }

    // draw entities
    for (size_t i = 0; i < entities.size(); ++i) {
        auto &entity = entities[i];
        if (i == TURRET_POS) {
            screen->draw_sprite(entity.pos_x, entity.pos_y, *(entity.sprite), main_theme->turret);
        } else {
            screen->draw_sprite(entity.pos_x, entity.pos_y, *(entity.sprite), main_theme->aliens);
        }
    }

    // draw shots
    for (auto &shot : shots) {
        screen->draw_sprite(shot.pos_x, shot.pos_y, *(shot.sprite), main_theme->selection);
    }

    for (int i = 0; i < turret_lives; ++i) {
        int life_x = 10 + i * 20;
        int life_y = 10;
        screen->draw_rectangle(life_x, life_y, 15, 15, main_theme->turret);
    }

    screen->flush();
}

int GameModule::update_base_position() {
    int new_x = turret_x + spiled->read_knob_change(KnobColor::Green) * 2;
    if (new_x < 0)
        new_x = 0;
    if (new_x > SCREEN_WIDTH - base.width)
        new_x = SCREEN_WIDTH - base.width;
    return new_x;
}

void GameModule::update_shots() {
    for (auto shot = shots.begin(); shot != shots.end();) {
        if (shot->sprite == &alien_shot) {
            shot->pos_y += 7;
        } else {
            shot->pos_y -= 15;
        }

        bool collided = handle_shield_collisions(*shot) || handle_entity_collisions(*shot);
        bool out_of_screen = (shot->pos_y + shot->sprite->height) < 0;

        if (collided || out_of_screen) {
            shot = shots.erase(shot);
        } else {
            ++shot;
        }
    }
}

void GameModule::update_alien_positions() {
    bool edge_reached = false;

    for (size_t i = 1; i < entities.size(); ++i) {
        // skip invisible aliens
        if (entities[i].sprite == &blank_sprite)
            continue;

        int alien_bottom = entities[i].pos_y + entities[i].sprite->height;

        if (alien_bottom >= 375) {
            // victory
            std::cout << "GG" << std::endl;
            return;
        }

        int new_pos = entities[i].pos_x + alien_speed * alien_direction;

        if (new_pos < 0 || (new_pos + entities[i].sprite->width) >= SCREEN_WIDTH) {
            edge_reached = true;
            break;
        }
    }

    if (edge_reached) {
        for (size_t i = 1; i < entities.size(); ++i) {
            if (entities[i].sprite != &blank_sprite) {
                entities[i].pos_y += 5;
            }
        }
        alien_direction *= -1;
    } else {
        for (size_t i = 1; i < entities.size(); ++i) {
            if (entities[i].sprite != &blank_sprite) {
                entities[i].pos_x += alien_speed * alien_direction;
            }
        }
    }
}

void GameModule::aliens_shoot() {
    if (alien_shot_cooldown > 0)
        return;

    int number_shooters = 0;
    std::vector<size_t> shooter_indices;
    for (int i = 1; i < entities.size(); ++i) {
        if (entities[i].is_shooter && entities[i].sprite != &blank_sprite) {
            number_shooters++;
            shooter_indices.push_back(i);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, number_shooters - 1);
    int random_number = distrib(gen);

    Entity &shooter = entities[shooter_indices[random_number]];

    Entity new_shot;
    new_shot.pos_x = shooter.pos_x + shooter.sprite->width / 2 - alien_shot.width / 2;
    new_shot.pos_y = shooter.pos_y + shooter.sprite->height;
    new_shot.sprite = &alien_shot;
    shots.push_back(new_shot);
    buzzer->play_tone(Tone::EnemyMissile, 250);

    alien_shot_cooldown = alien_shot_cooldown_time;
}

bool GameModule::handle_shield_collisions(Entity &shot) {
    for (auto &[shield, shield_entity] : shields) {
        int relative_x = shot.pos_x - shield_entity.pos_x;
        int relative_y = shot.pos_y - shield_entity.pos_y;

        // shot inside shield
        if (relative_x < 0 || relative_x >= shield->width || relative_y < 0 ||
            relative_y >= shield->height)
            continue;

        // go through pixels
        for (int sy = 0; sy < shot.sprite->height; ++sy) {
            for (int sx = 0; sx < shot.sprite->width; ++sx) {
                int check_x = relative_x + sx;
                int check_y = relative_y + sy;
                if (check_x >= 0 && check_x < shield->width && check_y >= 0 &&
                    check_y < shield->height && shield->at(check_x, check_y) != 0) {
                    shield->damage_area(check_x, check_y, 10);
                    return true;
                }
            }
        }
    }
    return false;
}

bool GameModule::handle_entity_collisions(Entity &shot) {
    for (size_t i = 0; i < entities.size(); ++i) {
        Entity &target = entities[i];

        // skip invisible
        if (target.sprite == &blank_sprite)
            continue;

        bool collide_x = shot.pos_x + shot.sprite->width > target.pos_x &&
                         shot.pos_x < target.pos_x + target.sprite->width;
        bool collide_y = shot.pos_y + shot.sprite->height > target.pos_y &&
                         shot.pos_y < target.pos_y + target.sprite->height;

        if (collide_x && collide_y) {
            // turret hit
            if (i == TURRET_POS && shot.sprite == &alien_shot) {
                turret_lives--;
                if (turret_lives <= 0) {
                    // defeat
                }
                return true;
            }

            // alien hit
            if (i != TURRET_POS && shot.sprite == &turret_shot) {
                // choose new shooter
                if (i >= 8 && entities[i - 7].sprite != &blank_sprite) {
                    entities[i - 7].is_shooter = true;
                }
                // dead alien
                entities[i].sprite = &blank_sprite;
                destroyed_aliens_nbr++;
                alien_speed = 1 + (destroyed_aliens_nbr / 5);
                return true;
            }
        }
    }
    return false;
}
