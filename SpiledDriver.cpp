#include "SpiledDriver.hpp"

#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <cstdint>
#include <iostream>


SpiledDriver::SpiledDriver(void* spiled_mem_base) :
    spiled_mem_base(spiled_mem_base),
    red_prev_knob_val(0),
    green_prev_knob_val(0),
    blue_prev_knob_val(0),
    n_of_led_blocks(0),
    led_line(0),
    led_section_size(0)
{}

SpiledDriver::~SpiledDriver() {
    clear_led_line();
}

bool SpiledDriver::init_led_line(uint8_t n_of_led_blocks) {
    this->n_of_led_blocks = n_of_led_blocks;
    led_line = 0;
    if (Constants::Hardware::LedLineSize % n_of_led_blocks != 0) {
        return false;
    }
    led_section_size = Constants::Hardware::LedLineSize / n_of_led_blocks;
    std::cout << static_cast<int>(led_section_size) << "\n";
    return true;
}

void SpiledDriver::set_led_bit(uint8_t index) {
    
    int reverse_index = Constants::Hardware::LedLineSize - index - 1;
    if (index < Constants::Hardware::LedLineSize) {
        led_line |= (1 << reverse_index);
    }
}

void SpiledDriver::set_led_line(uint32_t led_line) {
    this->led_line = led_line; 
    *(volatile uint32_t *)(spiled_mem_base + SPILED_REG_LED_LINE_o) = led_line;
}

void SpiledDriver::set_led_line(uint8_t leds) {
    for (int i = 0; i < leds; i++) {
        for (int j = 0; j < led_section_size; j++) {
            set_led_bit(i * led_section_size + j);
        }
    }
    set_led_line(this->led_line);
}

void SpiledDriver::clear_led_line() {
    led_line = 0;
    set_led_line(led_line);
}

void SpiledDriver::init_knobs() {
    red_prev_knob_val = read_knob_val(KnobColor::Red);
    green_prev_knob_val = read_knob_val(KnobColor::Green);
    blue_prev_knob_val = read_knob_val(KnobColor::Blue);
}

uint8_t SpiledDriver::read_knob_val(KnobColor color) {
    uint32_t knob_data = *(volatile uint32_t*)(spiled_mem_base + SPILED_REG_KNOBS_8BIT_o);
    uint8_t knob_val = 0;
    switch (color) {
        case KnobColor::Blue:
            knob_val = (knob_data >> 0) & 0xFF;
            break;
        case KnobColor::Green:
            knob_val = (knob_data >> 8) & 0xFF;
            break;
        case KnobColor::Red:
            knob_val = (knob_data >> 16) & 0xFF;
            break;
    }
    return knob_val;
}

int SpiledDriver::read_knob_change(KnobColor color) {
    uint8_t knob_val = read_knob_val(color);
    int delta = 0;
    switch (color) {
        case KnobColor::Blue:
            delta = knob_val - blue_prev_knob_val;
            blue_prev_knob_val = knob_val;
            break;
        case KnobColor::Green:
            delta = knob_val - green_prev_knob_val;
            green_prev_knob_val = knob_val;
            break;
        case KnobColor::Red:
            delta = knob_val - red_prev_knob_val;
            red_prev_knob_val = knob_val;
            break;
    }
    if (delta > 127) delta -= 256;
    if (delta < -127) delta += 256;
    return delta;
}

bool SpiledDriver::read_knob_press(KnobColor color) {
    uint32_t knob_data = *(volatile uint32_t*)(spiled_mem_base + SPILED_REG_KNOBS_8BIT_o);
    uint32_t buttons = (knob_data >> 24) & 0x0F;
    switch (color) {
        case KnobColor::Blue:
            return (buttons & 0x01) != 0;
        case KnobColor::Green:
            return (buttons & 0x02) != 0;
        case KnobColor::Red:
            return (buttons & 0x04) != 0;
    }
    
}