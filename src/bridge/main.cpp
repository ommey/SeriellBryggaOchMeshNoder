#include <Arduino.h>
#include "BridgeScene.h"
#include "BridgeComms.h"



// Main application
void setup() {
    delay(1000);
    static BridgeScene scene;
    static BridgeComms comms(&scene);
    scene.registerSerialQueue(&comms.serialOutPutQueue);
    scene.registerMeshQueue(&comms.meshOutputQueue);
    comms.start();
    comms.enqueueSerialOutput("Just started");
    //comms.enqueueMeshOutput("Just started");


}

void loop() {
    // Nothing here, tasks handle everything
}
