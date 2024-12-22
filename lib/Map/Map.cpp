#include "Map.h"

Map::Map(int rows, int columns) : Rows(rows), Columns(columns) {
        tiles = new Tile*[Rows];
        for (int i = 0; i < Rows; ++i) {
            tiles[i] = new Tile[Columns];
        }
}

Map::~Map()
 {
    for (int i = 0; i < Rows; ++i) {
    delete[] tiles[i];
    }
    delete[] tiles;
}

void Map::updateTile(int row, int column, Tile::TileType type)
{
    tiles[row][column].type = type;
}
