#include "Scene.h"

Scene::Scene() : map(Map()), sceneSerialQueue(nullptr), /*comms(nullptr),*/ sceneUpdateQueue(xQueueCreate(1024, sizeof(MapUpdate))){}

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
    map.createMap(rows, columns);
    sceneToSerial("Created new map" + String(rows) + "x" + String(columns));
}

void Scene::enqueueMapUpdate(int row, int column, Tile::TileType type)
{
    if (!map.isCreated())
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

    while (scene->map.isCreated())
    {

        if (xQueueReceive(scene->sceneUpdateQueue, &mapUpdate, 0) == pdPASS) // Non-blocking
        {
            if (scene->map.isCreated())
            {
                scene->map.updateTile(mapUpdate.row, mapUpdate.column, mapUpdate.type);
                

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

    while (scene->map.isCreated())
    {
        // Perform periodic handling logic
        if (scene->map.isCreated())
        {              
         scene->internMapUpdate(); 
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for periodic processing
        }

    }

    void Scene::updateTile(int row, int column, Tile::TileType type)
    {
    TileUpdate tileUpdate(row, column, Tile::typeToString(type));
    sceneToSerial(tileUpdate.ToJson());
    //internt i klienten
    map.updateTile(row, column, type);
    //för klienten
    }

    void Scene::internMapUpdate()
    {
        return;
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
    if (map.isCreated())
    {
        map.createMap(0, 0);
    }
    vQueueDelete(sceneUpdateQueue);

}

void Scene::reset()
{
    sceneToSerial("Sceme Reset...");
    if (map.isCreated())
    {
        map.createMap(0, 0);
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
