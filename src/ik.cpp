#include "ik.h"
#include "config.h"
#include <math.h>

void IK::calculate(float x, float y, float z, int& theta1, int& d2, int& d3) {
    theta1 = (int)round(atan2(y, x) * 57.3);
    d2 = (int)round(z);
    d3 = (int)round(sqrt(x*x + y*y));
}
