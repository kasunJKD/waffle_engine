#include "w_math.h"
namespace libc_w_math {
    float add(float a, float b) { return a + b; }
    float sub(float a, float b) { return a - b; }
    float mul(float a, float b) { return a * b; }
    float div(float a, float b) { return (b != 0) ? a / b : 0.0f; }
}
