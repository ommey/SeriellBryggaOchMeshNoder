#ifndef BRIDGESCENE_H
#define BRIDGESCENE_H

#include <Arduino.h>
#include "Map.h"
#include "Tile.h"
#include <ArduinoJson.h>
#include "MapUpdate.h"
#include "TileUpdate.h"



class BridgeScene {
    private:
        Map map;
        //Comms* comms;
        QueueHandle_t* sceneSerialQueue;
        QueueHandle_t* sceneMeshQueue;
        QueueHandle_t sceneUpdateQueue;
        TaskHandle_t mapHandlerTaskHandle;
        TaskHandle_t tileUpdateTaskHandle;
        static void mapHandlerTask(void* p);
        static void tileUpdateTask(void* p);
        
    public:
        BridgeScene();
        ~BridgeScene();

        void reset();

        void registerSerialQueue(QueueHandle_t* serialQueue);

        void registerMeshQueue(QueueHandle_t* meshQueue);

        void sceneToSerial(const String& msg);

        void sceneToMesh(const String& msg);
        
        void createNewMap(int rows, int columns);

        void enqueueMapUpdate(int row, int column, Tile::TileType type);
        
        void enqueueTileMovement(int oldRow, int oldColumn, int newRow, int newColumn);

        void openTileUpdates();

        void updateTile(int row, int column, Tile::TileType type);

        void internMapUpdate();

        void start();
};

#endif