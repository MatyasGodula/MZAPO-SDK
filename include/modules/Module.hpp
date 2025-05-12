// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Matyas Godula

/// @file Module.hpp
/// @brief A module class for the state machine.
/// @author Matyas Godula
/// @date 29.4.2025

#pragma once

/// @brief ModuleType used in the state machine.
/// @details This enum is used to specify the type of module to switch to and which is currently
/// being executed.
enum class StateFlag {
    Settings,
    Menu,
    Game,
    Tutorial,
    Win,
    Loss,
    ResetGame,
    Exit // Does not have its own module, just tells the main loop to exit
};

/// @brief A virtual module trait implemeted by different modules.
/// @details This class is used to define the interface for all modules.
class Module {
    public:
        /// @note Disables copy constructor and assignment operator.
        Module() = default;
        Module(const Module &) = delete;
        Module &operator=(const Module &) = delete;

        /// @brief Virtual destructor for the module class.
        virtual ~Module() = default;

        /// @brief The trait interface to be implemented by derived classes.

        virtual void update() = 0;
        virtual void redraw() = 0;
        virtual void switch_setup() = 0;
        virtual void switch_to(StateFlag new_mod) = 0;
};