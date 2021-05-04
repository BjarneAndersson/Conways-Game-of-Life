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
    Cell* m_surrounding_cells[8];

private:
    enum Direction
    {
        N = 0, NE, E, SE, S, SW, W, NW
    };

public:
    Cell() {};
    Cell(int x, int y);

    // prints basic informations about the cell
    void Print() {
        std::cout << m_x << ", " << m_y << " | Alive: " << m_alive << " | Alive next Epoch: " << m_alive_next_epoch << std::endl;
    }

    // sets the surrounding cells as attributes
    /*void setSurroundingCells(Cell *n, Cell *ne, Cell *e, Cell *se, Cell *s, Cell *sw, Cell *w, Cell *nw) {
        //std::cout << n << std::endl;
        m_surrounding_cells[N] = n;
        m_surrounding_cells[NE] = ne;
        m_surrounding_cells[E] = e;
        m_surrounding_cells[SE] = se;
        m_surrounding_cells[S] = s;
        m_surrounding_cells[SW] = sw;
        m_surrounding_cells[W] = w;
        m_surrounding_cells[NW] = nw;
    }*/

    void setSurroundingCellsArr(Cell* surrounding_cells[]) {
        m_surrounding_cells = surrounding_cells;
    }

    // counts how many alive neighbours the cell has
    int howManyAliveNeighbours() {
        int alive_neighbours = 0;
        for (int i = 0; i < 8; i++) {
            Cell *neighbour = m_surrounding_cells[i];
            bool neighbour_alive = neighbour->m_alive;
            std::cout << neighbour->m_x << ", " << neighbour->m_y << " | " << neighbour_alive << std::endl;

            if (neighbour_alive) {
                alive_neighbours += 1;
            }
        }
        return alive_neighbours;
    }

    // calculates, based on the number of alive neighbours, if the cell will be alive next epoch
    void calculateNextEpoch() {
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
    //m_surrounding_cells = new Cell[8];
    //Cell *m_surrounding_cells[8];
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
        // initialization of important variables
        width = ScreenWidth();
        height = ScreenHeight();
        length = width * height;
        
        srand(time(0));

        Cell *grid = new Cell[length];
        //memset(grid, 0, length * sizeof(Cell));


        // filling the grid with cell-objects
        for (int i = 0; i < length; i++) {
            int x = (int)i % width;
            int y = (int)floor(i / width);

            //std::cout << x << ", " << y << std::endl;
            grid[i] = Cell(x, y);
        }

        // initializing border cell - the alive-status of this cell will never be calculated
        Cell border = Cell(-1, -1);
        border.m_alive = false;
        border.m_alive_next_epoch = false;

        // (y * w + x) index a 2d array in 1d

        // set surrounding cells per cell | if next to an edge -> border-cell
        for (int i = 0; i < length; i++) {
            int x = grid[i].m_x;
            int y = grid[i].m_y;

            Cell* n = (y == 0) ? &border : &grid[(y - 1) * width + (x + 0)];
            Cell* ne = (y == 0 || x == (width - 1)) ? &border : &grid[(y - 1) * width + (x + 1)];
            Cell* e = (x == (width - 1)) ? &border : &grid[(y + 0) * width + (x + 1)];
            Cell* se = (y == (height - 1) || x == (width - 1)) ? &border : &grid[(y + 1) * width + (x + 1)];
            Cell* s = (y == (height - 1)) ? &border : &grid[(y + 1) * width + (x + 0)];
            Cell* sw = (x == 0 || y == (height - 1)) ? &border : &grid[(y + 1) * width + (x - 1)];
            Cell* w = (x == 0) ? &border : &grid[(y + 0) * width + (x - 1)];
            Cell* nw = (x == 0 || y == 0) ? &border : &grid[(y - 1) * width + (x - 1)];

            Cell* surrounding_cells[8]{n, ne, e, se, s, sw, w, nw};

            //grid[i].setSurroundingCells(n, ne, e, se, s, sw, w, nw);
            grid[i].setSurroundingCellsArr(surrounding_cells);

            // sets random alive-status
            grid[i].m_alive = (bool)(rand() % 2 == 0);


            grid[i].Print();
        }

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // calculate the alive-status of each cell for the next epoch
        for (int i = 0; i < length; i++) {
            grid[i].calculateNextEpoch();
        }
        std::cout << "\n\n";
        

        // If the current cell is alive, color the pixel black, else white
        for (int i = 0; i < length; i++) {
            Draw(grid[i].m_x, grid[i].m_y, (grid[i].m_alive) ? olc::BLACK : olc::WHITE);;
        }

        // moves the calculation result of the next epoch into the variable alive
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