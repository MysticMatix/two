#include "level.hpp"

#include "game.hpp"
#include "levels.hpp"
#include "textureManager.hpp"

#include <math.h>

#include <iostream>

//#define debug

Level::Level(int x, int y, int width, int height){
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    this->mapWidth = 0;
    this->mapHeight = 0;

    this->blockSize = 0;

    this->playerData[0].dead = false;
    this->playerData[1].dead = false;
    this->playerData[0].won = false;
    this->playerData[1].won = false;

    this->stars = 0;
    this->starCount = 0;

    this->playerData[0].pos = {0, 0};
    this->playerData[1].pos = {0, 0};

    playerTexture = textureManager::loadTexture("assets/player.png");
    starTexture = textureManager::loadTexture("assets/star.png");
    spikeTexture = textureManager::loadTexture("assets/spike.png");
    wallTexture = textureManager::loadTexture("assets/wall.png");
    endTexture = textureManager::loadTexture("assets/end.png");

    //load(levels::getLevel(0));
}

Level::~Level(){
    this->clean();
}

void Level::load(LevelSave level){
    #ifdef debug
    std::cout << "loading level\n";
    #endif

    this->level = level;

    this->mapWidth = level.width;
    this->mapHeight = level.height;

    this->blockSize = this->width / this->mapWidth;

    this->map.clear();

    this->map.resize(this->mapWidth);
    for(int i = 0; i < this->mapWidth; i++){
        this->map[i].resize(this->mapHeight);
        fill(this->map[i].begin(), this->map[i].end(), Block{Air, {true, true}, {0, 0}});
    }

    this->blocks.clear();

    for(Block block : level.blocks){
        this->map[block.position.x][block.position.y] = block;
        this->blocks.push_back(block.position);
    }

    this->playerData[0].start.x = level.start[0].x;
    this->playerData[0].start.y = level.start[0].y;
    this->playerData[1].start.x = level.start[1].x;
    this->playerData[1].start.y = level.start[1].y;

    this->playerData[0].end.x = level.end[0].x;
    this->playerData[0].end.y = level.end[0].y;
    this->playerData[1].end.x = level.end[1].x;
    this->playerData[1].end.y = level.end[1].y;

    this->playerData[0].pos.x = this->playerData[0].start.x;
    this->playerData[0].pos.y = this->playerData[0].start.y;
    this->playerData[1].pos.x = this->playerData[1].start.x;
    this->playerData[1].pos.y = this->playerData[1].start.y;
    this->playerData[0].renderPos = {(double)this->playerData[0].pos.x, (double)this->playerData[0].pos.y};
    this->playerData[1].renderPos = {(double)this->playerData[1].pos.x, (double)this->playerData[1].pos.y};

    this->playerData[0].dead = false;
    this->playerData[1].dead = false;
    this->playerData[0].won = false;
    this->playerData[1].won = false;

    this->starCount = level.starCount;
    this->stars = 0;

    this->blockSize = std::min(this->width / this->mapWidth, this->height / this->mapHeight) * 1.5;
    this->displacement = {0.0, 0.0};

    this->blockSize = std::min(width / (5 + abs(playerData[0].pos.x - playerData[1].pos.x)), height / (5 + abs(playerData[0].pos.y - playerData[1].pos.y))) * 0.8;
    this->displacement = {(double)-(playerData[0].pos.x + playerData[1].pos.x + 1)/2, (double)-(playerData[0].pos.y + playerData[1].pos.y + 1)/2};

    this->animationTime = 0;

    this->moveQueue = std::queue<Direction>();

    this->gameStatus = GameStatus::PLAYING;

    #ifdef debug
    std::cout << "Level loaded, width: " << this->mapWidth << ", height: " << this->mapHeight << std::endl;
    #endif
}

void Level::update(){
    #ifdef debug
    std::cout << "Level::update\n";
    #endif
    animationTime++;

    auto it = this->blocks.begin();
    while(it != this->blocks.end()){
        this->updateBlock(&map[it->x][it->y]);
        it++;
    }

    if(!moveQueue.empty() && !playerStillMoving && this->gameStatus == GameStatus::PLAYING){
        #ifdef debug
        std::cout << "Moving " << moveQueue.front() << std::endl;
        #endif
        movePlayer(0, moveQueue.front());
        movePlayer(1, moveQueue.front());
        moveQueue.pop();
    }

    playerStillMoving = false;
    this->updatePlayer(0);
    this->updatePlayer(1);

    //calculate visble rectangle
    int minReqY, maxReqY, minReqX, maxReqX;
    for(int i = 0; i < 2; i++){
        minReqY = maxReqY = this->playerData[i].pos.y;
        minReqX = maxReqX = this->playerData[i].pos.x;
        while(minReqY > 0 && (!map[playerData[i].pos.x][minReqY - 1].applicableTo[i] || map[playerData[i].pos.x][minReqY - 1].type == Air || map[playerData[i].pos.x][minReqY - 1].type == Star)) minReqY--;
        while(maxReqY < this->mapHeight - 1 && (!map[playerData[i].pos.x][maxReqY + 1].applicableTo[i] || map[playerData[i].pos.x][maxReqY + 1].type == Air || map[playerData[i].pos.x][maxReqY + 1].type == Star)) maxReqY++;
        while(minReqX > 0 && (!map[minReqX - 1][playerData[i].pos.y].applicableTo[i] || map[minReqX - 1][playerData[i].pos.y].type == Air || map[minReqX - 1][playerData[i].pos.y].type == Star)) minReqX--;
        while(maxReqX < this->mapWidth - 1 && (!map[maxReqX + 1][playerData[i].pos.y].applicableTo[i] || map[maxReqX + 1][playerData[i].pos.y].type == Air || map[maxReqX + 1][playerData[i].pos.y].type == Star)) maxReqX++;
        if(i == 0){
            visibleBlocks = {(double)minReqX, (double)minReqY, (double)maxReqX, (double)maxReqY};
        }
        else{
            visibleBlocks.x = std::min(visibleBlocks.x, (double)minReqX);
            visibleBlocks.y = std::min(visibleBlocks.y, (double)minReqY);
            visibleBlocks.w = std::max(visibleBlocks.w, (double)maxReqX);
            visibleBlocks.h = std::max(visibleBlocks.h, (double)maxReqY);
        }
    }

    //calculate the size of the visible rectangle
    visibleBlocks.w = visibleBlocks.w - visibleBlocks.x + 2;
    visibleBlocks.h = visibleBlocks.h - visibleBlocks.y + 2;
    visibleBlocks.x -= 0.5;
    visibleBlocks.y -= 0.5;

    visibleBlocks = {0.0, 0.0, double(mapWidth), double(mapHeight)};

    //std::cout << "visibleBlocks: " << visibleBlocks.x << ", " << visibleBlocks.y << ", " << visibleBlocks.w << ", " << visibleBlocks.h << std::endl;

    //calculate the block size so that the visible rectangle fits in the screen
    this->blockSize = std::min(width / (visibleBlocks.w), height / (visibleBlocks.h));

    //calculate the displacement so that the visible rectangle is centered

    this->displacement.x = -visibleBlocks.x + (width / (double)this->blockSize - visibleBlocks.w) / 2;
    this->displacement.y = -visibleBlocks.y + (height / (double)this->blockSize - visibleBlocks.h) / 2;

    this->combinedDisplacement = {int(this->displacement.x * this->blockSize), int(this->displacement.y * this->blockSize)};

    if(this->playerData[0].dead || this->playerData[1].dead){
        this->gameStatus = GameStatus::LOST;
    }
    else if(this->playerData[0].won && this->playerData[1].won && this->stars == this->starCount){
        this->gameStatus = GameStatus::WON;
    }
    else{
        this->gameStatus = GameStatus::PLAYING;
    }

    #ifdef debug
    std::cout << "Level::update end\n";
    #endif
}

void Level::render(){
    renderEnd(0);
    renderEnd(1);

    auto it = this->blocks.begin();
    //std::cout << blocks.size() << std::endl;
    while(it != this->blocks.end()){
        this->renderBlock(&map[it->x][it->y]);
        it++;
    }

    renderPlayer(0);
    renderPlayer(1);
}

bool Level::handleEvents(SDL_Event event){
    switch(event.type){
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
                case SDLK_w: case SDLK_UP:
                    moveQueue.push(Direction::UP);
                    return true;
                case SDLK_s: case SDLK_DOWN:
                    moveQueue.push(Direction::DOWN);
                    return true;
                case SDLK_a: case SDLK_LEFT:
                    moveQueue.push(Direction::LEFT);
                    return true;
                case SDLK_d: case SDLK_RIGHT:
                    moveQueue.push(Direction::RIGHT);
                    return true;
                case SDLK_r:
                    restart();
                    return true;
            }
            break;
    }

    return false;
}

void Level::clean(){
    this->blocks.clear();
    SDL_DestroyTexture(this->playerTexture);
    SDL_DestroyTexture(this->starTexture);
}

void Level::renderBlock(Block* block){
    SDL_Rect rect = {this->combinedDisplacement.x + block->position.x * this->blockSize, this->combinedDisplacement.y + block->position.y * this->blockSize, this->blockSize, this->blockSize};
    SDL_Rect srcRect = {
        0,
        (block->applicableTo[0] ? 64 : 0) + (block->applicableTo[1] ? 128 : 0),
        64,
        64
    };
    switch(block->type){
        case Wall:
            SDL_RenderCopy(Game::renderer, this->wallTexture, &srcRect, &rect);
            break;
        case Star:
            //std::cout << "rendering star: " << block->position.x << ", " << block->position.y << ", " << block->applicableTo[0] << ", " << block->applicableTo[1] << std::endl;
            //std::cout << "srcRect: " << srcRect.x << ", " << srcRect.y << ", " << srcRect.w << ", " << srcRect.h << std::endl;
            if(animationTime % 60 < 30) rect.y += 4;
            SDL_RenderCopy(Game::renderer, this->starTexture, &srcRect, &rect);
            break;
        case Spike:
            //render rotated spike
            SDL_RenderCopyEx(Game::renderer, this->spikeTexture, &srcRect, &rect, animationTime % 360, NULL, SDL_FLIP_NONE);
            break;
    }
}

void Level::updateBlock(Block* block){
    switch(block->type){
        case Wall:
            break;
        case Star:
            break;
            //std::cout << "star: " << block->position.x << ", " << block->position.y << ", " << block->applicableTo[0] << ", " << block->applicableTo[1] << std::endl;
    }
}

void Level::renderPlayer(int player){
    SDL_Rect srcRect = {0, 0, 64, 64};
    SDL_Rect rect = {(int)(this->combinedDisplacement.x + this->playerData[player].renderPos.x * this->blockSize), (int)(this->combinedDisplacement.y + this->playerData[player].renderPos.y * this->blockSize), this->blockSize, this->blockSize};
    if(player) srcRect.y = 64;
    if(this->playerData[player].won) srcRect.x = 64;
    SDL_RenderCopy(Game::renderer, this->playerTexture, &srcRect, &rect);
}

void Level::renderEnd(int player){
    SDL_Rect srcRect = {0, player * 64, 64, 64};
    SDL_Rect rect = {(int)(this->combinedDisplacement.x + this->playerData[player].end.x * this->blockSize), (int)(this->combinedDisplacement.y + this->playerData[player].end.y * this->blockSize), this->blockSize, this->blockSize};
    SDL_RenderCopy(Game::renderer, this->endTexture, &srcRect, &rect);
}

static double eps = 0.03;

void Level::updatePlayer(int player){
    Point nextPos = this->playerData[player].pos;

    this->playerData[player].renderPos.x = (double)nextPos.x + (this->playerData[player].renderPos.x - (double)nextPos.x) * 0.8;
    this->playerData[player].renderPos.y = (double)nextPos.y + (this->playerData[player].renderPos.y - (double)nextPos.y) * 0.8;

    if(!(abs(this->playerData[player].renderPos.x - (double)nextPos.x) < eps && abs(this->playerData[player].renderPos.y - (double)nextPos.y) < eps)){
        playerStillMoving = true;
    }

    if(this->playerData[player].dead){
        return;
    }

    if(this->playerData[player].pos.x < 0 || this->playerData[player].pos.x >= this->mapWidth || this->playerData[player].pos.y < 0 || this->playerData[player].pos.y >= this->mapHeight){
        this->playerData[player].dead = true;
        return;
    }

    if(this->playerData[player].pos.x == this->playerData[player].end.x && this->playerData[player].pos.y == this->playerData[player].end.y){
        this->playerData[player].won = true;
    }
    else this->playerData[player].won = false;


}

void Level::movePlayer(int player, Direction direction){
    if(this->playerData[player].dead){
        return;
    }

    Point newPosition = this->playerData[player].pos;
    switch(direction){
        case Direction::UP:
            newPosition.y--;
            break;
        case Direction::DOWN:
            newPosition.y++;
            break;
        case Direction::LEFT:
            newPosition.x--;
            break;
        case Direction::RIGHT:
            newPosition.x++;
            break;
    }

    while(newPosition.x >= 0 && newPosition.x < this->mapWidth && newPosition.y >= 0 && newPosition.y < this->mapHeight){
        Block* block = &this->map[newPosition.x][newPosition.y];
        switch(block->type){
            case Wall:
                if(!block->applicableTo[player]){
                    break;
                }
                return;
            case Spike:
                if(!block->applicableTo[player]){
                    break;
                }
                this->playerData[player].dead = true;
                this->playerData[player].won = false;
                this->playerData[player].pos = newPosition;
                return;
            case Star:
                if(!block->applicableTo[player]){
                    break;;
                }
                this->stars++;
                block->applicableTo[0] = block->applicableTo[1] = false;
        }

        this->playerData[player].pos = newPosition;

        switch(direction){
            case Direction::UP:
                newPosition.y--;
                break;
            case Direction::DOWN:
                newPosition.y++;
                break;
            case Direction::LEFT:
                newPosition.x--;
                break;
            case Direction::RIGHT:
                newPosition.x++;
                break;
        }
    }   
}

void Level::restart(){
    int oldBlocksize = this->blockSize;
    DPoint oldDisplacement = this->displacement;

    this->load(this->level);

    this->blockSize = oldBlocksize;
    this->displacement = oldDisplacement;
}