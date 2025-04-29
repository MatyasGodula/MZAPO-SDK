#pragma once

enum class ModuleType {
    Settings,
    Menu,
    Game,
    Exit // Does not have its own module, just tells the main loop to exit
};

class Module {
    public:
        Module() = default;
        Module(const Module&) = delete;
        Module& operator=(const Module&) = delete;

        virtual ~Module() = default;

        virtual void update() = 0;
        virtual void redraw() = 0;
        virtual void switch_setup() = 0;
        virtual void switch_to(ModuleType new_mod) = 0;
};