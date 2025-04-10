#pragma once
#include "config.h"

class taskrunner {
public:
    static void startTask(JointConfig *joints);
    static void runTask(JointConfig *joints);

private:
    static bool allJointsReached(const JointConfig* joints);
    static void advanceTaskStep(JointConfig* joints);
};
