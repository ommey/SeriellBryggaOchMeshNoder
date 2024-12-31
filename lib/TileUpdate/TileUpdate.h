#ifndef TILEUPDATE_H
#define TILEUPDATE_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct TileUpdate {
    String Command;
    int Row{0};
    int Column{0};
    int OldRow{-1};     // Default to -1 to indicate "not applicable"
    int OldColumn{-1};  // Default to -1 to indicate "not applicable"
    String Type;

    // Default constructor
    TileUpdate() 
        : Command(""), Row(0), Column(0), OldRow(-1), OldColumn(-1), Type("") {}

    // Constructor for creating a new tile update
    TileUpdate(int row, int column, const String& type) 
        : Command("Tile"), Row(row), Column(column), OldRow(-1), OldColumn(-1), Type(type) {}

    // Constructor for moving a tile
    TileUpdate(int oldRow, int oldColumn, int row, int column, const String& type) 
        : Command("MoveTile"), Row(row), Column(column), OldRow(oldRow), OldColumn(oldColumn), Type(type) {}

    // Serialize to JSON
    String ToJson() const {
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

#endif // TILEUPDATE_H
