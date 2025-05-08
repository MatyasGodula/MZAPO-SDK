#include "AudioDriver.hpp"

#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <expected>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <stop_token>
#include <thread>

AudioDriver::AudioDriver() : stop_source() {
    pwm_reg = static_cast<void *>(map_phys_address(AUDIOPWM_REG_BASE_PHYS, AUDIOPWM_REG_SIZE, 0));

    if (pwm_reg == nullptr) {
        throw std::runtime_error("Failed to map physical address");
    }

    // Start the worker thread and provide it with our controled stop token.
    worker = std::jthread(
        // Capture the current instance and pass the stop token to the lambda.
        // A little note: This is actually very Rust-like.
        [this](std::stop_token token) { audio_thread_loop(token); },
        // Pass our own stop token that is used in the destructor.
        stop_source.get_token());
}

AudioDriver::~AudioDriver() {
    stop_source.request_stop();
    // Added this line ^^ to try and prevent a potential deadlock.
    condvar.notify_one();
}

void AudioDriver::set_tone(uint32_t frequency_hz, uint32_t duty_pct) {
    if (pwm_reg == nullptr) {
        throw std::runtime_error("Trying to access a non mapped address");
    }

    *(volatile uint32_t *)(pwm_reg + AUDIOPWM_REG_PWMPER_o) = frequency_hz;
    *(volatile uint32_t *)(pwm_reg + AUDIOPWM_REG_PWM_o) = duty_pct;
    *(volatile uint32_t *)(pwm_reg + AUDIOPWM_REG_CR_o) = 1; // Enable
}

void AudioDriver::turn_off_buzzer() {
    *(volatile uint32_t *)(pwm_reg + AUDIOPWM_REG_PWMPER_o) = 0;
    *(volatile uint32_t *)(pwm_reg + AUDIOPWM_REG_PWM_o) = 0;
    *(volatile uint32_t *)(pwm_reg + AUDIOPWM_REG_CR_o) = 0; // Disable
}

/// @note I tried to get as close to a Rust implementation as possible.
void AudioDriver::audio_thread_loop(std::stop_token stop_token) {
    Tone tone = Tone::NullTone;
    int duration_ms = 0;
    while (!stop_token.stop_requested()) {
        // Uses a more Rust-like approach to handling mutexes.
        {
            // Lock the mutex for the condvar.
            std::unique_lock lock(current_tone_mutex);
            // Condvar waits for a notification and checks the stop token and new tone flag.
            condvar.wait(lock, [this, &stop_token]() {
                return new_tone_requested || stop_token.stop_requested();
            });

            // If the stop token has been rquested, break out of the loop.
            if (stop_token.stop_requested()) {
                break;
            }

            // Copy important data to local variables.
            tone = current_tone;
            duration_ms = current_duration_ms;
            // Reset the new tone flag.
            new_tone_requested = false;
            // End of the lock guard, mutex is automatically released here.
        }

        // Check if the tone is null or has a frequency of 0.
        if (tone.get_frequency() <= 0) {
            turn_off_buzzer();
            continue;
        }

        // Set the tone on the buzzer.
        set_tone(tone.get_frequency(), tone.get_duty());

        // Start the timer for the duration of the tone.
        auto start_time = std::chrono::steady_clock::now();
        // Periodically check the timer so we know when to stop the tone.
        while (std::chrono::steady_clock::now() - start_time <
               std::chrono::milliseconds(duration_ms)) {
            // Check the stop token on every iteration.
            if (stop_token.stop_requested()) {
                break;
            }
            // No need for perpetual checking.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        // After the duration, turn off the buzzer.
        turn_off_buzzer();
    }
    // Stop requested, end the thread.
    std::cout << "Stop requested, exiting audio thread!..." << std::endl;
    turn_off_buzzer();
}

void AudioDriver::play_tone(Tone tone, int duration_ms) {
    {
        // Lock the mutex, use lock_guard because we don't wait for condvars.
        std::lock_guard lock(current_tone_mutex);
        current_tone = tone;
        current_duration_ms = duration_ms;
        new_tone_requested = true;
    }
    // Notify the worker thread.
    condvar.notify_one();
}

void AudioDriver::stop_tone() {
    {
        std::lock_guard lock(current_tone_mutex);
        current_tone = Tone::NullTone;
        current_duration_ms = 0;
        new_tone_requested = true;
    }
    condvar.notify_one();
}