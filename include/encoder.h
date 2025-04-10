#pragma once
#include "config.h"

class Encoders{
    public:
        static void initialize(JointConfig *joints);
        static void handleEncoder(uint8_t joint);
    private:
        static JointConfig *m_joints;
        static void encoderISR_J0(); 
        static void encoderISR_J1();
        static void encoderISR_J2();
        
};