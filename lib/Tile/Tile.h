#ifndef TILE_H
#define TILE_H

#include <Arduino.h>

class Tile{
    public: 
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
        
        Tile(): type(Path) {}
        static TileType stringToType(const String &type);
        static String typeToString(const TileType type);
};

#endif