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

void Scene::sceneToComms(const String &msg)
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
    sceneToComms("Created new map" + String(rows) + "x" + String(columns));
}

void Scene::enqueueMapUpdate(int row, int column, Tile::TileType type)
{
    if (!map)
            {
                sceneToComms("No map to update");
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

    while (true)
    {

        if (xQueueReceive(scene->sceneUpdateQueue, &mapUpdate, 0) == pdPASS) // Non-blocking
        {
            if (scene->map)
            {
                scene->map->updateTile(mapUpdate.row, mapUpdate.column, mapUpdate.type);
                //scene->sceneToComms("Server updated tile (" + String(mapUpdate.row) + ", " + String(mapUpdate.column) + ") to "+ Tile::typeToString(mapUpdate.type) +" from gui");
            }
            else
            {
                Serial.println("No map to update");
                //scene->sceneToComms("No map to update");
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS); // Delay for periodic processing
    }
}

void Scene::mapHandlerTask(void *p)
{
    Scene* scene = static_cast<Scene*>(p);

    while (true)
    {
        // Perform periodic handling logic
        if (scene->map)
        {
                        
            /*for(int r = 0; r < scene->map->Rows; r++)
            {
                for(int c = 0; c < scene->map->Columns; c++)
                {
                    if (scene->map->tiles[r][c].type == Tile::Fire)
                    {
                    //scene->sceneToComms("Fire at (" + String(r) + ", " + String(c) + ")");

                    
                    }
                    }
                }*/
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for periodic processing
        }

    }

    void Scene::updateTile(int row, int column, Tile::TileType type)
    {
    //för servern
    TileUpdate tileUpdate(row, column, Tile::typeToString(type));
    sceneToComms(tileUpdate.ToJson());
    //internt i servern
    map->updateTile(row, column, type);
    //för klienten
    }


void Scene::openTileUpdates()
{
    sceneToComms("Opening tile updates...");
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
    sceneToComms("Starting sceneHandling...");
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
    sceneToComms("Sceme Reset...");
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
