#include "Scene.h"

Scene::Scene() : map(nullptr), sceneSerialQueue(nullptr), /*comms(nullptr),*/ sceneUpdateQueue(xQueueCreate(1024, sizeof(MapUpdate))){}

/*void Scene::registerComms(Comms *comms)
{
    this->comms = comms;
}*/

void Scene::registerSerialQueue(QueueHandle_t* serialQueue)
{
    this->sceneSerialQueue = serialQueue;
}

/*void Scene::sceneToComms(const String &msg)
{
    if (comms)
    {
    comms->serialOutPut(msg);
    }
    else
    {
    Serial.println("No comms to send to");
    }
}*/
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


void Scene::mapHandlerTask(void *p)
{
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
    
    Scene* scene = static_cast<Scene*>(p);
    MapUpdate mapUpdate;
    int progression = 0;

    while (true)
    {
        // Process all pending map updates
        while (xQueueReceive(scene->sceneUpdateQueue, &mapUpdate, 0) == pdPASS) // Non-blocking
        {
            if (scene->map)
            {
                scene->map->updateTile(mapUpdate.row, mapUpdate.column, mapUpdate.type);
                scene->sceneToComms("Tile updated at (" + String(mapUpdate.row) + ", " + String(mapUpdate.column) + ")");
            }
            else
            {
                scene->sceneToComms("No map to update");
            }
        }

        // Perform periodic handling logic
        if (scene->map)
        {
            TileUpdate tileUpdate(progression, progression++, "Fire");
            scene->sceneToComms(tileUpdate.ToJson());
            for(int r = 0; r < scene->map->Rows; r++)
            {
                for(int c = 0; c < scene->map->Columns; c++)
                {
                    if (scene->map->tiles[r][c].type == Tile::Fire)
                    {
                    //scene->sceneToComms("Fire at (" + String(r) + ", " + String(c) + ")");

                    
                    }
                    }
                }
            }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for periodic processing
        }

    }

        void Scene::start()
        {
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
}
