#include "game.hpp"

#include <iostream>

Game::Game(){
    this->isRunning = false;
}

Game::~Game(){
    this->clean();
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen){
    int flags = 0;
    if(fullscreen){
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0){
        std::cout << "Subsystems initialized" << std::endl;

        this->window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if(this->window){
            std::cout << "Window created" << std::endl;
        }

        Game::renderer = SDL_CreateRenderer(this->window, -1, 0);
        if(Game::renderer){
            SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
            std::cout << "Renderer created" << std::endl;
            SDL_GetRendererOutputSize(Game::renderer, &Game::windowWidth, &Game::windowHeight);
            std::cout << "Window size: " << Game::windowWidth << "x" << Game::windowHeight << std::endl;
            width = Game::windowWidth;
            height = Game::windowHeight;
        }

        this->isRunning = true;
    }else{
        this->isRunning = false;
    }

    SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_BLEND);

    this->levelView = new LevelView(0, 0, width, height);
}

void Game::handleEvents(){
    SDL_Event event;
    while(this->isRunning){
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    this->isRunning = false;
                    break;
                default:
                    this->levelView->handleEvents(event);
                    break;
            }
        }
    }
}

void Game::update(){
    this->levelView->update();

    SDL_SetWindowTitle(this->window, ("stars: " + std::to_string(this->levelView->getStars().first) + "/" + std::to_string(this->levelView->getStars().second)).c_str());
}

void Game::render(){
    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(Game::renderer);

    this->levelView->render();

    SDL_RenderPresent(Game::renderer);
}

void Game::clean(){
    delete this->levelView;
    SDL_DestroyWindow(this->window);
    SDL_DestroyRenderer(Game::renderer);
    SDL_Quit();
    std::cout << "Game cleaned" << std::endl;
}

static int framteRate = 60;
static int frameDelay = 1000 / framteRate;

void Game::run(){
    this->eventHandler = std::async(std::launch::async, [this](){
        while(this->isRunning){
            this->handleEvents();
            SDL_Delay(frameDelay/2);
        }
    });

    while(this->isRunning){
        Uint64 start = SDL_GetTicks64();

        this->update();
        this->render();

        if(frameDelay > SDL_GetTicks64() - start){
            SDL_Delay(frameDelay - (SDL_GetTicks64() - start));
        }
    }

    this->eventHandler.wait();
}