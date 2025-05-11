#include "include/GameModule.hpp"

#include "third_party/mzapo/mzapo_parlcd.h"  // parlcd hardware
#include "third_party/mzapo/mzapo_phys.h"    // physical memory mappings
#include "third_party/mzapo/mzapo_regs.h"    // register definitions

#include "include/DisplayDriver.hpp"
#include "include/AudioDriver.hpp"
#include "include/SpiledDriver.hpp"
#include "include/Theme.hpp"
#include "include/AlienSprites.hpp"
#include "include/TurretShotSprite.hpp"
#include "include/AlienShotSprite.hpp"
#include "include/ShieldSprite.hpp"
#include "include/BaseSprite.hpp"
#include "include/Entity.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <iostream>
#include <ctime>
#include <unistd.h>

#define SCREEN_WIDTH 320    // display width
#define SCREEN_HEIGHT 480   // display height
#define TURRET_POS 0        // index of turret in entities

//---- Constructor & Destructor ---------------------------------------------
GameModule::GameModule(
    DisplayDriver *screen_ptr,
    AudioDriver *buzzer_ptr,
    SpiledDriver *spiled_ptr,
    Theme *main_theme_ptr,
    StateFlag *current_type_ptr
) :
    screen(screen_ptr),
    buzzer(buzzer_ptr),
    spiled(spiled_ptr),
    main_theme(main_theme_ptr),
    current_type(current_type_ptr)
{
    if (!screen || !buzzer || !spiled || !main_theme || !current_type) {
        throw std::invalid_argument("Null argument passed to GameModule constructor");
    }

    // timing for game loop
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 50 * 1000;

    // initial turret position
    int turret_y = SCREEN_HEIGHT - base.height - 5;
    turret_x = (SCREEN_WIDTH - base.width) / 2;

    // spawn player and aliens
    entities = {
        {turret_x, turret_y, &base},
        {20,  80, &inv_a}, {60,  80, &inv_a}, {100, 80, &inv_a}, {140, 80, &inv_a},
        {180, 80, &inv_a}, {220, 80, &inv_a}, {260, 80, &inv_a},
        {20, 130, &inv_b}, {60, 130, &inv_b}, {100,130, &inv_b}, {140,130, &inv_b},
        {180,130, &inv_b}, {220,130, &inv_b}, {260,130, &inv_b},
        {20, 180, &inv_c, true}, {60,  180, &inv_c, true},
        {100,180, &inv_c, true}, {140,180, &inv_c, true},
        {180,180, &inv_c, true}, {220,180, &inv_c, true}, {260,180, &inv_c, true}
    };

    // place shields
    shields = {
        {&shield_1, {20, 375, &shield_1}},
        {&shield_2, {100,375, &shield_2}},
        {&shield_3, {180,375, &shield_3}},
        {&shield_4, {260,375, &shield_4}}
    };
}

GameModule::~GameModule() {
    std::cout << "GameModule destroyed" << std::endl;
}

//---- Module Switching -----------------------------------------------------
void GameModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
    spiled->init_led_line(4);  // prepare LED display
}

void GameModule::switch_to(StateFlag new_mod) {
    *current_type = new_mod;    // change game state
}

//---- Main Loop -------------------------------------------------------------
void GameModule::update() {
    // return to menu on Red knob press
    if (spiled->read_knob_press(KnobColor::Red)) {
        *current_type = StateFlag::Menu;
        std::cout << "Switching to Menu" << std::endl;
        return;
    }

    // move turret
    turret_x = update_base_position();
    entities[TURRET_POS].pos_x = turret_x;

    // fire turret shot
    if (spiled->read_knob_press(KnobColor::Blue) && turret_shot_cooldown == 0) {
        buzzer->play_tone(Tone::PlayerMissile, 250);
        Entity shot{ turret_x + base.width/2 - turret_shot.width/2,
                     entities[TURRET_POS].pos_y,
                     &turret_shot };
        shots.push_back(shot);
        turret_shot_cooldown = turret_shot_cooldown_time;
    }

    update_shots();           // move shots and handle collisions
    update_alien_positions(); // move aliens
    aliens_shoot();           // aliens fire randomly

    // cooldown timers
    if (turret_shot_cooldown) --turret_shot_cooldown;
    if (alien_shot_cooldown)  --alien_shot_cooldown;

    // update lives LEDs
    spiled->clear_led_line();
    spiled->set_led_line(static_cast<uint8_t>(turret_lives));

    // regulate frame rate
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, nullptr);
}

void GameModule::redraw() {
    screen->fill_screen(main_theme->background);   // clear screen

    // display score
    std::string score = "Score: " + std::to_string(destroyed_aliens_nbr * 100);
    screen->draw_text(10, 10, FontType::WinFreeSystem14x16, score, main_theme->text);

    // render shields
    for (auto &[shield, ent] : shields) {
        for (int y = 0; y < shield->height; ++y) {
            for (int x = 0; x < shield->width; ++x) {
                if (shield->at(x,y)) {
                    screen->draw_pixel(ent.pos_x + x, ent.pos_y + y, main_theme->shield);
                }
            }
        }
    }

    // draw entities (turret + aliens)
    for (size_t i = 0; i < entities.size(); ++i) {
        auto &e = entities[i];
        auto color = (i == TURRET_POS) ? main_theme->turret : main_theme->aliens;
        screen->draw_sprite(e.pos_x, e.pos_y, *e.sprite, color);
    }

    // draw shots
    for (auto &s : shots) {
        screen->draw_sprite(s.pos_x, s.pos_y, *s.sprite, main_theme->selection);
    }

    screen->flush();  // update display
}

//---- Helpers ---------------------------------------------------------------
int GameModule::update_base_position() {
    int dx = spiled->read_knob_change(KnobColor::Green) * 2;
    int x = turret_x + dx;
    // constrain to screen
    x = std::clamp(x, 0, SCREEN_WIDTH - base.width);
    return x;
}

void GameModule::update_shots() {
    for (auto it = shots.begin(); it != shots.end();) {
        // move shot up or down
        it->pos_y += (it->sprite == &alien_shot) ? 7 : -15;

        bool hit = handle_shield_collisions(*it) || handle_entity_collisions(*it);
        bool offscreen = it->pos_y + it->sprite->height < 0 || it->pos_y > SCREEN_HEIGHT;

        if (hit || offscreen) {
            it = shots.erase(it);
        } else {
            ++it;
        }
    }
}

void GameModule::update_alien_positions() {
    bool edge = false;
    // check for edges and loss condition
    for (size_t i = 1; i < entities.size(); ++i) {
        auto &e = entities[i];
        if (e.sprite == &blank_sprite) continue;
        if (e.pos_y + e.sprite->height >= 375) {
            switch_to(StateFlag::Loss);
            return;
        }
        int nx = e.pos_x + alien_speed * alien_direction;
        if (nx < 0 || nx + e.sprite->width >= SCREEN_WIDTH) { edge = true; break; }
    }
    // move down and reverse or step sideways
    for (size_t i = 1; i < entities.size(); ++i) {
        auto &e = entities[i];
        if (e.sprite == &blank_sprite) continue;
        if (edge) e.pos_y += 5;
        else      e.pos_x += alien_speed * alien_direction;
    }
    if (edge) alien_direction = -alien_direction;
}

void GameModule::aliens_shoot() {
    if (alien_shot_cooldown) return;

    std::vector<size_t> shooters;
    for (size_t i = 1; i < entities.size(); ++i) {
        if (entities[i].is_shooter && entities[i].sprite != &blank_sprite)
            shooters.push_back(i);
    }
    if (shooters.empty()) return;

    // pick random shooter
    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<> dist(0, shooters.size()-1);
    auto &sh = entities[shooters[dist(gen)]];

    // spawn alien shot
    Entity shot{ sh.pos_x + sh.sprite->width/2 - alien_shot.width/2,
                 sh.pos_y + sh.sprite->height,
                 &alien_shot };
    shots.push_back(shot);
    buzzer->play_tone(Tone::EnemyMissile, 250);
    alien_shot_cooldown = alien_shot_cooldown_time;
}

bool GameModule::handle_shield_collisions(Entity &shot) {
    for (auto &[shield, ent] : shields) {
        int rx = shot.pos_x - ent.pos_x;
        int ry = shot.pos_y - ent.pos_y;
        if (rx < 0 || ry < 0 || rx >= shield->width || ry >= shield->height)
            continue;  // outside shield

        // pixel-level check
        for (int sy = 0; sy < shot.sprite->height; ++sy)
            for (int sx = 0; sx < shot.sprite->width; ++sx)
                if (shield->at(rx+ sx, ry+ sy)) {
                    shield->damage_area(rx+ sx, ry+ sy, 10); // peel shield
                    return true;
                }
    }
    return false;
}

bool GameModule::handle_entity_collisions(Entity &shot) {
    for (size_t i = 0; i < entities.size(); ++i) {
        auto &t = entities[i];
        if (t.sprite == &blank_sprite) continue;

        bool overlap = shot.pos_x < t.pos_x + t.sprite->width &&
                       shot.pos_x + shot.sprite->width > t.pos_x &&
                       shot.pos_y < t.pos_y + t.sprite->height &&
                       shot.pos_y + shot.sprite->height > t.pos_y;
        if (!overlap) continue;

        // turret hit by alien
        if (i == TURRET_POS && shot.sprite == &alien_shot) {
            --turret_lives;
            if (turret_lives <= 0) switch_to(StateFlag::Loss);
            return true;
        }
        // alien hit by turret
        if (i != TURRET_POS && shot.sprite == &turret_shot) {
            // assign new shooter below
            if (i >= 8 && entities[i-7].sprite != &blank_sprite)
                entities[i-7].is_shooter = true;

            t.sprite = &blank_sprite;  // mark dead
            ++destroyed_aliens_nbr;
            alien_speed = 1 + destroyed_aliens_nbr / 5;
            if (destroyed_aliens_nbr >= 21) switch_to(StateFlag::Win);
            return true;
        }
    }
    return false;
}

void GameModule::reset_game() {
    // reset positions and counters
    int turret_y = SCREEN_HEIGHT - base.height - 5;
    turret_x = (SCREEN_WIDTH - base.width) / 2;

    destroyed_aliens_nbr = 0;
    alien_speed = 1;
    alien_direction = 1;

    shots.clear();
    turret_lives = 4;

    // respawn entities and shields
    update();  // reuse constructor logic for setup

    shield_1.reset_shield(); shield_2.reset_shield();
    shield_3.reset_shield(); shield_4.reset_shield();
}