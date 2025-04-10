#include "config.h"
#include "encoder.h"

JointConfig *Encoders::m_joints = nullptr;

void Encoders::initialize(JointConfig *joints){
    m_joints = joints;
    for(int i=0; i<NUM_JOINTS; i++) {
        pinMode(joints[i].encCLK, INPUT_PULLUP);
        pinMode(joints[i].encDT, INPUT_PULLUP);
        pinMode(joints[i].servoPin, OUTPUT);
        
        switch(i) {
            case 0: attachInterrupt(digitalPinToInterrupt(joints[0].encCLK), encoderISR_J0, CHANGE);attachInterrupt(digitalPinToInterrupt(joints[0].encDT), encoderISR_J0, CHANGE); break;
            case 1: attachInterrupt(digitalPinToInterrupt(joints[1].encCLK), encoderISR_J1, CHANGE);attachInterrupt(digitalPinToInterrupt(joints[1].encDT), encoderISR_J1, CHANGE); break;
            case 2: attachInterrupt(digitalPinToInterrupt(joints[2].encCLK), encoderISR_J2, CHANGE);attachInterrupt(digitalPinToInterrupt(joints[2].encDT), encoderISR_J2, CHANGE); break;
        }
    }
}

void Encoders::handleEncoder(uint8_t joint){
    JointConfig &j = m_joints[joint];
    uint8_t clk = digitalRead(j.encCLK);
    uint8_t dt = digitalRead(j.encDT);
    uint8_t encoded = (clk << 1) | dt;
    static uint8_t lastEncoded[3] = {0};

    uint8_t sum = (lastEncoded[joint] << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        j.encoderPos--;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        j.encoderPos++;
    }
    // Serial.println(j.encoderPos);
    j.currentPosition = j.isLinear ? 
        (j.encoderPos * 1000) / j.stepsPerMeter : 
        (j.encoderPos * DEGREES_PER_STEP) % 360;
    lastEncoded[joint] = encoded;
}

void Encoders::encoderISR_J0() { handleEncoder(0); }
void Encoders::encoderISR_J1() { handleEncoder(1); }
void Encoders::encoderISR_J2() { handleEncoder(2); }