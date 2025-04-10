#pragma once
#include "config.h"

class Eeprom{
    public:
        static void loadPositions(JointConfig* joints);
        static void savePositions(JointConfig* joints);

    private:
        static bool validateSignature(SavedData &data);
};
