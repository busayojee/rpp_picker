#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <math.h>

enum OperationMode {
    MODE_NONE,
    DIRECT_MODE,
    IK_MODE
};


extern const int NUM_TASK_STEPS; 
extern const int NUM_JOINTS;

extern const int STEPS_PER_METER;
extern const int16_t SERVO_NEUTRAL;
extern const int16_t SERVO_CW_MAX;
extern const int16_t SERVO_CCW_MAX;
extern const int DEGREES_PER_STEP;
extern const int TOLERANCE;
extern const int Tolerance_linear;


extern int currentTaskStep;
extern unsigned long taskTimer;
extern bool taskInProgress;
extern OperationMode currentMode;

struct JointConfig {
    uint8_t encCLK;
    uint8_t encDT;
    uint8_t servoPin;
    volatile int32_t encoderPos;
    int currentPosition;  
    int targetPosition;
    int stepsPerMeter; 
    bool isLinear;
    int JointLimit;
    int jointNum;
};

extern JointConfig joints[];

extern int taskSequence[][3];

struct SavedData {
    char signature[4]; //  = "MEC"
    int32_t encoderPositions[3];  
    float stepsPerMeter[3];     
};

extern unsigned long lastEEPROMSave;
extern const unsigned long EEPROM_SAVE_INTERVAL;
