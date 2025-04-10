#include "config.h"
#include "encoder.h"
#include "myEeprom.h"
#include "servoControl.h"
#include "parser.h"
#include "taskrunner.h"
#include "rpp.h"

void RPP::initialize(){
    Serial.begin(9600);
    Serial.println("Booting...");

    // Initialize hardware
    Encoders::initialize(joints);
    ServoControl::initialize(joints);

    // load previous positions from EEPROM
    Eeprom::loadPositions(joints);

    // Move all joints to zero
    returnToZero();

    // Prompt user
    promptOperationMode();
    Serial.println("Ready to go!");
}

void RPP::update()
{
     // If no mode selected, wait for user
    if (currentMode == MODE_NONE) {
        handleModeSelection();
        return;
    }

    // If a task is in progress, keep it moving
    if (taskInProgress) {
        taskrunner::runTask(joints);
    }

    // User to drive the robot
    if (currentMode == DIRECT_MODE) {
        Parser::parseDirectCommands(joints);
    } 
    else if (currentMode == IK_MODE) {
        Parser::parseIKCommands(joints);
    }

    // Move each joint a bit closer to target
    bool anyMoving = false;
    for(int i=0; i<NUM_JOINTS; i++) {
        bool stillMoving = ServoControl::moveToTarget(joints[i]);
        if(stillMoving) anyMoving = true;
    }

    // 5 seconds periodic saves
    if(millis() - lastEEPROMSave > EEPROM_SAVE_INTERVAL) {
        Eeprom::savePositions(joints);
        Serial.println("Positions saved");
    }
    // A small delay so we don't spam too fast
    delay(10);
}

void RPP::promptOperationMode(){
    Serial.println();
    Serial.println("Select operation mode:");
    Serial.println("  D - Direct joint control");
    Serial.println("  I - Inverse kinematics control");
    currentMode = MODE_NONE;
}

void RPP::handleModeSelection(){
        if (Serial.available()) {
        char mode = toupper(Serial.read());
        if (mode == 'D') {
            currentMode = DIRECT_MODE;
            Serial.println("Selected: Direct Control Mode");
        } else if (mode == 'I') {
            currentMode = IK_MODE;
            Serial.println("Selected: Inverse Kinematics Mode");
        } else {
            Serial.println("Invalid mode, choose D or I");
            promptOperationMode();
        }
        while (Serial.available()) Serial.read(); // flush input
    }
}

void RPP::returnToZero(){
    Serial.println("Homing all joints...");
    for(int i=0; i<NUM_JOINTS; i++) {
        int initPos = joints[i].currentPosition;
        Serial.print("J");
        Serial.print(i+1);
        Serial.print(" initial: ");
        Serial.print(initPos);
        Serial.println(joints[i].isLinear ? "mm" : "deg");
        
        joints[i].targetPosition = 0;
        while(1) {

            bool stillMoving = ServoControl::moveToTarget(joints[i]);
            delay(10);
            if(!stillMoving) break;
        }
        // Reset the encoder count for that joint
        joints[i].encoderPos = 0;
        joints[i].currentPosition = 0;
        // Stop servo
        ServoControl::updateServo(joints[i], SERVO_NEUTRAL);
    }
    Eeprom::savePositions(joints);
    Serial.println("Homing complete");
}
