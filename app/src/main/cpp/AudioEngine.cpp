//
// Created by Brendan Huffman on 9/23/23.
//


#include <android/log.h>
#include "AudioEngine.h"
#include "dj_fft.h"

AudioEngine::AudioEngine() {
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setChannelCount(1);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setCallback(this);
    builder.openStream(&stream);

    stream->setBufferSizeInFrames(stream->getFramesPerBurst() * 2);
    stream->requestStart();
    sampleRate = stream->getSampleRate();
    sawOsc.initParams(sampleRate);
}

AudioEngine::~AudioEngine() {
    // Cleanup heap allocated member variables.
    delete fftCache;
}

std::array<std::string, 3> AudioEngine::getSpectralHexes() {
    return hexCache;
}

void AudioEngine::buildHexCache() {
    int fftSize = fftCache->size();

    // Extract the real magnitudes of fft data and store the index (we'll need it later to calculate freq).
    std::vector<std::pair<float, int>> fftExtract;
    for (int i = 0; i < fftSize; ++i) {
        // Important to use absolute values here so we don't get conjugate, imaginary results.
        float realMag = std::abs(fftCache->at(i).real());
        fftExtract.push_back(std::make_pair(realMag, i));
    }

    // Sort fft data by highest magnitudes to isolate notable harmonics.
    std::sort(
            fftExtract.begin(),
            fftExtract.end(),
            [](std::pair<float, int> a, std::pair<float, int> b) {
                return a.first > b.first;
            });

    std::array<std::array<int, 3>, 3> rgbColors;
    // Extract the first 9 harmonics.
    // We'll use groups of 3 harmonics to calculate RGB values of each of the 3 colors in the view gradient.
    for (int i = 0; i < 9; ++i) {
        // float magnitude = fftExtract[i].first;

        // Use fft bin/index to find freq: | bin * sampleRate / fftSize = freq in hz |
        // Using i * 99 as index to get some harmonic separation. Closely correlated vals lead to boring, gray hex colors.
        // The modulo wraps around so we don't access data out of bounds.
        float frequency = fftExtract[(i * 99) % fftExtract.size()].second * sampleRate / fftSize;

        // Map frequency to RGB value range (0 - 255).
        int rgbVal = (int) ((frequency / sampleRate) * 255.0f);

        rgbColors[i / 3][i % 3] = rgbVal;
    }

    std::array<std::string, 3> hexColors;
    for (int i = 0; i < rgbColors.size(); ++i) {
        std::stringstream stream;
        stream << std::hex << rgbColors[i][0] << rgbColors[i][1] << rgbColors[i][2];

        std::string hexStr = stream.str();
        // std::hex returns a single char for some values. Zero padding here as needed.
        while(hexStr.length() < 6) {
            hexStr += "0";
        }
        hexColors[i] = hexStr;
    }

    hexCache = hexColors;
}

void AudioEngine::updateSpectralData(float *output, int numFrames) {
    // FFT expects a signal with a power-of-two length so it can exploit symmetrical redundancies in the algorithm.
    // So we need to find the next highest power-of-two above the buffer length and zero pad it accordingly.
    float floorExp = std::floor(std::log2(numFrames));
    float ceilPow2 = (int) std::pow(2, floorExp + 1);

    if (fftCache->size()) {
        fftCache->clear();
    }
    fftCache->resize(ceilPow2);

    for (int i = 0; i < ceilPow2; ++i) {
        if (i < numFrames) {
            fftCache->at(i) = output[i];
        } else {
            // Zero pad beyond end of buffer data up to nearest power of 2.
            fftCache->at(i) = 0.0f;
        }
    }

    dj::fft1d(*fftCache, dj::fft_dir::DIR_BWD);
}

/*
 * Main audio callback provided to us by the oboe library.
 * DSP processing begins here.
**/
oboe::DataCallbackResult
AudioEngine::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    float *output = (float *)audioData;
    for (int i = 0; i < numFrames; i++) {
        if (playSound) {
            float sawOut = sawOsc.render();
            float filterOut = filter.process(sawOut);
            output[i] = filterOut;
        } else {
            output[i] = 0.0f;
        }
    }

    // Spectral fft data caching logic.
    if (sampleRate) {
        cacheCount += numFrames;
        //  Update spectral data and bust cache every 10th of a second.
        if (cacheCount > (sampleRate / 10)) {
            updateSpectralData(output, numFrames);
            buildHexCache();
            cacheCount = 0;
        }
    }

    return oboe::DataCallbackResult::Continue;
}
