#include <Arduino.h>
#include "Scene.h"
#include "Comms.h"
#include "FireFighter.h"



// Main application
void setup() {
    delay(1000);
    static Scene scene;
    static FireFighter fireFighter;
    static Comms comms(&scene, &fireFighter);
    fireFighter.registerSerialOutput(&comms.serialOutPutQueue);
    fireFighter.registerMeshOutput(&comms.meshOutputQueue);
    fireFighter.registerMap(&*scene.map);
    scene.registerSerialQueue(&comms.serialOutPutQueue);
    comms.start();
    comms.enqueueSerialOutput("Just started");
    //comms.enqueueMeshOutput("Just started");

}

void loop() {
    // Nothing here, tasks handle everything
}
