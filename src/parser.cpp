#include "parser.h"
#include "ik.h"
#include "taskrunner.h"
#include <Arduino.h>
#include "config.h"

void Parser::parseDirectCommands(JointConfig* joints) {
    static String inputBuffer;
    
    while(Serial.available()) {
        char c = Serial.read();
        if(c == '\n') {
            if(inputBuffer.equalsIgnoreCase("TASK")) {
                taskrunner::startTask(joints);
            } else {
                processDirectInput(inputBuffer, joints);
            }
            inputBuffer = "";
        } 
        else if(c != '\r') {
            inputBuffer += c;
        }
    }
}

void Parser::parseIKCommands(JointConfig* joints) {
    static String inputBuffer;
    
    while(Serial.available()) {
        char c = Serial.read();
        if(c == '\n') {
            processIKInput(inputBuffer, joints);
            inputBuffer = "";
        } 
        else if(c != '\r') {
            inputBuffer += c;
        }
    }
}

void Parser::processDirectInput(String input, JointConfig* joints) {
    int targets[3] = {0};
    int index = 0;
    
    while(input.length() > 0 && index < 3) {
        int commaIndex = input.indexOf(',');
        if(commaIndex == -1) commaIndex = input.length();
        
        targets[index] = input.substring(0, commaIndex).toInt();
        input = input.substring(commaIndex + 1);
        index++;
    }

    for(int i=0; i<NUM_JOINTS; i++) {
        joints[i].targetPosition = constrain(targets[i], 0, joints[i].JointLimit);
    }
}

void Parser::processIKInput(String input, JointConfig* joints) {
    float x=0, y=0, z=0;
    parseCSV(input, x, y, z);
    
    int theta1, d2, d3;
    IK::calculate(x, y, z, theta1, d2, d3);
    theta1 = constrain(theta1, 0, joints[0].JointLimit);
    d2     = constrain(d2,   0, joints[1].JointLimit);
    d3     = constrain(d3,   0, joints[2].JointLimit);  
    joints[0].targetPosition = theta1;
    joints[1].targetPosition = d2;
    joints[2].targetPosition = d3;
}

void Parser::parseCSV(String input, float& x, float& y, float& z) {
    input.trim();
    int index = 0;
    
    while(input.length() > 0 && index < 3) {
        int commaIndex = input.indexOf(',');
        if(commaIndex == -1) commaIndex = input.length();
        
        String token = input.substring(0, commaIndex);
        switch(index) {
            case 0: x = token.toFloat(); break;
            case 1: y = token.toFloat(); break;
            case 2: z = token.toFloat(); break;
        }
        input = input.substring(commaIndex + 1);
        index++;
    }
}