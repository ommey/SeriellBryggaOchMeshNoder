#include "FireFighter.h"

FireFighter::FireFighter() : x(0), y(0), fireFighterTaskHandle(nullptr)
{
}

FireFighter::~FireFighter()
{
    if (fireFighterTaskHandle != NULL)
    {
        vTaskDelete(fireFighterTaskHandle);
        fireFighterTaskHandle = NULL;
    }
}

void FireFighter::registerSerialOutput(QueueHandle_t *serialOutputQueue)
{
    this->serialOutputQueue = serialOutputQueue;
}



void FireFighter::registerMeshOutput(QueueHandle_t *meshOutPutQueue)
{
    this->meshOutPutQueue = meshOutPutQueue;
}

void FireFighter::registerMap(Map* map)
{
    this->map = map;
}

void FireFighter::start()
{
    if(fireFighterTaskHandle == NULL)
    {
        if(xTaskCreate(FireFighterTask, "FireFighterTask", 2048, this, 1, &fireFighterTaskHandle) != pdPASS)
        {
            Serial.println("Failed to create FireFighterTask");
        }
    }
    else
    {
        Serial.println("Task already running");
    }
}

void FireFighter::reset()
{
    if (fireFighterTaskHandle != NULL)
    {
        vTaskDelete(fireFighterTaskHandle);
        fireFighterTaskHandle = NULL;
    }
}

void FireFighter::spawnFireFighter()
{
    std::vector<Tile> validSpawnLocations = map->getValidSpawnLocations();
    if (!validSpawnLocations.empty()) {
        randomSeed(millis());  
        int randomIndex = random(0, validSpawnLocations.size());
        Tile randomTile = validSpawnLocations[randomIndex];
        this->x = randomTile.Row;
        this->y = randomTile.Column;
        updateTile(randomTile.Row, randomTile.Column, Tile::TileType::FireFighter);
    }
}

void FireFighter::moveFireFighter()
{
    std::vector<Tile> validMoveLocations = map->getAdjacentTiles(x, y);
    if (!validMoveLocations.empty()) {
        randomSeed(millis());  
        int randomIndex = random(0, validMoveLocations.size());
        Tile randomTile = validMoveLocations[randomIndex];
        moveTile(x, y, randomTile.Row, randomTile.Column, Tile::TileType::FireFighter);
        x = randomTile.Row;
        y = randomTile.Column;
    }
    
}

void FireFighter::moveFireFighter(int newX, int newY)
{
    TileUpdate moveTileUpdate(x, y, newX, newY, Tile::typeToString(Tile::TileType::FireFighter), "MoveTile");
    enqueueMeshOutput(moveTileUpdate.ToJson());
}

void FireFighter::updateTile(int row, int column, Tile::TileType type)
{
    TileUpdate tileUpdate(row, column, Tile::typeToString(type), "Tile");
    enqueueMeshOutput(tileUpdate.ToJson());
}

void FireFighter::moveTile(int x, int y, int newX, int newY, Tile::TileType type)
{
    TileUpdate moveTileUpdate(x, y, newX, newY, Tile::typeToString(type), "MoveTile");
    enqueueMeshOutput(moveTileUpdate.ToJson());
}

void FireFighter::FireFighterTask(void *pvParameters)
{
    FireFighter* fireFighter = static_cast<FireFighter*>(pvParameters);
    while (fireFighter->map->isCreated())
    {
        fireFighter->spawnFireFighter();
        //fireFighter->enqueueSerialOutput("Im working with map: " + String(fireFighter->map->Rows) + ", " + String(fireFighter->map->Columns));
        while (1)
        {
            fireFighter->enqueueSerialOutput("FireFighterTask going strong" + String(millis()));
            fireFighter->moveFireFighter();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            for (int i = 0; i < fireFighter->map->Rows; i++)
            {
                fireFighter->enqueueSerialOutput(fireFighter->map->getRowCharRepresentation(i));
            }
            
            
        }
                
         
                
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void FireFighter::enqueueSerialOutput(const String &msg)
{
    if (msg != "") 
    {
        char msgChar[256];
        msg.toCharArray(msgChar, sizeof(msgChar));
        if (xQueueSend(*serialOutputQueue, &msgChar, 10) != pdPASS) 
        {
            Serial.println("Failed to add to serial queue");
        }
    }
}

void FireFighter::enqueueMeshOutput(const String &msg)
{
    if (msg != "") 
    {
        char msgChar[256];
        msg.toCharArray(msgChar, sizeof(msgChar));
        if (xQueueSend(*meshOutPutQueue, &msgChar, 10) != pdPASS) 
        {
            Serial.println("Failed to add to mesh queue");
        }
    }
}
