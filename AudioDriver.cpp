#include "AudioDriver.hpp"

#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <cstdint>
#include <atomic>
#include <mutex>
#include <thread>
#include <expected>
#include <stdexcept>
#include <stop_token>
#include <chrono>
#include <iostream>

AudioDriver::AudioDriver() :
    stop_source()
{
    pwm_reg = static_cast<void*>(map_phys_address(AUDIOPWM_REG_BASE_PHYS, AUDIOPWM_REG_SIZE, 0));
    
    if (pwm_reg == nullptr) {
        throw std::runtime_error("Failed to map physical address");
    }

    worker = std::jthread(
        [this](std::stop_token token) {
            audio_thread_loop(token);
        },
        stop_source.get_token()
    );
}

AudioDriver::~AudioDriver() noexcept {
    stop_source.request_stop();
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

void AudioDriver::audio_thread_loop(std::stop_token stop_token) {
    while (!stop_token.stop_requested()) {
        std::unique_lock lock(current_tone_mutex);
        condvar.wait(lock, [this, &stop_token]() { 
            return new_tone_requested || stop_token.stop_requested(); 
        });

        if (stop_token.stop_requested()) {
            break;
        }

        Tone tone = current_tone;
        int duration_ms = current_duration_ms;
        new_tone_requested = false;
        lock.unlock();

        if (tone.get_frequency() <= 0) {
            turn_off_buzzer();
            continue;
        }

        
        set_tone(tone.get_frequency(), tone.get_duty());

        auto start_time = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start_time < std::chrono::milliseconds(duration_ms)) {
            if (stop_token.stop_requested()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Sleep a little, allow for early exit
        }

        turn_off_buzzer();
    }
    std::cout << "Stop requested, exiting audio thread..." << std::endl;
    turn_off_buzzer();
}

void AudioDriver::play_tone(Tone tone, int duration_ms) {
    {
        std::lock_guard lock(current_tone_mutex);
        current_tone = tone;
        current_duration_ms = duration_ms;
        new_tone_requested = true;
    }
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