#pragma once
#include "config.h"


class RPP{
    public:
        static void initialize();
        static void update();
        
    private:
        static void promptOperationMode();
        static void handleModeSelection();
        static void returnToZero();
        
};