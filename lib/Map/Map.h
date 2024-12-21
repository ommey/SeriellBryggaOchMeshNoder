#ifndef MAP_H
#define MAP_H

#include <Arduino.h>
#include "Tile.h"



class Map {
private:


public:
    Tile** tiles; // Use a pointer to a pointer for dynamic 2D array
    int Rows;
    int Columns;
    Map(int rows, int columns);

    ~Map();

    void updateTile(int row, int column, Tile::TileType type);
};

#endif