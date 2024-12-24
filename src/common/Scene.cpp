#include "Scene.h"

Scene::Scene() : map(nullptr), sceneSerialQueue(nullptr), /*comms(nullptr),*/ sceneUpdateQueue(xQueueCreate(1024, sizeof(MapUpdate))){}

struct TileUpdate
    {
        String Command = "Tile";
        int Row;
        int Column;
        String Type;
        TileUpdate(int row, int column, String type) : Row(row), Column(column), Type(type) {}
        String ToJson()
        {
            StaticJsonDocument<256> doc;
            doc["Command"] = Command;
            doc["Row"] = Row;
            doc["Column"] = Column;
            doc["Type"] = Type;
            String json;
            serializeJson(doc, json);
            return json;
        }
    };

void Scene::registerSerialQueue(QueueHandle_t* serialQueue)
{
    this->sceneSerialQueue = serialQueue;
}

void Scene::sceneToSerial(const String &msg)
{
    if (sceneSerialQueue)
    {
        char msgChar[256];
        msg.toCharArray(msgChar, sizeof(msgChar));
        if(xQueueSend(*sceneSerialQueue, &msgChar, 10)== pdPASS)
        {
        }
        else
        {
            Serial.println("Failed to send to serial queue");
        }
    }
    else
    {
    Serial.println("No Queue to send to");
    }
}

void Scene::createNewMap(int rows, int columns)
{
    if (map != nullptr)
    {
        delete map;
    }
    map = new Map(rows, columns);
    sceneToSerial("Created new map" + String(rows) + "x" + String(columns));
}

void Scene::enqueueMapUpdate(int row, int column, Tile::TileType type)
{
    if (!map)
            {
                sceneToSerial("No map to update");
                return;
            }
            MapUpdate mapUpdate = {row, column, type};
            if (xQueueSend(sceneUpdateQueue, &mapUpdate, 10) != pdPASS) {
                Serial.println("Failed to add to map queue");
            }
}



void Scene::tileUpdateTask(void *p)
{
    Scene* scene = static_cast<Scene*>(p);
    MapUpdate mapUpdate;

    while (scene->map)
    {

        if (xQueueReceive(scene->sceneUpdateQueue, &mapUpdate, 0) == pdPASS) // Non-blocking
        {
            if (scene->map)
            {
                scene->map->updateTile(mapUpdate.row, mapUpdate.column, mapUpdate.type);
                

                //scene->sceneToSerial("Server updated tile (" + String(mapUpdate.row) + ", " + String(mapUpdate.column) + ") to "+ Tile::typeToString(mapUpdate.type) +" from gui");
                       
            }
            else
            {
                Serial.println("No map to update");
                //scene->sceneToSerial("No map to update");
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS); // Delay for periodic processing
    }
}

void Scene::mapHandlerTask(void *p)
{
    Scene* scene = static_cast<Scene*>(p);

    while (scene->map)
    {
        // Perform periodic handling logic
        if (scene->map)
        {              
         scene->internMapUpdate(); 
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for periodic processing
        }

    }

    void Scene::updateTile(int row, int column, Tile::TileType type)
    {
    //för servern
    TileUpdate tileUpdate(row, column, Tile::typeToString(type));
    sceneToSerial(tileUpdate.ToJson());
    //internt i servern
    map->updateTile(row, column, type);
    //för klienten
    }

    void Scene::internMapUpdate()
    {
        map->incrementFireSpread();
          for(int r = 0; r < map->Rows; r++)
            {
                for(int c = 0; c < map->Columns; c++)
                {
                    switch (map->tiles[r][c].type)
                    {
                    case Tile::TileType::Path:
                        
                        break;
                    case Tile::TileType::Wall:
                        
                        break;
                    case Tile::TileType::Smokey:
                        for (Tile tile : map->getAdjacentTiles(r, c))
                            {
                                if (tile.type == Tile::TileType::Fire)
                                {
                                updateTile(r, c, Tile::TileType::Fire);
                                break;
                                }  
                        }
                        break;
                    case Tile::TileType::Fire:
                        if (map->fireSpreadMap[r][c] > 5)
                        {
                            map->fireSpreadMap[r][c] = 0;
                            for (Tile tile : map->getAdjacentTiles(r, c))
                            {
                                if (tile.type != Tile::TileType::Fire && tile.type != Tile::TileType::Wall)
                                {
                                updateTile(tile.Row, tile.Column, Tile::TileType::Fire);
                                break;
                                }
                                
                            }
                        }
                        
                        /*for(Tile tile : map->getAdjacentTiles(r, c))
                        {
                            if (map->fireSpreadMap[r][c] > 5)
                            {
                                updateTile
                            }
                        }*/
                        break;
                    case Tile::TileType::HasVictim:
                        
                        break;
                    case Tile::TileType::HasHazard:
                        {
                        bool hasAdjacentFire = false;
                        for (Tile tile : map->getAdjacentTiles(r, c))
                        {
                            if (tile.type == Tile::TileType::Fire)
                            {
                                hasAdjacentFire = true;
                                break; // räcker med att en av granntilar är i brand
                            }
                        }
                        if (hasAdjacentFire)
                        {
                            for (Tile tile : map->getAdjacentTiles(r, c))
                            {
                                if (tile.type != Tile::TileType::Wall && tile.type != Tile::TileType::Fire)
                                {
                                    updateTile(tile.Row, tile.Column, Tile::TileType::Fire);
                                    //vTaskDelay(15 / portTICK_PERIOD_MS); // Optional: Spread with delay
                                }
                            }
                        }
                    }
                    break;
                    case Tile::TileType::FireFighter:
                        
                        break;
                    default:
                        break;
                    }
                }
            }
    }

void Scene::openTileUpdates()
{
    sceneToSerial("Opening tile updates...");
    if (tileUpdateTaskHandle == NULL)
    {
    if (xTaskCreate(tileUpdateTask, "tileUpdateTask", 5000, this, 1, &tileUpdateTaskHandle) != pdPASS) {
        Serial.println("Failed to create ExternSceneUpdateTask");
    }
    }
    else
    {
        Serial.println("Task already running");
        
    }
}

void Scene::start()
{
    sceneToSerial("Starting sceneHandling...");
    if (mapHandlerTaskHandle == NULL)
    {
    if (xTaskCreate(mapHandlerTask, "mapHandlerTask", 5000, this, 1, &mapHandlerTaskHandle) != pdPASS) {
        Serial.println("Failed to create ExternSceneUpdateTask");
    }
    }
    else
    {
        Serial.println("Task already running");
    }
}

Scene::~Scene()
{
    if (map)
    {
        delete map;
    }
    vQueueDelete(sceneUpdateQueue);
}

void Scene::reset()
{
    sceneToSerial("Sceme Reset...");
    if (map)
    {
        delete map;
        map = nullptr;
    }
    if (mapHandlerTaskHandle != NULL)
    {
        vTaskDelete(mapHandlerTaskHandle);
        mapHandlerTaskHandle = NULL;
    }
    if (tileUpdateTaskHandle != NULL)
    {
        vTaskDelete(tileUpdateTaskHandle);
        tileUpdateTaskHandle = NULL;
    }
}
