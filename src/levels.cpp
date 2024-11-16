#include "levels.hpp"

#include <iostream>
#include <vector>

#include "levelGen.hpp"

/* levelSave format
    * 
    * width
    * height
    * 
    * start[0].x
    * start[0].y
    * start[1].x
    * start[1].y
    * 
    * end[0].x
    * end[0].y
    * end[1].x
    * end[1].y
    * 
    * starCount
    * 
    * blocks
    * 
    * block.type
    * block.applicableTo[0] (Red)
    * block.applicableTo[1] (Blue)
    * block.position.x
    * block.position.y
*/

constexpr blockSave EMPTY = {BlockType::Air, N};
constexpr blockSave WALL= {BlockType::Wall, A};
constexpr blockSave SPIKE = {BlockType::Spike, A};
constexpr blockSave STAR = {BlockType::Star, A};

constexpr blockSave AIR = EMPTY;
constexpr blockSave WALL_r = {BlockType::Wall, R};
constexpr blockSave WALL_b = {BlockType::Wall, B};
constexpr blockSave SPIKE_r = {BlockType::Spike, R};
constexpr blockSave SPIKE_b = {BlockType::Spike, B};
constexpr blockSave STAR_r = {BlockType::Star, R};
constexpr blockSave STAR_b = {BlockType::Star, B};

std::vector<levelSave2> levels2 = {
    {
        {
            {WALL, WALL, WALL, WALL, WALL},
            {WALL, AIR , STAR, AIR , WALL},
            {WALL, WALL, WALL, WALL, WALL},
            {WALL, AIR , STAR, AIR , WALL},
            {WALL, WALL, WALL, WALL, WALL},
        },
        {{1, 1}, {1, 3}},
        {{3, 1}, {3, 3}}
    },
    {
        {
            {WALL, WALL, WALL, WALL, WALL, WALL, WALL},
            {WALL, WALL, STAR, WALL, STAR, WALL, WALL},
            {WALL, AIR , AIR , WALL, AIR , AIR , WALL},
            {WALL, WALL, WALL, WALL, WALL, WALL, WALL},
        },
        {{1, 2}, {5, 2}},
        {{2, 2}, {4, 2}}
    },
    {
        {
            {WALL, WALL, WALL, WALL, WALL, WALL, WALL},
            {WALL, WALL, STAR, WALL, WALL, STAR, WALL},
            {WALL, AIR , AIR , WALL_r, STAR, AIR, WALL},
            {WALL, WALL, WALL, WALL, WALL, WALL, WALL},
        },
        {{1, 2}, {2, 2}},
        {{2, 1}, {5, 1}}
    },

};

LevelSave loadFromSave2(levelSave2 save){
    LevelSave levelSave;
    levelSave.starCount = 0;

    levelSave.width = save.blocks[0].size();
    levelSave.height = save.blocks.size();

    levelSave.blocks.clear();

    levelSave.start[0] = save.start[0];
    levelSave.start[1] = save.start[1];

    levelSave.end[0] = save.end[0];
    levelSave.end[1] = save.end[1];

    for(int y = 0; y < levelSave.height; y++){
        for(int x = 0; x < levelSave.width; x++){
            switch(save.blocks[y][x].type){
                case BlockType::Air:
                    break;
                case BlockType::Wall:
                    levelSave.blocks.push_back({Wall, {save.blocks[y][x].applicableTo == R || save.blocks[y][x].applicableTo == A, save.blocks[y][x].applicableTo == B || save.blocks[y][x].applicableTo == A}, {x, y}});
                    break;
                case BlockType::Spike:
                    levelSave.blocks.push_back({Spike, {save.blocks[y][x].applicableTo == R || save.blocks[y][x].applicableTo == A, save.blocks[y][x].applicableTo == B || save.blocks[y][x].applicableTo == A}, {x, y}});
                    break;
                case BlockType::Star:
                    levelSave.blocks.push_back({Star, {save.blocks[y][x].applicableTo == R || save.blocks[y][x].applicableTo == A, save.blocks[y][x].applicableTo == B || save.blocks[y][x].applicableTo == A}, {x, y}});
                    levelSave.starCount++;
                    break;
            }
        }
    }

    return levelSave;
}

static Block wall(int x, int y){
    return {Wall, {true, true}, {x, y}};
}

static Block star(int x, int y){
    return {Star, {true, true}, {x, y}};
}

static std::vector<std::string> tlevels[] = {
    {
        "####!#",
        "#0 *b#",
        "#1*rz#",
        "##!!##",
        "#!##!#"
    },
    {
        "###!###",
        "#@ * z#",
        "###!###"
    },
    {
        "#####",
        "#@*z#",
        "#####"
    },
    {
        "######",
        "#@* z#",
        "######"
    },
};

LevelSave loadFromTexts(std::vector<std::string> save){
    LevelSave levelSave;
    levelSave.starCount = 0;

    levelSave.width = save[0].length();
    levelSave.height = save.size();

    levelSave.blocks.clear();

    for(int y = 0; y < levelSave.height; y++){
        for(int x = 0; x < levelSave.width; x++){
            switch(save[y][x]){
                case '#':
                    levelSave.blocks.push_back(wall(x, y));
                    break;
                case 'r':
                    levelSave.blocks.push_back({Wall, {true, false}, {x, y}});
                    break;
                case 'b':
                    levelSave.blocks.push_back({Wall, {false, true}, {x, y}});
                    break;
                case '!':
                    levelSave.blocks.push_back({Spike, {true, true}, {x, y}});
                    break;
                case '@':
                    levelSave.start[0] = {x, y};
                    levelSave.start[1] = {x, y};
                    break;
                case '0':
                    levelSave.start[0] = {x, y};
                    break;
                case '1':
                    levelSave.start[1] = {x, y};
                    break;
                case 'x':
                    levelSave.end[0] = {x, y};
                    break;
                case 'y':
                    levelSave.end[1] = {x, y};
                    break;
                case 'z':
                    levelSave.end[0] = {x, y};
                    levelSave.end[1] = {x, y};
                    break;
                case '*':
                    levelSave.blocks.push_back(star(x, y));
                    levelSave.starCount++;
                    break;
            }
        }
    }
    return levelSave;
}

namespace levels{
    const int levelCount = 2;
    
    LevelSave levelSave;

    

    LevelSave getLevel(int level){
        if(level >= levelCount){
            level = 0;
        }
        return loadFromSave2(levelGen::getRandomLevel(10, 10));
    }
}