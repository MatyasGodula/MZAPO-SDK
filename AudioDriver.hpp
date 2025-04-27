/// @note There might be a deadlock somewhere in the code but idk where

#pragma once

#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <thread>

struct Tone {
public: 
    constexpr Tone(uint32_t frequency, uint32_t duty)
    : frequency(frequency), duty(duty) {}

    constexpr uint32_t get_frequency() {
        return frequency;
    }

    constexpr uint32_t get_duty() {
        return duty;
    }

    static const Tone PlayerMissile;
    static const Tone EnemyMissile;
    static const Tone Explosion;
    static const Tone NullTone;

private:
    uint32_t frequency;
    uint32_t duty;
};

constexpr Tone Tone::PlayerMissile = Tone(440000, 1000);
constexpr Tone Tone::EnemyMissile = Tone(300000, 1000);
constexpr Tone Tone::Explosion = Tone(1000000, 1000);
constexpr Tone Tone::NullTone = Tone(500000, 1000);

class AudioDriver {
private:
    void* pwm_reg;
    std::jthread worker; // The worker thread repsonsible for sound playing
    std::mutex current_tone_mutex; // Mutex to protect the current_tone
    std::condition_variable condvar; // Condition variable to notify the worker thread
    std::stop_source stop_source; // Stop source stored manually to manually stop the worker

    Tone current_tone = Tone::NullTone;
    int current_duration_ms = 0;
    std::atomic<bool> new_tone_requested = false;

    void set_tone(uint32_t frequency, uint32_t duty);

    void turn_off_buzzer();

public:
    AudioDriver();
    ~AudioDriver();

    void play_tone(Tone tone, int duration_ms);
    void audio_thread_loop(std::stop_token stop_token);

    void stop_tone();

};