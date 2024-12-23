#ifndef MAP_H
#define MAP_H

#include <vector>
#include "Tile.h"

class Map {
private:

public:
    int Rows;
    int Columns;
    std::vector<std::vector<Tile>> tiles;          // 2D grid of Tiles
    std::vector<std::vector<int>> fireSpreadMap;   // 2D grid for fire spread

    Map(int rows, int columns);

    ~Map() = default; // Default destructor as vectors manage memory automatically

    void updateTile(int row, int column, Tile::TileType type);
    void incrementFireSpread();
    std::vector<Tile> getAdjacentTiles(int row, int col) const;
};

#endif
