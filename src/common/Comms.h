#ifndef COMMS_H
#define COMMS_H



#include <Arduino.h>
#include "namedMesh.h"
#include <ArduinoJson.h>
#include "Scene.h"
#include "FireFighter.h"

#define   MESH_SSID       "OmarsSexigaNätverk"
#define   MESH_PASSWORD   "Porr2Porr"
#define   MESH_PORT       5555

class Comms {
private:
    String nodeName = "Bridge";
    Scene* scene;
    FireFighter* fireFighter;
    namedMesh mesh;

static void meshUpdate(void* pvParameters);

static void meshBroadCastTask(void* pvParameters);

static void serialWriteTask(void* pvParameters);

static void serialReadTask(void* pvParameters);
public:
    QueueHandle_t serialOutPutQueue;
    QueueHandle_t meshOutputQueue;

    enum commandsToReceive{
        NewMap,
        Tile,
        Reset,
        Go,
    };

    commandsToReceive stringToCommand(const String& command);

    QueueHandle_t getSerialOutPutQueue();

    Comms(Scene* scene, FireFighter* fireFighter);

    ~Comms();

    void start();

    void enqueueMeshOutput(const String& msg);

    void enqueueSerialOutput(const String& msg);
};



#endif