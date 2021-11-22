#include "Airport.hpp"

float roundWithPrecision(const float &f) {
    float value = int(f * 100 + .5);
    return float(value / 100);
}
