#include "rpp.h"

RPP robot;
void setup(){
    // Boot the robot
    robot.initialize();
}

void loop(){
    // Run the robot
    robot.update();
}

