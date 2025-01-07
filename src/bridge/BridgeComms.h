#ifndef BRIDGECOMMS_H
#define BRIDGECOMMS_H



#include <Arduino.h>
#include "namedMesh.h"
#include <ArduinoJson.h>
#include "BridgeScene.h"
#include "TileUpdate.h"

#define   MESH_SSID       "OmarsSexigaNätverk"
#define   MESH_PASSWORD   "Porr2Porr"
#define   MESH_PORT       5555

class BridgeComms {
private:
    String nodeName = "Bridge";
    BridgeScene* scene;
    namedMesh mesh;

static void meshUpdate(void* pvParameters);

static void meshBroadCastTask(void* pvParameters);

static void serialWriteTask(void* pvParameters);

static void serialReadTask(void* pvParameters);

public:
    QueueHandle_t serialOutPutQueue;
    QueueHandle_t meshOutputQueue;
    std::vector<uint32_t> ActiveNodes;


    enum commandsToReceive{
        NewMap,
        MoveTile,
        Tile,
        Reset,
        Go
    };

    commandsToReceive stringToCommand(const String& command);

    QueueHandle_t getSerialOutPutQueue();

    BridgeComms(BridgeScene* scene);

    ~BridgeComms();

    void start();

    void enqueueMeshOutput(const String& msg);

    void enqueueSerialOutput(const String& msg);

};



#endif