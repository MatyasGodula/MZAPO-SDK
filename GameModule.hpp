#pragma once

#include <ctime>
#include <utility>
#include <vector>

#include "AlienShotSprite.hpp"
#include "AlienSprites.hpp"
#include "AudioDriver.hpp"
#include "BaseSprite.hpp"
#include "BlankSprite.hpp"
#include "DisplayDriver.hpp"
#include "Entity.hpp"
#include "Module.hpp"
#include "ShieldSprite.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"
#include "TurretShotSprite.hpp"

/// @brief GameModule class for core game logic and handling gameplay.
/// @details This class manages game state, updates, rendering, and user input during gameplay.
/// @note It handles entities, shots, shields, scoring, and transitions between game states.
class GameModule : public Module {
public:
    /// @brief Constructs a GameModule with necessary drivers and theme.
    /// @param screen_ptr Pointer to the DisplayDriver for rendering game visuals.
    /// @param buzzer_ptr Pointer to the AudioDriver for playing sound effects.
    /// @param spiled_ptr Pointer to the SpiledDriver for additional hardware integration.
    /// @param main_theme_ptr Pointer to the Theme defining game visuals and audio themes.
    /// @param current_type_ptr Pointer to the current state flag for mode switching.
    GameModule(
        DisplayDriver *screen_ptr,
        AudioDriver *buzzer_ptr,
        SpiledDriver *spiled_ptr,
        Theme *main_theme_ptr,
        StateFlag *current_type_ptr
    );

    /// @brief Destroys the GameModule and releases resources.
    ~GameModule() override;

    /// @brief Updates game state each frame.
    /// @details Handles input processing, entity updates, collision detection, and state transitions.
    void update() override;

    /// @brief Redraws the game screen.
    /// @details Renders all entities, shots, shields, and game HUD to the display.
    void redraw() override;

    /// @brief Sets up initial conditions when switching to this module.
    void switch_setup() override;

    /// @brief Switches to a different game state.
    /// @param new_mod The new state flag indicating the target module.
    void switch_to(StateFlag new_mod) override;

    /// @brief Resets the game to its initial state.
    /// @details Clears entities, shots, scores, and resets player lives and positions.
    void reset_game();

private:
    /// @name Core update routines
    /// @{ 
    /// @brief Updates the player's base (turret) position based on input.
    /// @return The updated x-coordinate of the turret.
    int update_base_position();

    /// @brief Updates all active shots in the game.
    void update_shots();

    /// @brief Updates positions of all alien entities.
    void update_alien_positions();

    /// @brief Handles alien shooting logic.
    void aliens_shoot();
    /// @}

    /// @name Collision handling
    /// @{ 
    /// @brief Checks for collisions between a shot and shields.
    /// @param shot The shot entity to check.
    /// @return True if the shot collides with a shield.
    bool handle_shield_collisions(Entity &shot);

    /// @brief Checks for collisions between a shot and other entities (aliens, turret).
    bool handle_entity_collisions(Entity &shot);
    /// @}

    /// @name Hardware interfaces
    /// @{ 
    DisplayDriver *const screen;     ///< Display driver for rendering
    AudioDriver *const buzzer;       ///< Audio driver for sound effects
    SpiledDriver *const spiled;      ///< Spiled hardware driver
    Theme *const main_theme;         ///< Current visual/audio theme
    StateFlag *const current_type;   ///< Pointer to current module state
    /// @}

    /// @name Sprites and entities
    /// @{ 
    BaseSprite base;                  ///< Player's turret sprite
    InvaderA inv_a;                  ///< Alien type A sprite
    InvaderB inv_b;                  ///< Alien type B sprite
    InvaderC inv_c;                  ///< Alien type C sprite
    AlienShotSprite alien_shot;      ///< Sprite for alien shots
    TurretShotSprite turret_shot;    ///< Sprite for turret shots
    ShieldSprite shield_1, shield_2, shield_3, shield_4;  ///< Shield sprites
    BlankSprite blank_sprite;         ///< Placeholder sprite
    /// @}

    /// @brief Base addresses for display memory
    unsigned char *lcd_mem_base = nullptr; ///< LCD memory base pointer
    unsigned char *spi_mem_base = nullptr; ///< SPI memory base pointer

    /// @brief Active game entities and shots
    std::vector<Entity> entities;    ///< Active aliens and other entities
    std::vector<Entity> shots;       ///< Active shot entities
    std::vector<std::pair<ShieldSprite *, Entity>> shields; ///< Shield positions and entities

    /// @name Gameplay state variables
    /// @{ 
    int turret_x = 0;                ///< Current x-position of the turret
    uint8_t prev_knob = 0;           ///< Previous knob input value
    int turret_shot_cooldown = 0;    ///< Cooldown timer for turret shots
    const int turret_shot_cooldown_time = 10; ///< Frames between turret shots
    int alien_shot_cooldown = 0;     ///< Cooldown timer for alien shots
    const int alien_shot_cooldown_time = 50; ///< Frames between alien shots

    int destroyed_aliens_nbr = 0;    ///< Count of destroyed aliens
    int alien_direction = 1;         ///< Current horizontal movement direction of aliens
    int alien_speed = 1;             ///< Speed multiplier for alien movement

    int turret_lives = 4;            ///< Remaining player lives
    /// @}

    /// @brief Delay timing for game loop
    struct timespec loop_delay;      ///< Time delay between game loop iterations
};
