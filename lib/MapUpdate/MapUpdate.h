#ifndef MAPUPDATE_H
#define MAPUPDATE_H

#include "Tile.h"
#include <Arduino.h>

struct MapUpdate{
    int row;
    int column;
    Tile::TileType type;
    MapUpdate() = default;
    MapUpdate(int row, int column, Tile::TileType type): row(row), column(column), type(type) {}
};

#endif