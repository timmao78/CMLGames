#pragma once

#include "grid.h"
#include <vector>
#include <fstream>

#define DEBUG2048 0

class Board
{
public:
    Board();

    void outputLog(std::string s);

    void startGame();

    bool success();

    void printGameOver();

    void printOpeningScreen();
    void unPrintOpeningScreen();

    void unPrintGameOver();

    void printGrids();

    bool cantMerge();

    bool new2();

    void printScore(int score);

    // void printUsrInput(std::string s);

    void printStatus(std::string s);

    void reportMove(std::string s);

    void reportMerge(std::string s);

    void analyzeMerge(std::string s);

    ~Board();

    void loop();

    void mergeUp(int j);

    void mergeDown(int j);

    void moveRight(int i);

    void moveLeft(int i);

    void moveDown(int j);

    void moveUp(int j);

    void mergeRight(int i);

    void mergeLeft(int i);

    bool merge(int i1, int j1, int i2, int j2);

private:
    WINDOW *playWin;
    WINDOW *infoWin;
    WINDOW *statsWin;

    Grid grids[4][4];

    bool initSuccess; // indicating if the current terminal is big enough to play this game
    int usrInput;
    int score;
    bool moved;
    std::ofstream output;
    std::string lastOP;
};
