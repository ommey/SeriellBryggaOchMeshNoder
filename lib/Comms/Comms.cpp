#include "Comms.h"

Comms::Comms(Scene *scene) : scene(scene), serialOutPutQueue(xQueueCreate(200, sizeof(char) * 256)), meshOutputQueue(xQueueCreate(200, sizeof(char) * 256)) {
    Serial.begin(115200);
    Serial.setTimeout(50);
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive([this](uint32_t from, String& msg) {
        this->serialOutPut(msg);
    });
    }
    
void Comms::meshUpdate(void *pvParameters)
{
    Comms* comms = static_cast<Comms*>(pvParameters);
    while (1) {
        comms->mesh.update();
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}

void Comms::meshBroadCastTask(void *pvParameters)
{
Comms* comms = static_cast<Comms*>(pvParameters);
char msgChar[256];
while (1) 
    {
    if (xQueueReceive(comms->meshOutputQueue, &msgChar, 10) == pdPASS) 
    {
        if (!comms->mesh.sendBroadcast(msgChar)) 
        {
        Serial.println("Failed to send broadcast");
        }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void Comms::serialWriteTask(void *pvParameters)
    {
        Comms* comms = static_cast<Comms*>(pvParameters);
        char msgChar[256];
        while (1) {
            if (xQueueReceive(comms->serialOutPutQueue, &msgChar, 10) == pdPASS) {
                Serial.println(msgChar);
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }

    void Comms::serialReadTask(void *pvParameters)
 {
        Comms* comms = static_cast<Comms*>(pvParameters);
        int amntjson = 0;
        int amntString = 0;
        while (1) {
            if (Serial.available() > 0) {
                String msg = Serial.readStringUntil('\n');
                if (msg.startsWith("{") && msg.endsWith("}"))
                {
                    StaticJsonDocument<256> doc;
                    DeserializationError error = deserializeJson(doc, msg);
                    if (error) {
                        comms->serialOutPut("Failed to parse JSON");
                    }
                    else{
                        //comms->serialOutPut("Parsed JSON, " + String(amntjson) + ", " + msg);
                        //amntjson++;
                        switch (comms->stringToCommand(doc["Command"]))
                        {
                        case commandsToReceive::NewMap:
                            comms->scene->createNewMap(doc["Rows"], doc["Columns"]);
                            break;
                        case commandsToReceive::Tile:
                            comms->scene->enqueueMapUpdate(doc["Row"], doc["Column"], Tile::stringToType(doc["Type"]));
                            break;
                        case commandsToReceive::Go:
                            comms->scene->start();
                            break;
                        case commandsToReceive::Reset:
                            comms->scene->reset();
                            amntjson = 0;
                            break;
                        default:
                            break;
                        }

                    }
                }


                //comms->serialOutPut(msg);
                //comms->meshOutPut(msg);
            }
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
    }

    Comms::commandsToReceive Comms::stringToCommand(const String &command)
    {
        if (command == "NewMap"){
            return NewMap;
        }
        else if (command == "Tile"){
            return Tile;
        }
        else if(command == "Reset"){
            return Reset;
        }
        else if (command == "Go"){
            return Go;
        }
        else{
            return Reset;
        }
    }

    QueueHandle_t Comms::getSerialOutPutQueue()
   { 
    return serialOutPutQueue; 
   }

    Comms::~Comms()
    {
        vQueueDelete(serialOutPutQueue);
        vQueueDelete(meshOutputQueue);
    }

    void Comms::start()
    {
        if (xTaskCreate(meshUpdate, "meshUpdate", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create meshUpdate task");
        }
        if (xTaskCreate(serialWriteTask, "serialWriteTask", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create serialWriteTask");
        }
        if (xTaskCreate(serialReadTask, "serialReadTask", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create serialReadTask");
        }
        if (xTaskCreate(meshBroadCastTask, "meshBroadCastTask", 5000, this, 1, NULL) != pdPASS) {
            Serial.println("Failed to create meshBroadCastTask");
        }
    }

    void Comms::meshOutPut(const String &msg)
    {
        if (msg != "") {
            char msgChar[256];
            msg.toCharArray(msgChar, sizeof(msgChar));
            if (xQueueSend(meshOutputQueue, &msgChar, 10) != pdPASS) {
                Serial.println("Failed to add to mesh queue");
            }
        }
    }

    void Comms::serialOutPut(const String &msg)
    {
        if (msg != "") {
            char msgChar[256];
            msg.toCharArray(msgChar, sizeof(msgChar));
            if (xQueueSend(serialOutPutQueue, &msgChar, 10) != pdPASS) {
                Serial.println("Failed to add to serial queue");
            }
        }
    }
