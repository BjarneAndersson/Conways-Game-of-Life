#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <vector>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <chrono>
#include <thread>

// -----------------------------------------------------------------------

#define LOG(x) std::cout << x << std::endl

// -----------------------------------------------------------------------

class Cell
{
public:
    int m_x, m_y;
    bool m_alive, m_alive_next_epoch;

    // m_n, m_ne, m_e, m_se, m_s, m_sw, m_w, m_nw
    std::vector<Cell*> m_surrounding_cells;

private:
    enum Direction
    {
        N = 0, NE, E, SE, S, SW, W, NW
    };

    

public:
    Cell() {};
    Cell(int x, int y);

    void Print() {
        std::cout << m_x << ", " << m_y << " | Alive: " << m_alive << " | Alive next Epoch: " << m_alive_next_epoch << std::endl;
    }

    void setSurroundingCells(Cell* n, Cell* ne, Cell* e, Cell* se, Cell* s, Cell* sw, Cell* w, Cell* nw) { // requiers pointers
        std::cout << n << std::endl;
        m_surrounding_cells.push_back(n);
        m_surrounding_cells.push_back(ne);
        m_surrounding_cells.push_back(e);
        m_surrounding_cells.push_back(se);
        m_surrounding_cells.push_back(s);
        m_surrounding_cells.push_back(sw);
        m_surrounding_cells.push_back(w);
        m_surrounding_cells.push_back(nw);
    }

    int howManyAliveNeighbours() {
        int alive_neighbours = 0;
        for (std::vector<Cell*>::iterator pObj = m_surrounding_cells.begin(); pObj != m_surrounding_cells.end(); ++pObj) {
            //Cell* neighbour = m_surrounding_cells[i];
            bool neighbour_alive = (*pObj)->m_alive;
            //std::cout << neighbour->m_x << ", " << neighbour->m_y << " | " << neighbour_alive << std::endl;

            if (neighbour_alive) {
                alive_neighbours += 1;

            }

        }
        return alive_neighbours;
    }

    void calculateNextEpoch() {
        // How many alive neighbours
        int alive_neighbours = howManyAliveNeighbours();
        std::cout << alive_neighbours << std::endl;

        if (alive_neighbours == 2 || alive_neighbours == 3)
            m_alive_next_epoch = true;
        else
            m_alive_next_epoch = false;
    }
};

Cell::Cell(int x, int y) : m_x(x), m_y(y)
{
    m_alive = false;
    m_alive_next_epoch = false;
    m_surrounding_cells = std::vector<Cell*> ;
}

// -----------------------------------------------------------------------

class Window : public olc::PixelGameEngine
{
public:
    Window()
    {
        sAppName = "Conways Game Of Live";
    }

private:
    int width;
    int height;
    int length;

    Cell* grid;

public:
    bool OnUserCreate() override
    {
        width = ScreenWidth();
        height = ScreenHeight();
        length = width * height;
        
        srand(time(0));

        std::vector<Cell> grid(length);

        //memset(grid, 0, length * sizeof(Cell));


        for (int i = 0; i < length; i++) {
            int x = (int)i % width;
            int y = (int)floor(i / width);

            //std::cout << x << ", " << y << std::endl;
            grid.push_back(Cell(x, y));
        }

        Cell border = Cell(-1, -1);
        border.m_alive = false;
        border.m_alive_next_epoch = false;

        // (y * w + x) index a 2d array in 1d
        for (int i = 0; i < length; i++) {
            int x = grid[i].m_x;
            int y = grid[i].m_y;

            // set surrounding cells
            Cell* n = (y == 0) ? &border : &grid[(y - 1) * width + (x + 0)];
            Cell* ne = (y == 0 || x == (width - 1)) ? &border : &grid[(y - 1) * width + (x + 1)];
            Cell* e = (x == (width - 1)) ? &border : &grid[(y + 0) * width + (x + 1)];
            Cell* se = (y == (height - 1) || x == (width - 1)) ? &border : &grid[(y + 1) * width + (x + 1)];
            Cell* s = (y == (height - 1)) ? &border : &grid[(y + 1) * width + (x + 0)];
            Cell* sw = (x == 0 || y == (height - 1)) ? &border : &grid[(y + 1) * width + (x - 1)];
            Cell* w = (x == 0) ? &border : &grid[(y + 0) * width + (x - 1)];
            Cell* nw = (x == 0 || y == 0) ? &border : &grid[(y - 1) * width + (x - 1)];

            grid[i].setSurroundingCells(n, ne, e, se, s, sw, w, nw);

            // set random alive or not alive
            grid[i].m_alive = (bool)(rand() % 2 == 0);


            grid[i].Print();
        }

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        for (int i = 0; i < length; i++) {
            grid[i].calculateNextEpoch();
            
        }
        std::cout << "\n\n";
        


        for (int i = 0; i < length; i++) {
            Draw(grid[i].m_x, grid[i].m_y, (grid[i].m_alive) ? olc::BLACK : olc::WHITE);;
        }

        for (int i = 0; i < length; i++) {
            grid[i].m_alive = grid[i].m_alive_next_epoch;
        }
        return true;
    }
};

// -----------------------------------------------------------------------

int main()
{
    const int pixelSize = 8;


    // Initialize Window
    Window window;
    if (window.Construct(3, 3, pixelSize, pixelSize))
        window.Start();

    return 0;
}