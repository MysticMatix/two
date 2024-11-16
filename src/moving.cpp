#include "moving.hpp"
#include <iostream>


//#define debug

moving::moveManager::moveManager(){
    this->tasks = std::list<moving::Task*>();
}

moving::moveManager::~moveManager(){
    std::list<moving::Task*>::iterator it = this->tasks.begin();
    while(it != this->tasks.end()){
        delete *it;
        it++;
    }
}

void moving::moveManager::update(){
    #ifdef debug
    //std::cout << "moveMAnager::update" << std::endl;
    #endif
    for(auto task : this->tasks){
        task->update();
    }
    #ifdef debug
    //std::cout << "moveMAnager::update end" << std::endl;
    #endif
}

void moving::moveManager::addTask(Task* task){
    this->tasks.push_back(task);
}

void moving::moveManager::removeTask(Task* task){
    #ifdef debug
    std::cout << "moveMAnager::removeTask address: " << task << std::endl;
    #endif
    this->tasks.remove(task);
    #ifdef debug
    std::cout << "moveMAnager::removeTask finished" << std::endl;
    #endif
}

void moving::moveManager::removeFinishedTasks(){
    #ifdef debug
    std::cout << "moveMAnager::removeFinishedTasks" << std::endl;
    #endif
    tasks.remove_if([](moving::Task * task){
        return task->finished;
    });
    #ifdef debug
    std::cout << "moveMAnager::removeFinishedTasks end" << std::endl;
    #endif
}

using namespace moving;

void tasksQueue::update(){
    #ifdef debug
    std::cout << "Updating tasksQueue, size: " << this->tasks.size() << std::endl;
    #endif
    if(this->tasks.size() > 0){
        this->tasks.front()->update();
        #ifdef debug
        std::cout << "Finished: " << this->tasks.front()->finished << std::endl;
        #endif
        if(this->tasks.front()->finished){
            this->tasks.pop();
        }
    }
    
    finished = this->tasks.size() == 0;
}

InstantChangeTask::InstantChangeTask(movable* object, SDL_Rect targetRect){
    this->object = object;
    this->targetRect = targetRect;
}

void InstantChangeTask::update(){
    this->object->rect = this->targetRect;
    this->finished = true;
}

LinearMoveTask::LinearMoveTask(movable* object, int targetX, int targetY, int speed){
    this->object = object;
    this->targetX = targetX;
    this->targetY = targetY;
    this->speed = speed;
    this->finished = false;
}

void LinearMoveTask::update(){
    if(this->object->rect.x < this->targetX){
        this->object->rect.x += this->speed;
        if(this->object->rect.x > this->targetX){
            this->object->rect.x = this->targetX;
        }
    }
    else if(this->object->rect.x > this->targetX){
        this->object->rect.x -= this->speed;
        if(this->object->rect.x < this->targetX){
            this->object->rect.x = this->targetX;
        }
    }

    if(this->object->rect.y < this->targetY){
        this->object->rect.y += this->speed;
        if(this->object->rect.y > this->targetY){
            this->object->rect.y = this->targetY;
        }
    }
    else if(this->object->rect.y > this->targetY){
        this->object->rect.y -= this->speed;
        if(this->object->rect.y < this->targetY){
            this->object->rect.y = this->targetY;
        }
    }

    if(this->object->rect.x == this->targetX && this->object->rect.y == this->targetY){
        this->finished = true;
    }
    else {
        this->finished = false;
    }
}

linearSizeTask::linearSizeTask(movable* object, int targetW, int targetH, int speed){
    this->object = object;
    this->targetW = targetW;
    this->targetH = targetH;
    this->speed = speed;
    this->finished = false;
}

void linearSizeTask::update(){
    if(this->object->rect.w < this->targetW){
        this->object->rect.w += this->speed;
        if(this->object->rect.w > this->targetW){
            this->object->rect.w = this->targetW;
        }
    }
    else if(this->object->rect.w > this->targetW){
        this->object->rect.w -= this->speed;
        if(this->object->rect.w < this->targetW){
            this->object->rect.w = this->targetW;
        }
    }

    if(this->object->rect.h < this->targetH){
        this->object->rect.h += this->speed;
        if(this->object->rect.h > this->targetH){
            this->object->rect.h = this->targetH;
        }
    }
    else if(this->object->rect.h > this->targetH){
        this->object->rect.h -= this->speed;
        if(this->object->rect.h < this->targetH){
            this->object->rect.h = this->targetH;
        }
    }

    if(this->object->rect.w == this->targetW && this->object->rect.h == this->targetH){
        this->finished = true;
    }
    else {
        this->finished = false;
    }
}

linearRectTask::linearRectTask(movable* object, SDL_Rect targetRect, int speed){
    this->sizeTask.object = object;
    this->sizeTask.targetW = targetRect.w;
    this->sizeTask.targetH = targetRect.h;
    this->sizeTask.speed = speed;
    this->sizeTask.finished = false;

    this->moveTask.object = object;
    this->moveTask.targetX = targetRect.x;
    this->moveTask.targetY = targetRect.y;
    this->moveTask.speed = speed;
    this->moveTask.finished = false;
}

void linearRectTask::update(){
    this->sizeTask.update();
    this->moveTask.update();
    this->finished = this->sizeTask.finished && this->moveTask.finished;
}

SmoothMoveTask::SmoothMoveTask(movable* object, int targetX, int targetY, double speed){
    this->object = object;
    this->targetX = targetX;
    this->targetY = targetY;
    this->speed = speed;
    this->finished = false;
}

void SmoothMoveTask::update(){
    this->object->rect.x = this->targetX + (this->object->rect.x - this->targetX) * (1 - this->speed);
    this->object->rect.y = this->targetY + (this->object->rect.y - this->targetY) * (1 - this->speed);
    if(this->object->rect.x == this->targetX && this->object->rect.y == this->targetY){
        this->finished = true;
    }
    else{
        this->finished = false;
    }
}

void smoothSizeTask::update(){
    this->object->rect.w = this->targetW + (this->object->rect.w - this->targetW) * (1 - this->speed);
    this->object->rect.h = this->targetH + (this->object->rect.h - this->targetH) * (1 - this->speed);
    if(this->object->rect.w == this->targetW && this->object->rect.h == this->targetH){
        this->finished = true;
    }
    else{
        this->finished = false;
    }
}

smoothRectTask::smoothRectTask(movable* object, SDL_Rect targetRect, double speed){
    this->sizeTask.object = object;
    this->sizeTask.targetW = targetRect.w;
    this->sizeTask.targetH = targetRect.h;
    this->sizeTask.speed = speed;
    this->sizeTask.finished = false;

    this->moveTask.object = object;
    this->moveTask.targetX = targetRect.x;
    this->moveTask.targetY = targetRect.y;
    this->moveTask.speed = speed;
    this->moveTask.finished = false;
}

void smoothRectTask::update(){
    this->sizeTask.update();
    this->moveTask.update();
    this->finished = this->sizeTask.finished && this->moveTask.finished;
}