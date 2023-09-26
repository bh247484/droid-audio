//
// Created by Brendan Huffman on 9/23/23.
//

#ifndef DROIDAUDIO_LPF_H
#define DROIDAUDIO_LPF_H

/*
 * Biquad low pass filter implemented using the RBJ cookbook: https://www.w3.org/TR/audio-eq-cookbook.
**/
class LPF {
public:
    LPF() = default;
    ~LPF() = default;

    void setCutoff(float newCutoff);
    float process(float input);

private:
    float Q = 1.25f;
    float a0, a1, a2, b1, b2 = 0.0f;
    float z1, z2 = 0.0f;
};


#endif //DROIDAUDIO_LPF_H
