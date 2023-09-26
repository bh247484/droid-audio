//
// Created by Brendan Huffman on 9/23/23.
//

#include "LPF.h"
#include <cmath>

void LPF::setCutoff(float newCutoff) {
    float k = std::tan(M_PI * newCutoff);
    float norm = 1.0f / (1.0f + k / Q + k * k);
    a0 = k * k * norm;
    a1 = 2 * a0;
    a2 = a0;
    b1 = 2 * (k * k - 1) * norm;
    b2 = (1 - k / Q + k * k) * norm;
}

float LPF::process(float input) {
    float out = input * a0 + z1;
    z1 = input * a1 + z2 - b1 * out;
    z2 = input * a2 - b2 * out;

    return out;
}
