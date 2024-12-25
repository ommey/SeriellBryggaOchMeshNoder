#ifndef FIREFIGHTER_H
#define FIREFIGHTER_H

#include <Arduino.h>
#include "Map.h"
#include <random>
#include "TileUpdate.h"

class FireFighter{

private:
int x;
int y;
Map* map;

QueueHandle_t* serialOutputQueue;
QueueHandle_t* meshOutPutQueue;
TaskHandle_t fireFighterTaskHandle;


static void FireFighterTask(void *pvParameters);

void enqueueSerialOutput(const String& msg);
void enqueueMeshOutput(const String& msg);

public:
FireFighter();
~FireFighter();
void registerSerialOutput(QueueHandle_t* serialOutputQueue);
void registerMeshOutput(QueueHandle_t* meshOutPutQueue);
void registerMap(Map* map);

void start();
void reset();

void spawnFireFighter();
void moveFireFighter(int newX, int newY);
void moveFireFighter();

void updateTile(int x, int y, Tile::TileType type);
void moveTile(int x, int y, int newX, int newY, Tile::TileType type);



};






#endif