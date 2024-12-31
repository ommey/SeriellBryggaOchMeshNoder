#include "FireFighter.h"

constexpr size_t MSG_CHAR_SIZE = 256;
constexpr TickType_t QUEUE_SEND_TIMEOUT = 10 / portTICK_PERIOD_MS;

FireFighter::FireFighter() : x(0), y(0), fireFighterTaskHandle(nullptr), serialOutputQueue(nullptr), meshOutPutQueue(nullptr), map(nullptr) {}

FireFighter::~FireFighter()
{
    reset();
}

void FireFighter::registerSerialOutput(QueueHandle_t *serialOutputQueue)
{
    this->serialOutputQueue = serialOutputQueue;
}

void FireFighter::registerMeshOutput(QueueHandle_t *meshOutPutQueue)
{
    this->meshOutPutQueue = meshOutPutQueue;
}

void FireFighter::registerMap(Map *map)
{
    this->map = map;
}

void FireFighter::start()
{
    if (fireFighterTaskHandle == nullptr)
    {
        BaseType_t result = xTaskCreate(FireFighterTask, "FireFighterTask", 2048, this, 1, &fireFighterTaskHandle);
        if (result != pdPASS)
        {
            Serial.printf("Failed to create FireFighterTask, error code: %d\n", result);
        }
    }
    else
    {
        Serial.println("Task already running");
    }
}

void FireFighter::reset()
{
    if (fireFighterTaskHandle != nullptr)
    {
        vTaskDelete(fireFighterTaskHandle);
        fireFighterTaskHandle = nullptr;
    }
}

void FireFighter::spawnFireFighter()
{
    if (map == nullptr)
    {
        Serial.println("Map is not registered.");
        return;
    }

    std::vector<Tile> validSpawnLocations = map->getValidSpawnLocations();
    if (!validSpawnLocations.empty())
    {
        randomSeed(millis());
        int randomIndex = random(0, validSpawnLocations.size());
        Tile randomTile = validSpawnLocations[randomIndex];
        this->x = randomTile.Row;
        this->y = randomTile.Column;
        updateTile(randomTile.Row, randomTile.Column, Tile::TileType::FireFighter);
    }
}

void FireFighter::moveFireFighter(int newX, int newY)
{
}

void FireFighter::moveFireFighter()
{
    if (map == nullptr)
    {
        Serial.println("Map is not registered.");
        return;
    }

    std::vector<Tile> validMoveLocations = map->getAdjacentTiles(x, y);
    if (!validMoveLocations.empty())
    {
        randomSeed(millis());
        int randomIndex = random(0, validMoveLocations.size());
        Tile randomTile = validMoveLocations[randomIndex];
        moveTile(x, y, randomTile.Row, randomTile.Column);
        enqueueSerialOutput("FireFighter moved to: " + String(randomTile.Row) + ", " + String(randomTile.Column) + " from: " + String(x) + ", " + String(y));
        this->x = randomTile.Row;
        this->y = randomTile.Column;
    }
}

void FireFighter::updateTile(int row, int column, Tile::TileType type)
{
    TileUpdate tileUpdate(row, column, Tile::typeToString(type));
    String json = tileUpdate.ToJson();
    enqueueSerialOutput("Sending via mesh: " + json);
    enqueueMeshOutput(json);
}

void FireFighter::moveTile(int x, int y, int newX, int newY)
{
    TileUpdate tileUpdate(x, y, newX, newY, "FireFighter");
    String json = tileUpdate.ToJson();
    enqueueSerialOutput("Sending via mesh: " + json);
    enqueueMeshOutput(json);
}

void FireFighter::enqueueSerialOutput(const String &msg)
{
    enqueueOutput(serialOutputQueue, msg);
}

void FireFighter::enqueueMeshOutput(const String &msg)
{
    enqueueOutput(meshOutPutQueue, msg);
}

void FireFighter::enqueueOutput(QueueHandle_t *queue, const String &msg)
{
    if (queue != nullptr && !msg.isEmpty())
    {
        char msgChar[MSG_CHAR_SIZE];
        msg.toCharArray(msgChar, sizeof(msgChar));
        if (xQueueSend(*queue, &msgChar, QUEUE_SEND_TIMEOUT) != pdPASS)
        {
            Serial.println("Failed to enqueue message.");
        }
    }
}

void FireFighter::FireFighterTask(void *pvParameters)
{
    FireFighter *fireFighter = static_cast<FireFighter *>(pvParameters);

    if (fireFighter->map == nullptr || !fireFighter->map->isCreated())
    {
        Serial.println("Map is not initialized or not ready.");
        vTaskDelete(nullptr);
        return;
    }

    fireFighter->spawnFireFighter();

    while (true)
    {
        //fireFighter->enqueueMeshOutput("my position: " + String(fireFighter->x) + ", " + String(fireFighter->y));
        fireFighter->enqueueSerialOutput("my position: " + String(fireFighter->x) + ", " + String(fireFighter->y));
        fireFighter->enqueueSerialOutput("FireFighterTask going strong: " + String(millis()));
        fireFighter->moveFireFighter();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
