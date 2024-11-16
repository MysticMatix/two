#pragma once

#include <SDL2/SDL.h>

#include <list>
#include <queue>

#include <iostream>

namespace moving{
    struct movable{
        SDL_Rect rect;
    };

    struct Task{
        bool finished = false;
        virtual void update() = 0;
        virtual ~Task(){}
    };

    struct TaskGroup: virtual Task{
        std::list<Task*> tasks;

        TaskGroup(){}
        TaskGroup(std::list<Task*> tasks){
            this->tasks = tasks;
        }

        virtual void update() override{
            for(auto task: this->tasks){
                task->update();
                if(task->finished)
                    this->tasks.remove(task);
            }
            if(this->tasks.empty())
                this->finished = true;
        }
        
        ~TaskGroup(){
            while(!this->tasks.empty()){
                delete this->tasks.front();
                this->tasks.pop_front();
            }
        }
    };

    struct tasksQueue: virtual Task{
        std::queue<Task*> tasks;
        virtual void update() override;
        
        ~tasksQueue(){
            while(!this->tasks.empty()){
                delete this->tasks.front();
                this->tasks.pop();
            }
        }
    };

    struct LinearTask: virtual Task{
        LinearTask(){}
        LinearTask(int * value, int targetValue, int speed){
            this->value = value;
            this->targetValue = targetValue;
            this->speed = speed;
        }
        int * value;
        int targetValue;
        int speed;
        virtual void update() override{
            if(*value < targetValue){
                *value += speed;
                if(*value > targetValue)
                    *value = targetValue;
            }else if(*value > targetValue){
                *value -= speed;
                if(*value < targetValue)
                    *value = targetValue;
            }

            if(*value == targetValue)
                this->finished = true;
            else
                this->finished = false;
        }
    };

    struct SmoothTask: virtual Task{
        SmoothTask(){}
        SmoothTask(int * value, int targetValue, double speed){
            this->value = value;
            this->targetValue = targetValue;
            this->speed = speed;
        }
        int * value;
        int targetValue;
        double speed;
        virtual void update() override{
            *value = targetValue + (*value - targetValue) * (1 - speed);
        }
    };

    struct GravityTask: virtual Task{
        GravityTask(){}
        GravityTask(int * value, int targetValue, double acc, double velocity = 0, double maxVelocity = 0){
            this->value = value;
            this->targetValue = targetValue;
            this->acc = acc;
            this->velocity = velocity;
            this->maxVelocity = maxVelocity;
        }
        int * value;
        int targetValue;
        double acc;
        double velocity = 0; 
        double maxVelocity = 0;
        virtual void update() override{
            if(this->finished) return;

            if(*value < targetValue){
                velocity += acc;
                if(maxVelocity != 0 && velocity > maxVelocity)
                    velocity = maxVelocity;
                *value += velocity;
            }else if(*value > targetValue){
                velocity -= acc;
                if(maxVelocity != 0 && velocity < -maxVelocity)
                    velocity = -maxVelocity;
                *value += velocity;
            }

            velocity *= 0.9;

            if(*value == targetValue && abs(velocity) <= acc)
                this->finished = true;
            else
                this->finished = false;
        }
    };

    struct InstantChangeTask: virtual Task{
        InstantChangeTask(){}
        InstantChangeTask(movable* object, SDL_Rect targetRect);
        movable* object;
        SDL_Rect targetRect;
        virtual void update() override;
    };

    struct LinearMoveTask: virtual Task{
        LinearMoveTask(){}
        LinearMoveTask(movable* object, int targetX, int targetY, int speed);
        movable* object;
        int targetX, targetY;
        int speed;
        virtual void update() override;
    };

    struct linearSizeTask : virtual Task{
        linearSizeTask(){}
        linearSizeTask(movable* object, int targetW, int targetH, int speed);
        movable* object;
        int targetW, targetH;
        int speed;
        virtual void update() override;
    };

    struct linearRectTask : virtual Task{
        linearRectTask(movable* object, SDL_Rect targetRect, int speed);
        linearSizeTask sizeTask;
        LinearMoveTask moveTask;
        virtual void update() override;
    };

    struct SmoothMoveTask : virtual Task{
        SmoothMoveTask(){}
        SmoothMoveTask(movable* object, int targetX, int targetY, double speed);
        movable* object;
        int targetX, targetY;
        double speed;
        virtual void update() override;
    };

    struct smoothSizeTask : virtual Task{
        movable* object;
        int targetW, targetH;
        double speed;
        virtual void update() override;
    };

    struct smoothRectTask : virtual Task{
        smoothRectTask(movable* object, SDL_Rect targetRect, double speed);
        smoothSizeTask sizeTask;
        SmoothMoveTask moveTask;
        virtual void update() override;
    };

    class moveManager{
        private:
            std::list<Task*> tasks;
        public:
            moveManager();
            ~moveManager();

            void addTask(Task* task);
            void update();
            void removeFinishedTasks();
            void removeTask(Task* task);
    };
}




