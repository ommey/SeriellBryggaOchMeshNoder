#include "Map.h"
#include <stdexcept>


Map::Map() : Rows(0), Columns(0), tiles(), fireSpreadMap()
{
}

void Map::createMap(int rows, int columns)
{    
    Rows = rows;
    Columns = columns;
    tiles = std::vector<std::vector<Tile>>(Rows, std::vector<Tile>(Columns));
    fireSpreadMap = std::vector<std::vector<int>>(Rows, std::vector<int>(Columns, 0));

    for (int row = 0; row < Rows; ++row) {
        for (int col = 0; col < Columns; ++col) {
            tiles[row][col] = Tile(row, col, Tile::TileType::Path);
        }
    }
}

void Map::updateTile(int row, int column, Tile::TileType type)
{
    // Validate indices
    if (row < 0 || row >= Rows || column < 0 || column >= Columns) {
        throw std::out_of_range("Invalid row or column index");
    }
    for (Tile tile: getAdjacentTiles(row, column))
    {
        fireSpreadMap[tile.Row][tile.Column] = 0;
    }
    // Update the tile type
    tiles[row][column].type = type;
}

void Map::incrementFireSpread()
{
for (int row = 0; row < Rows; ++row) {
    for (int col = 0; col < Columns; ++col) {
        // Check if the tile type is FIRE
        if (tiles[row][col].type == Tile::TileType::Fire) {
            ++fireSpreadMap[row][col]; 
        }
    }
}

}

std::vector<Tile> Map::getAdjacentTiles(int row, int col) const {
    std::vector<Tile>adjacentTiles;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int newRow = row + i;
            int newCol = col + j;
            if (newRow >= 0 && newRow < Rows && newCol >= 0 && newCol < Columns) {
                adjacentTiles.push_back({tiles[newRow][newCol]});
            }
        }
    }
    return adjacentTiles;    
}
