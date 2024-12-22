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

    String Tile::typeToString(const TileType type)
    {
        switch (type)
        {
        case Path:
            return "Path";
            break;
        case Wall:
            return "Wall";
            break;
        case Smokey:
            return "Smokey";
            break;
        case Fire:
            return "Fire";
            break;
        case HasVictim:
            return "HasVictim";
            break;
        case HasHazard:
            return "HasHazard";
            break;
        case FireFighter:
            return "FireFighter";
            break;
        default:
            return "Unknown";
            break;
        }
    }
