/// @file AudioDriver.hpp
/// @brief Audio driver for the MZ-APO board
/// @author Matyas Godula
/// @date 26.4.2023

#pragma once

#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <thread>

/// @brief A class representing a tone to be played.
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

/// @brief Constants for the audio driver.
/// @note This is a place to centralize sounds for the game.
constexpr Tone Tone::PlayerMissile = Tone(440000, 1000);
constexpr Tone Tone::EnemyMissile = Tone(300000, 1000);
constexpr Tone Tone::Explosion = Tone(1000000, 1000);
constexpr Tone Tone::NullTone = Tone(500000, 1000);


/// @brief Audio driver for the PWM on the MZ-APO board.
/// @note This class uses a worker thread to not block the main thread.
/// @note The worker thread is stopped when the destructor is called.
class AudioDriver {
private:
    /// @brief Pointer to the PWM register.
    /// @note This is a pointer to the mapped physical address of the PWM register.
    void* pwm_reg;  

    /// @brief A worker thread responsible for playing sounds.
    /// @note This thread is stopped when the destructor is called.
    std::jthread worker;

    /// @brief Mutex to protect the current_tone variable.
    std::mutex current_tone_mutex;

    /// @brief Condition variable to notify the worker thread when a new tone is requested.
    std::condition_variable condvar;

    /// @brief Stop source for the worker thread.
    /// @note It is used to prevent deadlocks when the destructor is called.
    // TODO: Test whether this actually works.
    std::stop_source stop_source;

    Tone current_tone = Tone::NullTone;
    int current_duration_ms = 0;
    std::atomic<bool> new_tone_requested = false;

    /// @brief Sets tthe tone on the buzzer according to the given data.
    /// @param frequency 
    /// @param duty 
    /// @note This method is not thread safe, DO NOT CALL FROM THE MAIN THREAD!!!
    void set_tone(uint32_t frequency, uint32_t duty);

    /// @brief Same as set_tone, but sets everything to 0.
    void turn_off_buzzer();

    /// @brief The main loop of the worker thread.
    /// @param stop_token This token is used by the jthread to stop the thread.
    /// @note This method is called on the start of the worker thread.
    void audio_thread_loop(std::stop_token stop_token);

public:
    /// @brief Constructor for the AudioDriver class.
    AudioDriver();

    /// @brief Destructor for the AudioDriver class.
    /// @note This destructor first sends a stop request and then wakes it up if needed.
    /// @note It waits for the join before returning.
    ~AudioDriver();

    /// @brief Method to give a tone to the worker thread.
    /// @param tone The tone to play.
    /// @param duration_ms The duration of the tone in milliseconds.
    /// @note This method is thread safe and can be called from the main thread.
    void play_tone(Tone tone, int duration_ms);

    /// @brief Turns off the buzzer.
    /// @note This method does not stop the worker thread.
    /// @note This method is thread safe and can be called from the main thread.
    void stop_tone();

};