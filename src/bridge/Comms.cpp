#include "Comms.h"



Comms::Comms(Scene *scene) : scene(scene), serialOutPutQueue(xQueueCreate(201, sizeof(char) * 256)), meshOutputQueue(xQueueCreate(201, sizeof(char) * 256)) 
{
    Serial.begin(115200);
    Serial.setTimeout(50);
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.setName(nodeName);
    /*mesh.onReceive([this](uint32_t from, String& msg) {
    this->enqueueSerialOutput(msg);
    });*/



    mesh.onReceive([this](String &from, String &msg) {
    this->enqueueSerialOutput("Mesh message, from: " + from + ": " + msg);
    });

      mesh.onChangedConnections([this]() {
    this->enqueueSerialOutput("Mesh message: Changed connection");
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
                if (msgChar != "") {
                    try {
                        Serial.println(msgChar);
                    }
                    catch (...) {
                        
                    }
                }
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }

    void Comms::serialReadTask(void *pvParameters)
 {
    Comms* comms = static_cast<Comms*>(pvParameters);
    struct CommandToGui
    {
        String Command;
        String ToJson()
        {
            StaticJsonDocument<256> doc;
            doc["Command"] = Command;
            String json;
            serializeJson(doc, json);
            return json;
        }

    };
        int amntjson = 0;
        int amntString = 0;
        while (1) {
            if (Serial.available() > 0) {
                String msg = Serial.readStringUntil('\n');
                if (msg.startsWith("{") && msg.endsWith("}"))
                {
                    comms->enqueueSerialOutput(msg);
                    StaticJsonDocument<256> doc;
                    DeserializationError error = deserializeJson(doc, msg);
                    if (error) {
                        comms->enqueueSerialOutput("Failed to parse JSON");
                    }
                    else{
                        //comms->enqueueSerialOutput("Parsed JSON, " + String(amntjson) + ", " + msg);
                        //amntjson++;
                        CommandToGui commandToGui;
                        switch (comms->stringToCommand(doc["Command"]))
                        {
                        case commandsToReceive::NewMap:
                            comms->scene->reset();
                            comms->scene->createNewMap(doc["Rows"], doc["Columns"]);
                            comms->scene->openTileUpdates();
                            break;
                        case commandsToReceive::Tile:
                            comms->scene->enqueueMapUpdate(doc["Row"], doc["Column"], Tile::stringToType(doc["Type"]));
                            break;
                        case commandsToReceive::Go:
                            comms->scene->start(); // starta hantering av karta
                            break;
                        case commandsToReceive::Reset:
                            comms->scene->reset();
                            commandToGui.Command = "Reset";
                            comms->enqueueSerialOutput(commandToGui.ToJson());
                            break;
                        default:
                            break;
                        }

                    }
                }
                else 
                {
                    comms->enqueueSerialOutput("Bradcasting :" + msg);
                    comms->enqueueMeshOutput(msg);
                }


                //comms->enqueueSerialOutput(msg);
                //comms->enqueueMeshOutput(msg);
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

    void Comms::enqueueMeshOutput(const String &msg)
    {
        if (msg != "") {
            char msgChar[256];
            msg.toCharArray(msgChar, sizeof(msgChar));
            if (xQueueSend(meshOutputQueue, &msgChar, 10) != pdPASS) {
                Serial.println("Failed to add to mesh queue");
            }
        }
    }

    void Comms::enqueueSerialOutput(const String &msg)
    {
        if (msg != "") {
            char msgChar[256];
            msg.toCharArray(msgChar, sizeof(msgChar));
            if (xQueueSend(serialOutPutQueue, &msgChar, 10) != pdPASS) {
                Serial.println("Failed to add to serial queue");
            }
        }
    }
