#include "levelView.hpp"

#include "game.hpp"

#include "textureManager.hpp"

#include <iostream>

#include "levels.hpp"

using namespace moving;

//#define debug

LevelView::LevelView(int x, int y, int width, int height){
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    this->level = new Level(x, y, width, height);
    level->load(levels::getLevel(currentLevel));
    this->buttonLayer = new buttons::ButtonLayer(x, y, width, height);

    this->miniRestartButton = {x + 10, y + 10, 64, 64};
    this->bigRestartButton = {x + width / 2 - 128, y + height / 2 - 128, 256, 256};
    this->bigContinueButton = bigRestartButton;
    this->mediumRestartButton = {x + ((width / 2) - 128) / 2 - 64 + width/2 + 128, y + height / 2 - 64, 128, 128};

    this->buttonLayer->addButton(miniRestartButton, textureManager::loadTexture("assets/restart.png"), [this](){
        #ifdef debug
        std::cout << "restart button clicked\n";
        #endif
        this->level->restart();
    });

    this->buttonLayer->addButton(bigContinueButton, textureManager::loadTexture("assets/continue.png"), [this](){
        #ifdef debug
        std::cout << "continue button clicked\n";
        #endif
        if(this->level->gameStatus == GameStatus::WON){
            currentLevel++;
            this->level->load(levels::getLevel(currentLevel));
        }
    });

    restartButton = &this->buttonLayer->buttons[0];
    continueButton = &this->buttonLayer->buttons[1];

    continueButton->rect.y = y + height;

    std::cout << "LevelView created, x: " << x << ", y: " << y << ", width: " << width << ", height: " << height << std::endl;
}

LevelView::~LevelView(){
    this->clean();
}

void LevelView::load(LevelSave level){
    this->level->load(level);
}

void LevelView::update(){
    #ifdef debug
    std::cout << "LevelView::update\n";
    #endif
    this->level->update();
    if(this->level->gameStatus != lastGameStatus){
        moveManager.removeTask(restartButtonTask);
        moveManager.removeTask(continueButtonTask);

        if(this->level->gameStatus == GameStatus::WON){
            this->opacity = 100;

            InstantChangeTask * zero = new InstantChangeTask(this->restartButton, this->miniRestartButton);
            LinearMoveTask * first = new LinearMoveTask(this->restartButton, this->miniRestartButton.x, -this->miniRestartButton.h, 5);
            InstantChangeTask * second = new InstantChangeTask(this->restartButton, {this->mediumRestartButton.x, this->y + this->height, this->mediumRestartButton.w, this->mediumRestartButton.h});
            //SmoothMoveTask * third = new SmoothMoveTask(this->restartButton, this->mediumRestartButton.x, this->mediumRestartButton.y, 0.1);
            GravityTask * third = new GravityTask(&this->restartButton->rect.y, this->mediumRestartButton.y, 4.4, 50, 50);

            tasksQueue * queue = new tasksQueue();
            queue->tasks.push(zero);
            queue->tasks.push(first);
            queue->tasks.push(second);
            queue->tasks.push(third);
            restartButtonTask = queue;

            moveManager.addTask(queue);

            InstantChangeTask * zero2 = new InstantChangeTask(this->continueButton, {this->bigContinueButton.x, this->y + this->height, this->bigContinueButton.w, this->bigContinueButton.h});
            SmoothMoveTask * first2 = new SmoothMoveTask(this->continueButton, this->bigContinueButton.x, this->bigContinueButton.y, 0.1);

            tasksQueue * queue2 = new tasksQueue();
            queue2->tasks.push(zero2);
            queue2->tasks.push(first2);
            continueButtonTask = queue2;

            moveManager.addTask(queue2);

        }else if(this->level->gameStatus == GameStatus::LOST){
            this->opacity = 170;

            InstantChangeTask * zero = new InstantChangeTask(this->restartButton, this->miniRestartButton);
            LinearMoveTask * first = new LinearMoveTask(this->restartButton, this->miniRestartButton.x, -this->miniRestartButton.h, 5);
            InstantChangeTask * second = new InstantChangeTask(this->restartButton, {this->bigRestartButton.x, this->y + this->height, this->bigRestartButton.w, this->bigRestartButton.h});
            SmoothMoveTask * third = new SmoothMoveTask(this->restartButton, this->bigRestartButton.x, this->bigRestartButton.y, 0.1);

            tasksQueue * queue = new tasksQueue();
            queue->tasks.push(zero);
            queue->tasks.push(first);
            queue->tasks.push(second);
            queue->tasks.push(third);
            restartButtonTask = queue;

            continueButtonTask = new InstantChangeTask(this->continueButton, {this->bigContinueButton.x, this->y + this->height, this->bigContinueButton.w, this->bigContinueButton.h});

            moveManager.addTask(queue);
            moveManager.addTask(continueButtonTask);
        }else{
            this->opacity = 0;

            moveManager.removeFinishedTasks();

            InstantChangeTask * task = new InstantChangeTask(this->restartButton, this->miniRestartButton);
            restartButtonTask = task;
            moveManager.addTask(task);

            continueButtonTask = new InstantChangeTask(this->continueButton, {this->bigContinueButton.x, this->y + this->height, this->bigContinueButton.w, this->bigContinueButton.h});
            moveManager.addTask(continueButtonTask);
        }
        lastGameStatus = this->level->gameStatus;
    }
    moveManager.update();
    #ifdef debug
    std::cout << "LevelView::update end\n";
    #endif
}

void LevelView::render(){
    this->level->render();
    if(opacity > 0){
        SDL_Rect rect = {this->x, this->y, this->width, this->height};
        SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, opacity);
        SDL_RenderFillRect(Game::renderer, &rect);
    }
    this->buttonLayer->render();
}

bool LevelView::handleEvents(SDL_Event event){
    if(this->buttonLayer->handleEvents(event)){
        return true;
    }
    return this->level->handleEvents(event);
}

void LevelView::clean(){
    delete this->level;
    delete this->buttonLayer;
}