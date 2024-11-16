#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <list>
#include <vector>

#include <queue>

#include "levelSave.hpp"

enum Direction{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum GameStatus{
    PLAYING,
    WON,
    LOST
};

struct PlayerData{
    Point start;
    Point end;
    Point pos;
    DPoint renderPos;

    bool dead;
    bool won;
};

class Level{
    public:
        /// @brief Default constructor
        Level(int x, int y, int width, int height);
        /// @brief Default destructor
        ~Level();

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
            return std::make_pair(this->stars, this->starCount);
        }

        GameStatus gameStatus;

        void restart();

    private:
        LevelSave level;

        std::queue<Direction> moveQueue;

        Uint64 animationTime = 0;

        SDL_Texture *playerTexture;
        SDL_Texture *starTexture;
        SDL_Texture *spikeTexture;
        SDL_Texture *wallTexture;
        SDL_Texture *endTexture;

        DRect visibleBlocks;

        DPoint displacement;
        /// @brief The combined displacement of the level
        Point combinedDisplacement;

        /// @brief The size of the blocks
        int blockSize;

        bool playerStillMoving = false;

        /// @brief The x position of the level
        int x;
        /// @brief The y position of the level
        int y;
        /// @brief The width of the level
        int width;
        /// @brief The height of the level
        int height;

        /// @brief The width of the map
        int mapWidth;
        /// @brief The height of the map
        int mapHeight;

        /// @brief The active blocks in the level
        std::list<Point> blocks;

        /// @brief The blocks in the level
        std::vector<std::vector<Block>> map;

        /// @brief the player data
        PlayerData playerData[2];

        /// @brief how many stars the players have collected
        int stars;
        /// @brief how many stars there are in the level
        int starCount;

        /// @brief Update a block
        inline void updateBlock(Block* block);

        /// @brief Render a block
        inline void renderBlock(Block* block);

        /// @brief Update the player
        /// @param player The player to update
        void updatePlayer(int player);

        /// @brief Render the player
        /// @param player The player to render
        void renderPlayer(int player);

        void renderEnd(int player);

        /// @brief Move the player
        /// @param player The player to move
        /// @param direction The direction to move the player
        void movePlayer(int player, Direction direction);
};