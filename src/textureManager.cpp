#include "textureManager.hpp"

#include "game.hpp"

SDL_Texture* textureManager::loadTexture(const char* fileName){
    SDL_Surface* tempSurface = IMG_Load(fileName);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}