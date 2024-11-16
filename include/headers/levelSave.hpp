#pragma once

#include <list>
#include <vector>

enum Player{
    Red,
    Blue
};

enum BlockType{
    Undefined,
    Air,
    Wall,
    Spike,
    Star
};

struct Point{
    int x;
    int y;
};

struct DPoint{
    double x;
    double y;
};

struct DRect{
    double x;
    double y;
    double w;
    double h;
};

struct Block{
    BlockType type;
    bool applicableTo[2];
    Point position;
};

struct LevelSave{
    int width;
    int height;
    int starCount;
    std::list<Block> blocks;

    Point start[2];
    Point end[2];
};

enum appl{
    N,
    R,
    B,
    A
};

struct blockSave{
    BlockType type;
    appl applicableTo;
};

struct levelSave2{
    std::vector<std::vector<blockSave>> blocks;
    Point start[2];
    Point end[2];
};