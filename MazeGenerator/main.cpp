/// A C++ implementation of Randomized Prim's algorithm for generating a maze.

#include <iostream>
#include <vector>
#include <array>
#include <cstdlib>
#include <cassert>
#include <algorithm>

/// A simple enum class representing a cell
enum class Cell {
    Wall, Path
};

/**
 *  Structure containing position/coordinate information of our grid.
 */
struct Position {
    int row, col;
    
    inline bool operator==(const Position& pos) { return pos.row == row && pos.col == col; }
};

class Grid {
public:
    Grid(int width, int height);
    
    void generateMaze();
    
    void print();
    
private:
    const std::vector<Position>
    frontierCellsForPositionMatchingCell(const Position pos, const Cell cell);
    
    Cell& cellAtPosition(const Position pos);
    
    bool isPositionInGrid(const Position pos);
    
    const Position 
    positionBetweenFrontierAndCell(const Position cellPos, const Position frontierPos);
    
    std::vector< std::vector<Cell> > _cells;
    
    int _width;
    
    int _height;
};

Grid::Grid(int width, int height) :
_width(width), _height(height)
{
    for (int h = 0; h < height; h++) {
        std::vector<Cell> row;
        for (int w = 0; w < width; w++) {
            row.push_back(Cell::Wall); 
        }
        _cells.push_back(row);
    }
}

/**
 *  Generates a 2D maze using a the Randomized Prims algorithm
 *  https://en.wikipedia.org/wiki/Maze_generation_algorithm
 */
void 
Grid::generateMaze()
{
    std::vector<Position> frontierCells;

    // Pick random starting position
    std::srand(static_cast<unsigned int>(std::time(0)));
    Position startPos = { rand() % _height, rand() % _width };
    cellAtPosition(startPos) = Cell::Path;

    // Calculate the initial set of frontier cells
    auto frontier = frontierCellsForPositionMatchingCell(startPos, Cell::Wall);
    frontierCells.insert(frontierCells.end(), frontier.begin(), frontier.end());
    
    while (!frontierCells.empty()) {
        // Pick one of the frontier cells at random and make it a path
        int randomFrontierIndex = rand() % frontierCells.size();
        auto randomFrontierPos = frontierCells[randomFrontierIndex];
        cellAtPosition(randomFrontierPos) = Cell::Path;

        // Figure out all path neighbours to connect with
        auto neighbours = frontierCellsForPositionMatchingCell(randomFrontierPos, Cell::Path);
        if (!neighbours.empty()) {
            // Pick one and connect the two paths by making the in-between cell a path as well
            auto randomNeighbour = neighbours[rand() % neighbours.size()];
            auto inBetweenPos = positionBetweenFrontierAndCell(randomNeighbour, randomFrontierPos);
            cellAtPosition(inBetweenPos) = Cell::Path;
        }

        // Find the frontier cells for the randomly selected frontier cell
        auto newFrontier = frontierCellsForPositionMatchingCell(randomFrontierPos, Cell::Wall);
        
        // Remove new frontier cells if they can be found in the frontier
        // list so we don't end up with duplicates
        auto cellAlreadyInFrontier = [&](Position p) {
            return std::find(frontierCells.begin(), frontierCells.end(), p) != frontierCells.end();
        };
        
        newFrontier.erase(
            std::remove_if(newFrontier.begin(), newFrontier.end(), cellAlreadyInFrontier),
            newFrontier.end()
        );

        // Add all of the new frontier cells to our list and remove the processed frontier cell
        frontierCells.insert(frontierCells.end(), newFrontier.begin(), newFrontier.end());
        frontierCells.erase(frontierCells.begin() + randomFrontierIndex);
    }
}

/**
 *  A basic console print for displaying the maze.
 */
void
Grid::print()
{
    for (auto row : _cells) {
        for (auto cell : row) {
            switch (cell) {
                case Cell::Wall: {
                    std::cout << "█"; 
                    break;
                }
                case Cell::Path: {
                    std::cout << " ";
                    break;
                }
            }
        }
        std::cout << std::endl;
    }
}

/**
 *  Returns all of the valid frontier cell positions for the given position on the grid. The
 *  cells found are ones that are within the grid and that match the provided cell type.
 *
 *  @param pos  Grid position to get frontier cells for
 *  @param cell Frontier cell type
 *
 *  @return Vector of valid frontier cell positions that match the given cell type.
 */
const std::vector<Position> 
Grid::frontierCellsForPositionMatchingCell(const Position pos, const Cell cell)
{
    std::vector<Position> frontier;
    
    Position leftPos = { pos.row, pos.col - 2 };
    if (isPositionInGrid(leftPos) && cellAtPosition(leftPos) == cell) {
        frontier.push_back(leftPos);
    }
    
    Position rightPos = { pos.row, pos.col + 2 };
    if (isPositionInGrid(rightPos) && cellAtPosition(rightPos) == cell) {
        frontier.push_back(rightPos);
    }
    
    Position topPos = { pos.row - 2, pos.col };
    if (isPositionInGrid(topPos) && cellAtPosition(topPos) == cell) {
        frontier.push_back(topPos);
    }
    
    Position bottomPos = { pos.row + 2, pos.col };
    if (isPositionInGrid(bottomPos) && cellAtPosition(bottomPos) == cell) {
        frontier.push_back(bottomPos);
    }
    
    return frontier;
}

/**
 *  Returns a ref to the cell in the grid at the given position.
 *
 *  @param pos Position in the grid.
 *
 *  @return Reference to cell.
 */
Cell&
Grid::cellAtPosition(Position pos)
{
    assert(isPositionInGrid(pos));
    return _cells[pos.row][pos.col];
}

/**
 *  Checks to see if the given position is within our grid.
 *
 *  @param pos Grid position.
 *
 *  @return true if the position is in our grid, false otherwise.
 */
bool 
Grid::isPositionInGrid(Position pos)
{
    return (pos.row >= 0 && pos.row < _height &&
            pos.col >= 0 && pos.col < _width);
}

/**
 *  Given two positions, a frontier position and a cell position, this method returns
 *  the position of the cell that lies between them. The assumption is that the frontier
 *  and cell are 1 cells apart as per Prims algorithm.
 *
 *  @param frontierPos Frontier position.
 *  @param cellPos     Cell position.
 *
 *  @return The cell position between the frontier and cell.
 */
const Position 
Grid::positionBetweenFrontierAndCell(const Position frontierPos, const Position cellPos)
{
    if (cellPos.row < frontierPos.row) {
        return { cellPos.row + 1, cellPos.col };
    } else if (cellPos.row > frontierPos.row) {
        return { cellPos.row - 1, cellPos.col };
    }
    
    if (cellPos.col < frontierPos.col) {
        return { cellPos.row, cellPos.col + 1 };
    } else if (cellPos.col > frontierPos.col) {
        return { cellPos.row, cellPos.col - 1 };
    }
    
    // If we get here, that means that cellPos == frontierPos
    return cellPos;
}

int main(int argc, char** argv)
{
    Grid grid(30, 20);
    grid.generateMaze();
    grid.print();
    return 0;
}
