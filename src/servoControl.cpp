#include "servoControl.h"
#include "config.h"

void ServoControl::initialize(JointConfig *joints)
{
    for(int i=0; i<NUM_JOINTS; i++){
        pinMode(joints[i].servoPin, OUTPUT);
    }
}

void ServoControl::updateServo(const JointConfig& joint, int16_t value)
{
    analogWrite(joint.servoPin, value);
}

// void ServoControl::stopServo()
// {
// }

bool ServoControl::moveToTarget(JointConfig& joint){
    joint.targetPosition = constrain(joint.targetPosition, 0, joint.JointLimit);
    int error = joint.targetPosition - joint.currentPosition;
    int tol = joint.isLinear ? Tolerance_linear : TOLERANCE;
    
    static unsigned long moveEndTime[3] = {0};

    if(abs(error) <= tol) {
        if(moveEndTime[joint.jointNum-1] == 0) {
            moveEndTime[joint.jointNum-1] = millis();
        }
        if(millis() - moveEndTime[joint.jointNum-1] >= 100) {
            updateServo(joint, SERVO_NEUTRAL);
            return false;
        }
        return true;
    }
    
    int16_t output;
    if (error > 0) {
        output = (joint.jointNum == 3) ? SERVO_CCW_MAX : SERVO_CW_MAX;
    } else {
        output = (joint.jointNum == 3) ? SERVO_CW_MAX : SERVO_CCW_MAX;
    }
    updateServo(joint, output);

    return true;
}
