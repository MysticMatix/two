#pragma once

#include "level.hpp"
#include "buttons.hpp"
#include "moving.hpp"

class LevelView{
    public:
        /// @brief Default constructor
        LevelView(int x, int y, int width, int height);
        /// @brief Default destructor
        ~LevelView();

        /// @brief Load a level
        /// @param level The level to load
        void load(LevelSave level);

        /// @brief update the level
        void update();

        /// @brief Render the level
        void render();

        /// @brief Handle events for the level
        bool handleEvents(SDL_Event event);

        /// @brief Clean up the level
        void clean();

        std::pair<int, int> getStars(){
            return level->getStars();
        }

        Uint8 opacity = 0;

    private:
        /// @brief The x position of the level
        int x;
        /// @brief The y position of the level
        int y;
        /// @brief The width of the level
        int width;
        /// @brief The height of the level
        int height;
        
        /// @brief The level
        Level *level;
        buttons::ButtonLayer *buttonLayer;

        buttons::Button * restartButton;
        buttons::Button * continueButton;

        SDL_Rect miniRestartButton;
    
        SDL_Rect bigRestartButton;

        SDL_Rect mediumRestartButton;
        SDL_Rect bigContinueButton;

        GameStatus lastGameStatus = GameStatus::PLAYING;

        moving::moveManager moveManager;
        moving::Task * restartButtonTask = nullptr;
        moving::Task * continueButtonTask = nullptr;

        int currentLevel = 0;
};