#include "board.h"

#include "grid.h"
#include <vector>
#include <iostream>

Board::Board()
{
    /* NCURSES START */
    initscr();
    noecho();
    cbreak();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    if (yMax < 25 || xMax < 55)
    {
        endwin();
        std::cout << "Terminal Window Not Big Enough!" << std::endl;
        initSuccess = false;
    }
    else
    {
        int yStart = (yMax - 22) / 2;
        int xStart = (xMax - 51) / 2;

        // Create a window for the game
        playWin = newwin(17, 33, yStart, xStart);
        refresh();
        box(playWin, 0, 0);
        wrefresh(playWin);

        infoWin = newwin(17, 17, yStart, xStart + 34);
#if DEBUG2048 == 1
        box(infoWin, 0, 0);
#endif
        refresh();
        wrefresh(infoWin);

        statsWin = newwin(6, 51, yStart + 17, xStart);
#if DEBUG2048 == 1
        box(statsWin, 0, 0);
#endif
        refresh();
        wrefresh(statsWin);

        initSuccess = true;
        output.open("moves.log");

        printOpeningScreen();
    }
}

void Board::outputLog(std::string s)
{
    output << "  |  " << std::endl;
    output << "  V  " << s << std::endl;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            output << std::setw(7) << grids[i][j].getNum() << " ";
        }
        output << std::endl;
    }
}

void Board::unPrintOpeningScreen()
{
    for (int i = 1; i < 16; i++)
        for (int j = 1; j < 32; j++)
            mvwprintw(playWin, i, j, " ");
}

void Board::printOpeningScreen()
{
    mvwprintw(playWin, 5, 2, " #####  #####  #   #   ##### ");
    mvwprintw(playWin, 6, 2, "     #  #   #  #   #   #   # ");
    mvwprintw(playWin, 7, 2, " #####  #   #  #####   ##### ");
    mvwprintw(playWin, 8, 2, " #      #   #      #   #   # ");
    mvwprintw(playWin, 9, 2, " #####  #####      #   ##### ");

    mvwprintw(playWin, 12, 6, "Press Enter to Start");

    while (true)
    {
        usrInput = wgetch(playWin);
        if (usrInput == 10)
        {
            unPrintOpeningScreen();
            startGame();
            break;
        }
    }
}

void Board::startGame()
{
    // Initialize board
    for (int i = 0; i <= 16; i += 1)
        for (int j = 0; j <= 32; j += 1)
        {
            if (i % 4 == 0 && j % 8 == 0)
                mvwprintw(playWin, i, j, "*");
            else if ((i != 0 && i != 16 && i % 4 == 0) && j % 8 != 0)
                mvwprintw(playWin, i, j, "-");
            else if (i % 4 != 0 && j % 8 == 0 && j != 0 && j != 32)
                mvwprintw(playWin, i, j, "|");
        }

    // Initialize grids
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            grids[i][j].setNum(0);
            grids[i][j].setIndex(i, j);
            grids[i][j].printGrid(playWin);
        }

    wrefresh(playWin);

    // initialize variables
    usrInput = 0;
    score = 0;
    moved = true;
    printScore(score);
    lastOP = "";

    new2();
}

bool Board::success() { return initSuccess; }

void Board::printGrids()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            grids[i][j].printGrid(playWin);
        }
}

bool Board::cantMerge()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            if (i + 1 < 4)
                if (grids[i][j].getNum() == grids[i + 1][j].getNum())
                {
#if DEBUG2048 == 1
                    analyzeMerge(std::to_string(i) + "," + std::to_string(j) + "=" + std::to_string(i + 1) + "," + std::to_string(j));
#endif
                    return false;
                }
            if (i - 1 > 0)
                if (grids[i][j].getNum() == grids[i - 1][j].getNum())
                {
#if DEBUG2048 == 1
                    analyzeMerge(std::to_string(i) + "," + std::to_string(j) + "=" + std::to_string(i - 1) + "," + std::to_string(j));
#endif
                    return false;
                }
            if (j + 1 < 4)
                if (grids[i][j].getNum() == grids[i][j + 1].getNum())
                {
#if DEBUG2048 == 1
                    analyzeMerge(std::to_string(i) + "," + std::to_string(j) + "=" + std::to_string(i) + "," + std::to_string(j + 1));
#endif
                    return false;
                }
            if (j - 1 > 0)
                if (grids[i][j].getNum() == grids[i][j - 1].getNum())
                {
#if DEBUG2048 == 1
                    analyzeMerge(std::to_string(i) + "," + std::to_string(j) + "=" + std::to_string(i) + "," + std::to_string(j - 1));
#endif
                    return false;
                }
        }

#if DEBUG2048 == 1
    analyzeMerge("CANT MERGE");
#endif
    return true;
}

std::string Board::printGameOver()
{
    mvwprintw(playWin, 7, 6, "                     ");
    mvwprintw(playWin, 8, 6, "                     ");
    mvwprintw(playWin, 9, 6, "                     ");
    mvwprintw(playWin, 10, 6, "                     ");
    mvwprintw(playWin, 11, 6, "                     ");

    mvwprintw(playWin, 7, 12, "GAME OVER");
    std::string choices[2] = {" Restart ",
                              "  Quit   "};
    int choice;
    int highlight = 0;

    while (true)
    {
        for (int i = 0; i < 2; i++)
        {
            if (i == highlight)
                wattron(playWin, A_REVERSE);
            mvwprintw(playWin, i + 9, 12, choices[i].c_str());
            wattroff(playWin, A_REVERSE);
        }

        wmove(playWin, 0, 0);
        choice = wgetch(playWin);

        switch (choice)
        {
        case KEY_UP:
            highlight--;
            if (highlight == -1)
                highlight = 0;
            break;
        case KEY_DOWN:
            highlight++;
            if (highlight == 2)
                highlight = 1;
            break;
        default:
            break;
        }
        if (choice == 10)
            break;
    }

    wrefresh(playWin);

    return choices[highlight];
}

void Board::unPrintGameOver()
{
    mvwprintw(playWin, 7, 6, "                     ");
    mvwprintw(playWin, 8, 6, "                     ");
    mvwprintw(playWin, 9, 6, "                     ");
    mvwprintw(playWin, 10, 6, "                     ");
    mvwprintw(playWin, 11, 6, "                     ");
    wrefresh(playWin);
}

bool Board::new2()
{
    std::vector<int> v1;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (grids[i][j].getNum() == 0)
                v1.push_back(i * 4 + j);

    if (v1.size() == 0 && cantMerge())
    {
        std::string s = printGameOver();
        if (s == "  Quit   ")
            return false;
        else if (s == " Restart ")
        {
            unPrintGameOver();
            startGame();
            return true;
        }
    }

    if (moved)
    {
        outputLog(lastOP);
        int r = rand();
        int j = v1[r % v1.size()] % 4;
        int i = (v1[r % v1.size()] - j) / 4;
        grids[i][j].setNum(2);
        grids[i][j].printGrid(playWin);
        wrefresh(playWin);
#if DEBUG2048 == 1
        printStatus(std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(v1.size()));
#endif
    }

    moved = false;
    lastOP = "";
    outputLog("NEW CELL");
    return true;
}

void Board::printScore(int score)
{
    mvwprintw(infoWin, 1, 1, ("Score:            "));
    mvwprintw(infoWin, 1, 1, ("Score: " + std::to_string(score)).c_str());
    wrefresh(infoWin);
}

// void Board::printUsrInput(std::string s)
// {
//     mvwprintw(infoWin, 15, 1, "        ");
//     mvwprintw(infoWin, 15, 1, s.c_str());
//     wrefresh(infoWin);
// }

#if DEBUG2048 == 1

void Board::printStatus(std::string s)
{
    mvwprintw(statsWin, 1, 1, "                         ");
    mvwprintw(statsWin, 1, 1, s.c_str());
    wrefresh(statsWin);
}

void Board::reportMove(std::string s)
{
    mvwprintw(statsWin, 2, 1, "                         ");
    mvwprintw(statsWin, 2, 1, s.c_str());
    wrefresh(statsWin);
}

void Board::reportMerge(std::string s)
{
    mvwprintw(statsWin, 3, 1, "                         ");
    mvwprintw(statsWin, 3, 1, s.c_str());
    wrefresh(statsWin);
}

void Board::analyzeMerge(std::string s)
{
    mvwprintw(statsWin, 4, 1, "                         ");
    mvwprintw(statsWin, 4, 1, s.c_str());
    wrefresh(statsWin);
}

#endif

Board::~Board()
{
    /* NCURSES END */
    endwin();
    output.close();
}

void Board::loop()
{
    keypad(playWin, true);

    while (true)
    {
        usrInput = wgetch(playWin);
        if (usrInput == 'q')
            break;
        else if (usrInput == KEY_UP)
        {
            lastOP = "UP";
#if DEBUG2048 == 1
            reportMerge("");
            reportMove("");
#endif
            // printUsrInput("UP");
            moveUp(0);
            moveUp(1);
            moveUp(2);
            moveUp(3);
            mergeUp(0);
            mergeUp(1);
            mergeUp(2);
            mergeUp(3);
            moveUp(0);
            moveUp(1);
            moveUp(2);
            moveUp(3);
            printGrids();
            printScore(score);
        }
        else if (usrInput == KEY_DOWN)
        {
            lastOP = "DOWN";
#if DEBUG2048 == 1
            reportMerge("");
            reportMove("");
#endif
            // printUsrInput("DOWN");
            moveDown(0);
            moveDown(1);
            moveDown(2);
            moveDown(3);
            mergeDown(0);
            mergeDown(1);
            mergeDown(2);
            mergeDown(3);
            moveDown(0);
            moveDown(1);
            moveDown(2);
            moveDown(3);
            printGrids();
            printScore(score);
        }
        else if (usrInput == KEY_LEFT)
        {
            lastOP = "LEFT";
#if DEBUG2048 == 1
            reportMerge("");
            reportMove("");
#endif
            // printUsrInput("LEFT");
            moveLeft(0);
            moveLeft(1);
            moveLeft(2);
            moveLeft(3);
            mergeLeft(0);
            mergeLeft(1);
            mergeLeft(2);
            mergeLeft(3);
            moveLeft(0);
            moveLeft(1);
            moveLeft(2);
            moveLeft(3);
            printGrids();
            printScore(score);
        }
        else if (usrInput == KEY_RIGHT)
        {
            lastOP = "RIGHT";
#if DEBUG2048 == 1
            reportMerge("");
            reportMove("");
#endif
            // printUsrInput("RIGHT");
            moveRight(0);
            moveRight(1);
            moveRight(2);
            moveRight(3);
            mergeRight(0);
            mergeRight(1);
            mergeRight(2);
            mergeRight(3);
            moveRight(0);
            moveRight(1);
            moveRight(2);
            moveRight(3);
            printGrids();
            printScore(score);
        }
        if (!new2())
            break;
    }
}
void Board::mergeUp(int j)
{
    if (merge(0, j, 1, j))
        merge(2, j, 3, j);
    else if (!merge(1, j, 2, j))
        merge(2, j, 3, j);
}

void Board::mergeDown(int j)
{
    if (merge(3, j, 2, j))
        merge(1, j, 0, j);
    else if (!merge(2, j, 1, j))
        merge(1, j, 0, j);
}

void Board::moveRight(int i)
{
    for (int j1 = 3; j1 > 0; j1--)
        if (grids[i][j1].getNum() == 0)
        {
            for (int j2 = j1 - 1; j2 >= 0; j2--)
                if (grids[i][j2].getNum() != 0)
                {
                    grids[i][j1].setNum(grids[i][j2].getNum());
                    grids[i][j2].setNum(0);
                    moved = true;
                    lastOP += " MOVE";

#if DEBUG2048 == 1
                    reportMove("MOVE RIGHT");
#endif
                    break;
                }
        }
}
void Board::moveLeft(int i)
{
    for (int j1 = 0; j1 < 3; j1++)
        if (grids[i][j1].getNum() == 0)
        {
            for (int j2 = j1 + 1; j2 <= 3; j2++)
                if (grids[i][j2].getNum() != 0)
                {
                    grids[i][j1].setNum(grids[i][j2].getNum());
                    grids[i][j2].setNum(0);
                    moved = true;
                    lastOP += " MOVE";
#if DEBUG2048 == 1
                    reportMove("MOVE LEFT");
#endif
                    break;
                }
        }
}

void Board::moveDown(int j)
{
    for (int i1 = 3; i1 > 0; i1--)
        if (grids[i1][j].getNum() == 0)
        {
            for (int i2 = i1 - 1; i2 >= 0; i2--)
                if (grids[i2][j].getNum() != 0)
                {
                    grids[i1][j].setNum(grids[i2][j].getNum());
                    grids[i2][j].setNum(0);
                    moved = true;
                    lastOP += " MOVE";

#if DEBUG2048 == 1
                    reportMove("MOVE DOWN");
#endif
                    break;
                }
        }
}

void Board::moveUp(int j)
{
    for (int i1 = 0; i1 < 3; i1++)
        if (grids[i1][j].getNum() == 0)
        {
            for (int i2 = i1 + 1; i2 <= 3; i2++)
                if (grids[i2][j].getNum() != 0)
                {
                    grids[i1][j].setNum(grids[i2][j].getNum());
                    grids[i2][j].setNum(0);
                    moved = true;
                    lastOP += " MOVE";

#if DEBUG2048 == 1
                    reportMove("MOVE UP");
#endif
                    break;
                }
        }
}

void Board::mergeRight(int i)
{
    if (merge(i, 3, i, 2))
        merge(i, 1, i, 0);
    else if (!merge(i, 2, i, 1))
        merge(i, 1, i, 0);
}

void Board::mergeLeft(int i)
{
    if (merge(i, 0, i, 1))
        merge(i, 2, i, 3);
    else if (!merge(i, 1, i, 2))
        merge(i, 2, i, 3);
}

bool Board::merge(int i1, int j1, int i2, int j2)
{
    if (grids[i1][j1].getNum() == grids[i2][j2].getNum() && grids[i1][j1].getNum() != 0)
    {
        score += grids[i2][j2].getNum();
        grids[i1][j1].setNum(grids[i2][j2].getNum() * 2);
        grids[i2][j2].setNum(0);
        moved = true;
        lastOP += " MERGE";
#if DEBUG2048 == 1
        reportMerge("MERGED");
#endif
        return true;
    }
    else
    {
        return false;
    }
}
