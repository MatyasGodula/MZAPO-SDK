#pragma once

#include <cstdint>



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

private:
    float frequency;
    float volume;
};

constexpr Tone Tone::PlayerMissile = Tone(0.5, 0.5);
constexpr Tone Tone::EnemyMissile = Tone(0.2, 0.5);
constexpr Tone Tone::Explosion = Tone(0.8, 1);


class AudioDriver {
    void* pwm_reg;

public:
    AudioDriver();
    ~AudioDriver();

    void play_tone(Tone tone);

};