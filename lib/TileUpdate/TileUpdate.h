#ifndef TILEUPDATE_H
#define TILEUPDATE_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct TileUpdate
    {
        String Command;
        int Row;
        int Column;
        int OldRow;
        int OldColumn;
        String Type;
        TileUpdate() = default;
        TileUpdate(int row, int column, String type) : Row(row), Column(column), Type(type) { OldRow = -1; OldColumn = -1; Command = "Tile";}
        TileUpdate(int oldRow, int oldColumn, int row, int column) : Row(row), Column(column) { Command = "MoveTile";}
        String ToJson()
        {
            StaticJsonDocument<256> doc;
            doc["Command"] = Command;
            doc["OldRow"] = OldRow;
            doc["OldColumn"] = OldColumn;
            doc["Row"] = Row;
            doc["Column"] = Column;
            doc["Type"] = Type;
            String json;
            serializeJson(doc, json);
            return json;
        }
    };
/*struct moveTileUpdate
{
    String Command = "MoveTile";
    int X;
    int Y;
    int NewX;
    int NewY;
    String Type;
    moveTileUpdate(int x, int y, int newX, int newY, String type) : X(x), Y(y), NewX(newX), NewY(newY), Type(type) {}
    String ToJson()
    {
        StaticJsonDocument<256> doc;
        doc["Command"] = Command;
        doc["Row"] = X;
        doc["Column"] = Y;
        doc["NewRow"] = NewX;
        doc["NewColumn"] = NewY;
        doc["Type"] = Type;
        String json;
        serializeJson(doc, json);
        return json;
    }
};*/


#endif