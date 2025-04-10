#include "config.h"
#include "myEeprom.h"


void Eeprom::loadPositions(JointConfig* joints) {
    SavedData data;
    EEPROM.get(0, data);
    
    if(!validateSignature(data)) {
        for(int i=0; i<NUM_JOINTS; i++) {
            joints[i].encoderPos = 0;
            joints[i].stepsPerMeter = joints[i].isLinear ? STEPS_PER_METER : 0;
        }
        savePositions(joints);
        return;
    }
    
    for(int i=0; i<NUM_JOINTS; i++) {
        joints[i].encoderPos = data.encoderPositions[i];
        joints[i].stepsPerMeter = data.stepsPerMeter[i];
          if (joints[i].isLinear) {
            joints[i].currentPosition = (joints[i].encoderPos * 1000) / joints[i].stepsPerMeter;
        }
        else {
            joints[i].currentPosition = (joints[i].encoderPos * DEGREES_PER_STEP) % 360;
        }
    }
}

void Eeprom::savePositions(JointConfig* joints) {
    SavedData data;
    strncpy(data.signature, "MEC", 3);
    for(int i=0; i<NUM_JOINTS; i++) {
        data.encoderPositions[i] = joints[i].encoderPos;
        data.stepsPerMeter[i] = joints[i].stepsPerMeter;
    }
    EEPROM.put(0, data);
    lastEEPROMSave = millis();
}

bool Eeprom::validateSignature(SavedData &data) {
    return strncmp(data.signature, "MEC", 3) == 0;
}
