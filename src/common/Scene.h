#ifndef SCENE_H
#define SCENE_H

#include <Arduino.h>
#include "Map.h"
#include "Tile.h"
#include <ArduinoJson.h>
#include "MapUpdate.h"
#include "TileUpdate.h"



class Scene {
    private:
        //Comms* comms;
        QueueHandle_t* sceneSerialQueue;
        QueueHandle_t sceneUpdateQueue;
        TaskHandle_t mapHandlerTaskHandle;
        TaskHandle_t tileUpdateTaskHandle;
        static void mapHandlerTask(void* p);
        static void tileUpdateTask(void* p);
        
    public:
        Map map;
        Scene();
        ~Scene();

        void reset();

        void registerSerialQueue(QueueHandle_t* serialQueue);

        void sceneToSerial(const String& msg);
        
        void createNewMap(int rows, int columns);

        void enqueueMapUpdate(int row, int column, Tile::TileType type);

        void openTileUpdates();

        void updateTile(int row, int column, Tile::TileType type);

        void internMapUpdate();

        void start();
};

#endif