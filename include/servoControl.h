#pragma once
#include "config.h"

class ServoControl{
    public:
        static void initialize(JointConfig *joints);
        static void updateServo(const JointConfig& joint, int16_t value);
        static bool moveToTarget(JointConfig& joint);
    
    private:
        
};
