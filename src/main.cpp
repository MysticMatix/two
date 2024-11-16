#include "game.hpp"

#include <iostream>

int main(int argc, char* args[]){
    Game game;
    game.init("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);
    game.run();
}