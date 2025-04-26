#pragma once

#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <cstdint>
#include <atomic>
#include <mutex>


struct Tone {
public: 
    constexpr Tone(float frequency, float volume)
    : frequency(frequency), volume(volume) {}

    constexpr float get_frequency() {
        return frequency;
    }

    constexpr float get_volume() {
        return volume;
    }

    static const Tone PlayerMissile;
    static const Tone EnemyMissile;
    static const Tone Explosion;
    static const Tone NullTone;

private:
    float frequency;
    float volume;
};

constexpr Tone Tone::PlayerMissile = Tone(0.5, 0.5);
constexpr Tone Tone::EnemyMissile = Tone(0.2, 0.5);
constexpr Tone Tone::Explosion = Tone(0.8, 1);
constexpr Tone Tone::NullTone = Tone(0.0, 0.0);


class AudioDriver {
private:
    void* pwm_reg;
    std::atomic<bool> new_tone_requested = false;
    std::mutex current_tone_mutex;
    Tone current_tone = Tone::NullTone;

public:
    AudioDriver();
    ~AudioDriver();

    void play_tone(Tone tone);
    void audio_thread_loop();

    void stop_tone();

};