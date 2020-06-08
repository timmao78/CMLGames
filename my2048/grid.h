#pragma once

#include <ncurses.h>
#include <string>

class Grid
{
public:
    Grid()
    {
        m_IndexX = 0;
        m_IndexY = 0;
        m_Num = 0;
    }

    Grid(int indexY, int indexX, int num)
        : m_IndexY(indexY), m_IndexX(indexX), m_Num(num)
    {
    }

    void setIndex(int y, int x)
    {
        m_IndexY = y;
        m_IndexX = x;
    }

    void setNum(int num)
    {
        m_Num = num;
    }

    int getNum() const
    {
        return m_Num;
    }

    void printGrid(WINDOW *win) const
    {

        std::string disp;

        if (m_Num != 0)
        {
            disp = std::to_string(m_Num);
            int len = disp.length();
            int r = (7 - len) / 2;
            int l = 7 - len - r;
            for (int i = 0; i < r; i++)
                disp += " ";
            for (int i = 0; i < l; i++)
                disp = " " + disp;
        }
        else
            disp = "       ";

        mvwprintw(win, m_IndexY * 4 + 2, m_IndexX * 8 + 1, disp.c_str());
        // wrefresh(win);
    }

private:
    int m_IndexY, m_IndexX;
    int m_Num;
};