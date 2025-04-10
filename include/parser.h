#pragma once
#include "config.h"

class Parser {
public:
    static void parseDirectCommands(JointConfig* joints);
    static void parseIKCommands(JointConfig* joints);

private:
    static void processDirectInput(String input, JointConfig* joints);
    static void processIKInput(String input, JointConfig* joints);
    static void parseCSV(String input, float& x, float& y, float& z);
};