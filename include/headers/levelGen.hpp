#pragma once

#include <iostream>
#include <vector>
#include <random>

#include "levelSave.hpp"

using namespace std;

enum visitedType{
    NOT_VISITED,
    VISITED,
    BLOCKED,
    RED_BLOCKED,
    BLUE_BLOCKED
};

class levelGen{
    inline static int width, height;

    
    inline static pair<int, int> start[2];
    inline static pair<int, int> _end[2];
    
    inline static vector<vector<visitedType>> visited[2];
    inline static vector<vector<visitedType>> joined;
    
    inline static std::random_device rd;
    inline static std::mt19937 gen;
    
    inline static vector<pair<int, int>> moveList;
    
    static void init();
    
    static int getRandom(int min, int max);
    
    inline static int playerX, playerY;
    
    static void moveRandomLen(int player, int dirX, int dirY);
    
    static void fillMap(int player);
    
    static void fillMoves();
    
    public:
    
        static levelSave2 getRandomLevel(int w, int h);
};