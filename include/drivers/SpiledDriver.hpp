// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Matyas Godula

/// @file SpiledDriver.hpp
/// @brief Class to control all of SPILED hardware.
/// @author Matyas Godula
/// @date 28.4.2025

#pragma once

#include <cstdint>

enum class KnobColor { 
    Red, 
    Green, 
    Blue 
};

namespace Constants {
    namespace Hardware {
        constexpr int LedLineSize = 32;
    };
} // namespace Constants

/// @brief Class to control all of SPILED hardware.
class SpiledDriver {
    public:
        /// @brief Constructor for the SpiledDriver class.
        SpiledDriver(void *spiled_mem_base);

        /// @brief Destructor for the SpiledDriver class.
        ~SpiledDriver();

        /// @brief Initializes the LED line with the specified number of LEDs.
        /// @param n_of_led_blocks The number of LEDs in the line.
        /// @return True if the initialization was successful, false otherwise.
        /// @note The number of LEDs must be a divisor of the LED line size.
        bool init_led_line(uint8_t n_of_led_blocks);

        /// @brief Sets all the LEDs in the line in blocks.
        /// @param leds The number of LEDs to set.
        /// @note The LEDs are set in blocks, so if n_of_led_blocks is 4 and leds = 1, the first
        /// 32/4 = 8 LEDs will be set.
        void set_led_line(uint8_t leds);

        /// @brief Sets the LED line to the specified value.
        /// @param led_line The value to set the LED line to.
        /// @note The value is a 32-bit integer where each bit represents an LED.
        void set_led_line(uint32_t led_line);

        /// @brief Clears the LED line.
        void clear_led_line();

        /// @brief Sets the knob values for the red, green, and blue knobs.
        /// @note This function reads the current values of the knobs and stores them.
        void init_knobs();

        /// @brief Reads the change in a knob's position.
        /// @param color The color of the knob to read.
        /// @return The relative change in value of the knob.
        /// @note Overlflow is handled in the method.
        int read_knob_change(KnobColor color);

        /// @brief Reads whether or not a knob has been pressed.
        /// @param color The color of the knob to read.
        /// @return True if the knob has been pressed, false otherwise.
        bool read_knob_press(KnobColor color);

        /// @brief Reads the value of a knob.
        /// @param color The color of the knob to read.
        uint8_t read_knob_val(KnobColor color);

    private:
        void *spiled_mem_base;
        uint8_t red_prev_knob_val;
        uint8_t green_prev_knob_val;
        uint8_t blue_prev_knob_val;
        uint8_t n_of_led_blocks;
        uint32_t led_line;
        uint8_t led_section_size;

        /// @brief Sets a specific bit in the LED line.
        /// @param index The index of the LED to set.
        /// @note The index is from the left.
        void set_led_bit(uint8_t index);
};