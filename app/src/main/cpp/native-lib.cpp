#include <jni.h>
#include <string>
#include <math.h>
#include <android/log.h>
#include <oboe/Oboe.h>
#include "AudioEngine.h"

// Init AudioEngine Singleton.
static AudioEngine engine;

extern "C" {
// Toggle sound on screen press.
JNIEXPORT jint JNICALL
Java_com_example_droidaudio_MainActivity_toggleSound(
        JNIEnv* env,
jobject /* this */) {
    engine.playSound = !engine.playSound;
    return 1;
}

// Update frequency parameter.
JNIEXPORT int JNICALL
Java_com_example_droidaudio_MainActivity_setFreq(
        JNIEnv* env,
        jobject /* this */,
        jfloat value,
        jint screenWidth) {
    // Map incoming screen x position between midi notes 33 (55.00hz - A1) and 57 (220.00hz - A3).
    // Midi note to frequency relationship also helps us convert to logarithmic space, which better matches human perceptual hearing.
    int midiNote = (int)((value / screenWidth) * (45.0f - 33.0f) + 33.0f);
    float newFreq = (440.0f / 32.0f) * std::pow(((float) midiNote - 9.0f) / 12.0f , 2.0f);
    engine.sawOsc.setFreq(newFreq);

//    __android_log_print(ANDROID_LOG_DEBUG, "Midi_Note", "%d\n", midiNote);
//    __android_log_print(ANDROID_LOG_DEBUG, "Osc_Freq", "%f\n", newFreq);
    return 1;
}

// Update filter cutoff parameter.
JNIEXPORT int JNICALL
Java_com_example_droidaudio_MainActivity_setFilterCutoff(
        JNIEnv* env,
        jobject /* this */,
        jfloat value,
        jint screenHeight) {
    float newCutoff = (value / screenHeight) * (0.5f - 0.0f);
    engine.filter.setCutoff(newCutoff);

//    __android_log_print(ANDROID_LOG_DEBUG, "Filter_Cutoff", "%f\n", newCutoff);
    return 1;
}

// Return spectrally generated hex color data.
JNIEXPORT jobjectArray JNICALL
Java_com_example_droidaudio_MainActivity_getSpectralHexes(
        JNIEnv* env,
        jobject /* this */) {
    jobjectArray returnable = env->NewObjectArray(3, env->FindClass("java/lang/String"), NULL);
    std::array<std::string, 3> cppArr = engine.getSpectralHexes();

    // __android_log_print(ANDROID_LOG_DEBUG, "Native_Hexes", "{ %s, %s, %s }\n", cppArr[0].c_str(), cppArr[1].c_str(), cppArr[2].c_str());

    for (int i = 0; i < cppArr.size(); ++i) {
        env->SetObjectArrayElement(returnable, i, env->NewStringUTF(cppArr[i].c_str()));
    }

    return returnable;
}

}