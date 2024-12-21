#include <Arduino.h>
#include "Scene.h"
#include "Comms.h"



// Main application
void setup() {
    delay(1000);
    static Scene scene;
    static Comms comms(&scene);
    scene.registerSerialQueue(&comms.serialOutPutQueue);
    comms.start();
    //comms.serialOutPut("Just started");
    //comms.meshOutPut("Just started");

}

void loop() {
    // Nothing here, tasks handle everything
}
