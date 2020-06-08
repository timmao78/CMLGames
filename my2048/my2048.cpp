#include <ncurses.h>
#include <string>
#include <iostream>
#include "board.h"

int main()
{
    Board bd;

    if (bd.success())
        bd.loop();

    return 0;
}