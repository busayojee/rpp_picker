#include "taskrunner.h"
#include "config.h"
#include <Arduino.h>


void taskrunner::startTask(JointConfig *joints) {
    currentTaskStep = 0;
    taskTimer = 0;
    taskInProgress = true;
    for(int i = 0; i < NUM_JOINTS; i++) {
        joints[i].targetPosition = taskSequence[currentTaskStep][i];
    }
    Serial.println("Task started");
}

void taskrunner::runTask(JointConfig* joints) {
     if(!taskInProgress || currentTaskStep == -1) return;
    if(allJointsReached(joints)) {
        if(taskTimer == 0) {
            taskTimer = millis();
            int dwellTime = 5000;
            if(taskSequence[currentTaskStep][1] == 140 ||
               taskSequence[currentTaskStep][2] == 100) {
                dwellTime = 0;
            }

            if(dwellTime > 0) {
                Serial.print("Dwelling for ");
                Serial.print(dwellTime / 1000);
                Serial.println(" seconds");
            }
        }
        else {
            int dwellTime = 5000;
            if(taskSequence[currentTaskStep][1] == 140 ||
               taskSequence[currentTaskStep][2] == 100) {
                dwellTime = 0;
            }

            if(dwellTime == 0 || (millis() - taskTimer >= (unsigned long)dwellTime)) {
                advanceTaskStep(joints);
                taskTimer = 0;
            }
        }
    }
    else {
        taskTimer = 0;
    }
}

bool taskrunner::allJointsReached(const JointConfig* joints) {
    for(int i=0; i<NUM_JOINTS; i++) {
        int error = abs(joints[i].targetPosition - joints[i].currentPosition);
        int tol = joints[i].isLinear ? Tolerance_linear : TOLERANCE;
        if(error > tol) return false;
    }
    return true;
}

void taskrunner::advanceTaskStep(JointConfig* joints) {
    currentTaskStep++;
    if(currentTaskStep >= NUM_TASK_STEPS) {
        currentTaskStep = -1;
        taskInProgress = false;
        Serial.println("Task complete!");
        return;
    }
    
    for(int i=0; i<NUM_JOINTS; i++) {
        joints[i].targetPosition = taskSequence[currentTaskStep][i];
    }
    Serial.print("Moving to step ");
    Serial.println(currentTaskStep);
}