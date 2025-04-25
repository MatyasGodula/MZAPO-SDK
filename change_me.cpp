#include "AlienSprites.hpp"
#include "DisplayDriver.hpp"
#include "Color.hpp"
#include "BaseSprite.hpp"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <unistd.h>
#include <ctime>

#define _POSIX_C_SOURCE 200112L

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 480

int update_base_position(uint8_t knob_val, uint8_t &prev_knob_val, int current_x, int base_width) {
    int delta = knob_val - prev_knob_val;

    if (delta > 127) delta -= 256;
    if (delta < -127) delta += 256;

    prev_knob_val = knob_val;

    int new_x = current_x + delta * 2; // sens

    if (new_x < 0) new_x = 0;
    if (new_x > SCREEN_WIDTH - base_width) new_x = SCREEN_WIDTH - base_width;

    return new_x;
}


int main() {
    DisplayDriver screen(DisplayOrientation::Portrait);
    screen.fill_screen(Color::Black);

    BaseSprite base;
    InvaderA invA;
    InvaderB invB;
    InvaderC invC;


    unsigned char* lcd_mem_base = static_cast<unsigned char*>(map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0));
    unsigned char* spi_mem_base = static_cast<unsigned char*>(map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0));

    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 50 * 1000 * 1000;

    int baseY = SCREEN_HEIGHT - base.height - 5;

    uint8_t prev_knob = 0;
    int turret_x = (SCREEN_WIDTH - base.width) / 2;

    while (true) {
        uint32_t knob_data = *(volatile uint32_t*)(spi_mem_base + SPILED_REG_KNOBS_8BIT_o);
        uint8_t knob_pos = (knob_data >> 8) & 0xFF;

        turret_x = update_base_position(knob_pos, prev_knob, turret_x, base.width);

        screen.fill_screen(Color::Black);
        screen.draw_sprite(turret_x, baseY, base, Color::Green);
        screen.draw_sprite(0, 0, invA, Color::Green);
        screen.draw_sprite(150, 0, invB, Color::Green);
        screen.draw_sprite(280, 0, invC, Color::Green);
        screen.flush();

        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, nullptr);
    }

    return 0;
}