#include <Arduino.h>
#include "painlessMesh.h"
#include <ArduinoJson.h>


#define   MESH_SSID       "meshNetwork"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555

painlessMesh mesh; //variant på painlessMesh som kan skicka meddelanden till specifika noder

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

};

struct MapUpdate{
    MapUpdate(int row, int column, Tile::TileType type): row(row), column(column), type(type) {}
    int row;
    int column;
    Tile::TileType type;
    Tile::TileType stringToCommand(const String& type){
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


/*    MapUpdate MapUpdateFromJson(const String &json){
        StaticJsonDocument<256> mapUpdateDoc;
        DeserializationError error = deserializeJson(mapUpdateDoc, json);  
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            MapUpdate invalid;
            invalid.row = -1;
            invalid.column = -1;
            invalid.type = Tile::Path;
            return invalid;
        }
        else{
            MapUpdate mapUpdate;
            mapUpdate.row = mapUpdateDoc["Row"];
            mapUpdate.column = mapUpdateDoc["Column"];
            mapUpdate.type = stringToCommand(mapUpdateDoc["Type"]);
            return mapUpdate;
        }
    }*/
};




class Comms {
private:
    painlessMesh mesh;
    QueueHandle_t serialOutPutQueue;
    QueueHandle_t meshOutputQueue;

    static void meshUpdate(void* pvParameters) {
        Comms* comms = static_cast<Comms*>(pvParameters);
        while (1) {
            comms->mesh.update();
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
    }

    static void meshBroadCastTask(void* pvParameters) {
        Comms* comms = static_cast<Comms*>(pvParameters);
        char msgChar[256];
        while (1) {
            if (xQueueReceive(comms->meshOutputQueue, &msgChar, 10) == pdPASS) {
                if (!comms->mesh.sendBroadcast(msgChar)) {
                    Serial.println("Failed to send broadcast");
                }
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }

    static void serialWriteTask(void* pvParameters) {
        Comms* comms = static_cast<Comms*>(pvParameters);
        char msgChar[256];
        while (1) {
            if (xQueueReceive(comms->serialOutPutQueue, &msgChar, 10) == pdPASS) {
                Serial.println(msgChar);
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }

    static void serialReadTask(void* pvParameters) {
        Comms* comms = static_cast<Comms*>(pvParameters);
        int amntjson = 0;
        while (1) {
            if (Serial.available() > 0) {
                String msg = Serial.readStringUntil('\n');
                if (msg.startsWith("{") && msg.endsWith("}"))
                {
                    StaticJsonDocument<256> doc;
                    DeserializationError error = deserializeJson(doc, msg);
                    if (error) {
                        comms->serialOutPut("Failed to parse JSON");
                    }
                    else{
                        comms->serialOutPut("Parsed JSON, " + String(amntjson));
                        amntjson++;
                        switch (stringToCommand(doc.["Command"]))
                        {
                        case NewMap:
                            MapUpdate mapUpdate 
                            
                            break;
                        
                        default:
                            break;
                        }

                    }
                } else if (msg == "reset") {
                    amntjson = 0;
                }
                else {
                    comms->serialOutPut(msg);
                }


                //comms->serialOutPut(msg);
                //comms->meshOutPut(msg);
            }
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
    }

public:
    enum commandsToReceive{
        NewMap,
        Tile,
        Reset,
    };

    commandsToReceive stringToCommand(const String& command){
        if (command == "NewMap"){
            return NewMap;
        }
        else if (command == "Tile"){
            return Tile;
        }
        else if (command == "Reset"){
            return Reset;
        }
    }


    Comms() : serialOutPutQueue(xQueueCreate(100, sizeof(char) * 256)), meshOutputQueue(xQueueCreate(100, sizeof(char) * 256)) {
        Serial.begin(115200);
        Serial.setTimeout(50);
        mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
        mesh.onReceive([this](uint32_t from, String& msg) {
            this->serialOutPut(msg);
        });
    }

    ~Comms() {
        vQueueDelete(serialOutPutQueue);
        vQueueDelete(meshOutputQueue);
    }

    void start() {
        if (xTaskCreate(meshUpdate, "meshUpdate", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create meshUpdate task");
        }
        if (xTaskCreate(serialWriteTask, "serialWriteTask", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create serialWriteTask");
        }
        if (xTaskCreate(serialReadTask, "serialReadTask", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create serialReadTask");
        }
        if (xTaskCreate(meshBroadCastTask, "meshBroadCastTask", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create meshBroadCastTask");
        }
    }

    void meshOutPut(const String& msg) {
        if (msg != "") {
            char msgChar[256];
            msg.toCharArray(msgChar, sizeof(msgChar));
            if (xQueueSend(meshOutputQueue, &msgChar, 10) != pdPASS) {
                Serial.println("Failed to add to mesh queue");
            }
        }
    }

    void serialOutPut(const String& msg) {
        if (msg != "") {
            char msgChar[256];
            msg.toCharArray(msgChar, sizeof(msgChar));
            if (xQueueSend(serialOutPutQueue, &msgChar, 10) != pdPASS) {
                Serial.println("Failed to add to serial queue");
            }
        }
    }
};





class Map {
private:
    int Rows;
    int Columns;
    Tile** tiles; // Use a pointer to a pointer for dynamic 2D array


public:
    Map(int rows, int columns) : Rows(rows), Columns(columns) {
        tiles = new Tile*[Rows];
        for (int i = 0; i < Rows; ++i) {
            tiles[i] = new Tile[Columns];
        }
    }
    void updateTile(int row, int column, Tile::TileType type) {
        tiles[row][column].type = type;
    }

    ~Map() {
        for (int i = 0; i < Rows; ++i) {
            delete[] tiles[i];
        }
        delete[] tiles;
    }
};

class Scene {
    private:
        Map* map;
        Comms* comms;
        QueueHandle_t sceneUpdateQueue;
        static void InternSceneUpdateTask(void* pvParameters)
        {
            Scene* scene = static_cast<Scene*>(pvParameters);
            while (1)
            {
                scene->comms->serialOutPut("InternSceneUpdateTask");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            
        }
        static void ExternSceneUpdateTask(void* pvParameters)
        {
            Scene* scene = static_cast<Scene*>(pvParameters);
            MapUpdate mapUpdate;
            while(1)
            {
                if (xQueueReceive(scene->sceneUpdateQueue, &mapUpdate, 10) == pdPASS)
                {
                    scene->map->updateTile(mapUpdate.row, mapUpdate.column, mapUpdate.type);
                }
            }
        
        }
    public:
        Scene(Map* map, Comms* comms): map(map), comms(comms), sceneUpdateQueue(xQueueCreate(100, sizeof(MapUpdate)))
        {
            
        }
        void start()
        {
            if (xTaskCreate(ExternSceneUpdateTask, "ExternSceneUpdateTask", 5000, this, 1, NULL) != pdPASS) {
                Serial.println("Failed to create ExternSceneUpdateTask");
            }
        }
        void enqueueMapUpdate(int row, int column, Tile::TileType type)
        {
            MapUpdate mapUpdate = {row, column, type};
            if (xQueueSend(sceneUpdateQueue, &mapUpdate, 10) != pdPASS) {
                Serial.println("Failed to add to map queue");
            }
        }
        ~Scene()
        {
            vQueueDelete(sceneUpdateQueue);
        }
};





// Main application
void setup() {
    delay(1000);
    static Comms comms;
    comms.start();
    comms.serialOutPut("Just started");
    comms.meshOutPut("Just started");

}

void loop() {
    // Nothing here, tasks handle everything
}
