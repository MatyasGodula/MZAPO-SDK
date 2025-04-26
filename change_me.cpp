#include "AlienSprites.hpp"
#include "DisplayDriver.hpp"
#include "Entity.hpp"
#include "Color.hpp"
#include "BaseSprite.hpp"
#include "ShieldSprite.hpp"
#include "ShotSprite.hpp"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <vector>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <algorithm>

#define _POSIX_C_SOURCE 200112L

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480

#define TURRET_POS 0

int update_base_position(uint8_t knob_val, uint8_t &prev_knob_val, int current_x, int base_width) {
    int delta = knob_val - prev_knob_val;
    if (delta > 127) delta -= 256;
    if (delta < -127) delta += 256;
    prev_knob_val = knob_val;
    int new_x = current_x + delta * 2;
    if (new_x < 0) new_x = 0;
    if (new_x > SCREEN_WIDTH - base_width) new_x = SCREEN_WIDTH - base_width;
    return new_x;
}

void update_shots(std::vector<Entity> &shots, std::vector<Entity> &entities,
                  std::vector<std::pair<ShieldSprite*, Entity>> &shields) {
    for (auto shot = shots.begin(); shot != shots.end(); ) {
        shot->pos_y -= 15;
        bool destroyed = false;

        // shields
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
                shield->damage_area(local_x, local_y,10);
                destroyed = true;
                break;
            }
        }

        // aliens
        if (!destroyed) {
            for (size_t i = 1; i < entities.size(); ++i) {
                Entity &target = entities[i];
                bool collide_x = shot->pos_x + shot->sprite->width > target.pos_x &&
                                 shot->pos_x < target.pos_x + target.sprite->width;
                bool collide_y = shot->pos_y + shot->sprite->height > target.pos_y &&
                                 shot->pos_y < target.pos_y + target.sprite->height;

                if (collide_x && collide_y) {
                    entities.erase(entities.begin() + i);
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

int main() {
    DisplayDriver screen(DisplayOrientation::Portrait);
    screen.fill_screen(Color::Black);

    BaseSprite base;
    InvaderA invA;
    InvaderB invB;
    InvaderC invC;
    ShotSprite shotSprite;
    ShieldSprite shield_1;
    ShieldSprite shield_2;
    ShieldSprite shield_3;
    ShieldSprite shield_4;

    unsigned char* lcd_mem_base = static_cast<unsigned char*>(map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0));
    unsigned char* spi_mem_base = static_cast<unsigned char*>(map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0));

    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 50 * 1000;

    int baseY = SCREEN_HEIGHT - base.height - 5;
    uint8_t prev_knob = 0;
    int turret_x = (SCREEN_WIDTH - base.width) / 2;

    std::vector<Entity> entities = {
        {turret_x, baseY, &base},

        {20, 30, &invA},
        {60, 30, &invA},
        {100, 30, &invA},
        {140, 30, &invA},
        {180, 30, &invA},
        {220, 30, &invA},
        {260, 30, &invA},

        {20, 80, &invB},
        {60, 80, &invB},
        {100, 80, &invB},
        {140, 80, &invB},
        {180, 80, &invB},
        {220, 80, &invB},
        {260, 80, &invB},

        {20, 130, &invC},
        {60, 130, &invC},
        {100, 130, &invC},
        {140, 130, &invC},
        {180, 130, &invC},
        {220, 130, &invC},
        {260, 130, &invC},
    };

    std::vector<std::pair<ShieldSprite*, Entity>> shields = {
        {&shield_1, {20, 300, &shield_1}},
        {&shield_2, {100, 300, &shield_2}},
        {&shield_3, {180, 300, &shield_3}},
        {&shield_4, {260, 300, &shield_4}},
    };

    std::vector<Entity> shots;
    int shot_cooldown = 0;
    const int shot_cooldown_time = 10;

    while (true) {
        uint32_t knob_data = *(volatile uint32_t*)(spi_mem_base + SPILED_REG_KNOBS_8BIT_o);
        uint8_t knob_pos = (knob_data >> 8) & 0xFF;
        uint8_t buttons = (knob_data >> 24) & 0x0F;

        if (buttons & 0x01 && shot_cooldown == 0) {
            Entity new_shot;
            new_shot.pos_x = entities[TURRET_POS].pos_x + base.width/2 - shotSprite.width/2;
            new_shot.pos_y = entities[TURRET_POS].pos_y;
            new_shot.sprite = &shotSprite;
            shots.push_back(new_shot);
            shot_cooldown = shot_cooldown_time;
        }

        turret_x = update_base_position(knob_pos, prev_knob, turret_x, base.width);
        entities[TURRET_POS].pos_x = turret_x;

        update_shots(shots, entities, shields);

        screen.fill_screen(Color::Black);

        for (auto& [shield, shield_entity] : shields) {
            for (int y = 0; y < shield->height; ++y) {
                for (int x = 0; x < shield->width; ++x) {
                    if (shield->at(x, y)) {
                        screen.draw_pixel(shield_entity.pos_x + x, shield_entity.pos_y + y, Color::Green);
                    }
                }
            }
        }

        for (auto& entity : entities) {
            screen.draw_sprite(entity.pos_x, entity.pos_y, *(entity.sprite), Color::Green);
        }

        for (auto& shot : shots) {
            screen.draw_sprite(shot.pos_x, shot.pos_y, *(shot.sprite), Color::White);
        }

        if (shot_cooldown > 0) {
            --shot_cooldown;
        }

        screen.flush();
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, nullptr);
    }

    return 0;
}
