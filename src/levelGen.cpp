#include "levelGen.hpp"

int levelGen::getRandom(int min, int max){
    std::uniform_int_distribution<> dis(min, max);
    int res = dis(gen);
    return res;
}

void levelGen::init(){
    gen = std::mt19937(rd());
    gen.seed(time(NULL));
}

void levelGen::moveRandomLen(int player, int dirX, int dirY){
    int Maxlen = getRandom(1, max(width, height)/2);
    int x, y;
    for(int i = 0; i < Maxlen; i++){
        x = playerX + dirX;
        y = playerY + dirY;
        if(x <= 0 || x >= width-1 || y <= 0 || y >= height-1){
            return;
        }
        if(visited[player][y][x] == BLOCKED){
            return;
        }
        visited[player][y][x] = VISITED;
        playerX = x;
        playerY = y;

        while(visited[player][y][x] == VISITED){
            playerX = x;
            playerY = y;
            x = playerX + dirX;
            y = playerY + dirY;
        }
    }

    if(visited[player][y][x] == NOT_VISITED){
        visited[player][y][x] = BLOCKED;
    }
}

void levelGen::fillMap(int player){
    playerX = start[player].first;
    playerY = start[player].second;
    for(auto move : moveList){
        moveRandomLen(player, move.first, move.second);
    }
    _end[player] = {playerX, playerY};
}

void levelGen::fillMoves(){
    moveList.clear();
    int moves = getRandom(max(height, width)/2, 3 * max(width, height));
    while(moves--){
        int dir = getRandom(0, 3);
        switch(dir){
            case 0:
                moveList.push_back({1, 0});
                break;
            case 1:
                moveList.push_back({-1, 0});
                break;
            case 2:
                moveList.push_back({0, 1});
                break;
            case 3:
                moveList.push_back({0, -1});
                break;
        }
    }
}

levelSave2 levelGen::getRandomLevel(int w, int h){
    width = w;
    height = h;
    visited[0].resize(height);
    visited[1].resize(height);
    joined.resize(height);
    for(int i = 0; i < height; i++){
        visited[0][i].resize(width);
        visited[1][i].resize(width);
        joined[i].resize(height);
        fill(visited[0][i].begin(), visited[0][i].end(), NOT_VISITED);
        fill(visited[1][i].begin(), visited[1][i].end(), NOT_VISITED);
        fill(joined[i].begin(), joined[i].end(), NOT_VISITED);
        visited[0][i][0] = BLOCKED;
        visited[1][i][0] = BLOCKED;
        visited[0][i][width-1] = BLOCKED;
        visited[1][i][width-1] = BLOCKED;
    }

    for(int i = 0; i < width; i++){
        visited[0][0][i] = BLOCKED;
        visited[1][0][i] = BLOCKED;
        visited[0][height-1][i] = BLOCKED;
        visited[1][height-1][i] = BLOCKED;
    }

    int x = getRandom(1, width-2);
    int y = getRandom(1, height-2);
    visited[0][y][x] = VISITED;
    start[0] = {x, y};
    
    x = getRandom(1, width-2);
    y = getRandom(1, height-2);
    visited[1][y][x] = VISITED;
    start[1] = {x, y};

    fillMoves();
    fillMap(0);
    fillMap(1);

    //print map for player 1
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(visited[0][i][j] == VISITED){
                cout << ".";
            }else if(visited[0][i][j] == BLOCKED){
                cout << "#";
            }else{
                cout << " ";
            }
        }
        cout << endl;
    }

    cout << endl;

    //print map for player 2
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(visited[1][i][j] == VISITED){
                cout << ".";
            }else if(visited[1][i][j] == BLOCKED){
                cout << "#";
            }else{
                cout << " ";
            }
        }
        cout << endl;
    }

    cout << endl;

    //join the maps;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(visited[0][i][j] == visited[1][i][j]) joined[i][j] = visited[0][i][j];
            else if(visited[0][i][j] == BLOCKED) joined[i][j] = (visited[1][i][j] == NOT_VISITED ? BLOCKED : RED_BLOCKED);
            else if(visited[1][i][j] == BLOCKED) joined[i][j] = (visited[0][i][j] == NOT_VISITED ? BLOCKED : BLUE_BLOCKED);
            else if(visited[0][i][j] == VISITED) joined[i][j] = VISITED;
            else if(visited[1][i][j] == VISITED) joined[i][j] = VISITED;
            else joined[i][j] = NOT_VISITED;
        }
    }

    //print joined map
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(joined[i][j] == VISITED){
                cout << ".";
            }else if(joined[i][j] == BLOCKED){
                cout << "#";
            }else if(joined[i][j] == RED_BLOCKED){
                cout << "r";
            }else if(joined[i][j] == BLUE_BLOCKED){
                cout << "b";
            }else{
                cout << " ";
            }
        }
        cout << endl;
    }

    levelSave2 save;

    save.start[0] = {start[0].first, start[0].second};
    save.start[1] = {start[1].first, start[1].second};

    save.end[0] = {_end[0].first, _end[0].second};
    save.end[1] = {_end[1].first, _end[1].second};

    save.blocks.resize(height);
    for(int i = 0; i < height; i++){
        save.blocks[i].resize(width);
        for(int j = 0; j < width; j++){
            switch(joined[i][j]){
                case VISITED:
                    if(getRandom(0, 99) < 30) save.blocks[i][j] = {Star, A};
                    else save.blocks[i][j] = {Air, N};
                    break;
                case BLOCKED:
                    save.blocks[i][j] = {Wall, A};
                    break;
                case RED_BLOCKED:
                    save.blocks[i][j] = {Wall, R};
                    break;
                case BLUE_BLOCKED:
                    save.blocks[i][j] = {Wall, B};
                    break;
                case NOT_VISITED:
                    if(getRandom(0, 99) < 20) save.blocks[i][j] = {Spike, A};
                    else save.blocks[i][j] = {Air, N};
                    break;
            }
        }
    }

    if(save.blocks[start[0].second][start[0].first].type == Star) save.blocks[start[0].second][start[0].first] = {Air, A};
    if(save.blocks[start[1].second][start[1].first].type == Star) save.blocks[start[1].second][start[1].first] = {Air, A};

    std::cout << "generated level\n";
    std::cout << "solution:";
    for(auto move : moveList){
        if(move.first == 1 && move.second == 0) std::cout << " R";
        if(move.first == -1 && move.second == 0) std::cout << " L";
        if(move.first == 0 && move.second == 1) std::cout << " D";
        if(move.first == 0 && move.second == -1) std::cout << " U";
    }
    std::cout << std::endl;

    return save;
}