#ifndef TILE_H
#define TILE_H

#include <Arduino.h>

class Tile{
    public: 
        int Row;
        int Column;
        enum TileType {
            Path,
            Wall,
            Smokey,
            Fire,
            HasVictim,
            HasHazard,
            FireFighter,
        };

        TileType type;
        
        Tile(int row, int column, TileType type) : Row(row), Column(column), type(type) {}
        Tile(): type(Path) {}
        static TileType stringToType(const String &type);
        static String typeToString(const TileType type);
};

#endif