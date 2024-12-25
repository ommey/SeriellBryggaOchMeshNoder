#ifndef TILEUPDATE_H
#define TILEUPDATE_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct TileUpdate
    {
        String Command = "";
        int Row = 0;
        int Column = 0;
        int NewX = 0;
        int NewY = 0;
        String Type ="";
        TileUpdate(int x, int y, int newX, int newY, String type, String command) : Row(y), Column(x),  NewX(newX), NewY(newY), Type(type), Command(command){}
        TileUpdate(int x, int y, String type, String command) : Row(y), Column(x), Type(type), Command(command){}
        String ToJson()
        {
            StaticJsonDocument<256> doc;
            doc["Command"] = Command;
            doc["Row"] = Row;
            doc["Column"] = Column;
            doc["Type"] = Type;
            doc["NewRow"] = NewX;
            doc["NewColumn"] = NewY;
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