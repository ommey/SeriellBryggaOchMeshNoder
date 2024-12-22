#ifndef COMMS_H
#define COMMS_H



#include <Arduino.h>
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include "Scene.h"

#define   MESH_SSID       "meshNetwork"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555

class Comms {
private:
    Scene* scene;
    painlessMesh mesh;
    QueueHandle_t meshOutputQueue;

static void meshUpdate(void* pvParameters);

static void meshBroadCastTask(void* pvParameters);

static void serialWriteTask(void* pvParameters);

static void serialReadTask(void* pvParameters);
public:
    QueueHandle_t serialOutPutQueue;

    enum commandsToReceive{
        NewMap,
        Tile,
        Reset,
        Go,
    };

    commandsToReceive stringToCommand(const String& command);

    QueueHandle_t getSerialOutPutQueue();

    Comms(Scene* scene);

    ~Comms();

    void start();

    void meshOutPut(const String& msg);

    void serialOutPut(const String& msg);
};



#endif