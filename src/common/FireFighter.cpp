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

void FireFighter::FireFighterTask(void *pvParameters)
{
    FireFighter* fireFighter = static_cast<FireFighter*>(pvParameters);
    while (1)
    {
        fireFighter->enqueueSerialOutput("FireFighterTask going strong" + String(millis()));
        if(fireFighter->map != nullptr)
        {
            fireFighter->enqueueSerialOutput("Im working with map: " + String(fireFighter->map->Rows) + ", " + String(fireFighter->map->Columns));
        }
        else 
        {
            fireFighter->enqueueMeshOutput("No map to work with");
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
