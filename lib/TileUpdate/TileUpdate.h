#ifndef TILEUPDATE_H
#define TILEUPDATE_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct TileUpdate
    {
        String Command = "Tile";
        int Row;
        int Column;
        String Type;
        TileUpdate(int row, int column, String type) : Row(row), Column(column), Type(type) {}
        String ToJson()
        {
            StaticJsonDocument<256> doc;
            doc["Command"] = Command;
            doc["Row"] = Row;
            doc["Column"] = Column;
            doc["Type"] = Type;
            String json;
            serializeJson(doc, json);
            return json;
        }
    };
struct moveTileUpdate
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
};


#endif