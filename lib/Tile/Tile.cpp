#include "Tile.h"

Tile::TileType Tile::stringToType(const String &type)

    {
        {
        if (type == "Path"){
            return Tile::Path;
        }
        else if (type == "Wall"){
            return Tile::Wall;
        }
        else if (type == "Smokey"){
            return Tile::Smokey;
        }
        else if (type == "Fire"){
            return Tile::Fire;
        }
        else if (type == "HasVictim"){
            return Tile::HasVictim;
        }
        else if (type == "HasHazard"){
            return Tile::HasHazard;
        }
        else if (type == "FireFighter"){
            return Tile::FireFighter;
        }
    }
    }