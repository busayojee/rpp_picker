#include "config.h"


const int NUM_TASK_STEPS = 10;
const int NUM_JOINTS = 3;

const int STEPS_PER_METER = 5880;   // approx
const int16_t SERVO_NEUTRAL = -255;
const int16_t SERVO_CW_MAX   = -209;
const int16_t SERVO_CCW_MAX  = -300;
const int DEGREES_PER_STEP   = 6;
const int TOLERANCE          = 12;
const int Tolerance_linear   = 2;


int currentTaskStep = -1;
unsigned long taskTimer = 0;
bool taskInProgress = false;
OperationMode currentMode = MODE_NONE;


JointConfig joints[NUM_JOINTS] = {
    {18, 19, 12, 0, 0, 0, 0, false, 180, 1},  
    {2,  3,  10, 0, 0, 0, STEPS_PER_METER, true, 200, 2},
    {20, 21, 11, 0, 0, 0, STEPS_PER_METER, true, 100, 3}
};


int taskSequence[NUM_TASK_STEPS][3] = {
    {0,  0,   0},
    {0,  140, 50},
    {0,  140, 100},
    {0,  100, 100},
    {0,  0,   0},
    {90, 0,   0},
    {90, 140, 100},
    {90, 140, 50},
    {90, 0,   0},
    {0,  0,   0}
};


unsigned long lastEEPROMSave       = 0;
const unsigned long EEPROM_SAVE_INTERVAL = 5000;
