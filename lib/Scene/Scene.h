#ifndef SCENE_H
#define SCENE_H

#include <Arduino.h>
#include "Map.h"
#include "Tile.h"
#include <ArduinoJson.h>
#include "MapUpdate.h"



class Scene {
    private:
        Map* map;
        //Comms* comms;
        QueueHandle_t* sceneSerialQueue;
        QueueHandle_t sceneUpdateQueue;
        TaskHandle_t mapHandlerTaskHandle;
        TaskHandle_t tileUpdateTaskHandle;
        static void mapHandlerTask(void* p);
        static void tileUpdateTask(void* p);
        
    public:
        Scene();
        ~Scene();

        void reset();

        //void registerComms(Comms* comms);
        void registerSerialQueue(QueueHandle_t* serialQueue);

        void sceneToComms(const String& msg);
        
        void createNewMap(int rows, int columns);

        void enqueueMapUpdate(int row, int column, Tile::TileType type);

        void openTileUpdates();

        void updateTile(int row, int column, Tile::TileType type);

        void start();
};

#endif