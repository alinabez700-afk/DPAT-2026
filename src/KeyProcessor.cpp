#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(Mode::NORMAL), brightnessSlider(50) {}

void KeyProcessor::processKey(int key) {
    if (key == -1) return;
    switch (key) {
        case '0': currentMode = Mode::NORMAL; break;
        case '1': currentMode = Mode::INVERSION; break;
        case '2': currentMode = Mode::GAUSSIAN_BLUR; break;
        case '3': currentMode = Mode::CANNY_FILTER; break;
        case '4': currentMode = Mode::GLITCH_EFFECT; break;
        case '5': case 'f': case 'F': currentMode = Mode::FACE_DETECTION; break;
    }
}

KeyProcessor::Mode KeyProcessor::getMode() const { 
    return currentMode; 
}

int KeyProcessor::getBrightness() const { 
    return brightnessSlider; 
}

void KeyProcessor::setBrightness(int value) { 
    brightnessSlider = value; 
}
