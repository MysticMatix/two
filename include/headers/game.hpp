#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "levelView.hpp"

#include <future>

class Game {
    public:
        /// @brief Default constructor
        Game();
        /// @brief Default destructor
        ~Game();

        /// @brief Initialize the game
        /// @param title The title of the window
        /// @param xpos The x position of the window
        /// @param ypos The y position of the window
        /// @param width The width of the window (if not fullscreen)
        /// @param height The height of the window (if not fulscreen)
        /// @param fullscreen Whether the window should be fullscreen
        void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

        /// @brief Handle events for the game, called in a separate thread
        void handleEvents();

        /// @brief Run the game
        void run();

        /// @brief Update the game
        void update();

        /// @brief Render the game
        void render();

        /// @brief Clean up the game
        void clean();

        /// @brief Check if the game is running
        bool running() { return isRunning; }

        /// @brief The renderer for the game
        inline static SDL_Renderer* renderer;

        /// @brief The width of the window
        inline static int windowWidth;

        /// @brief The height of the window
        inline static int windowHeight;

    private:
        /// @brief Whether the game is running
        bool isRunning;

        /// @brief The window for the game
        SDL_Window* window;

        /// @brief The event handler thread
        std::future<void> eventHandler;

        /// @brief The level view
        LevelView *levelView;
};