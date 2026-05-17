#ifndef KEY_PROCESSOR_HPP
#define KEY_PROCESSOR_HPP

class KeyProcessor {
public:
    enum class Mode {
        NORMAL,
        INVERSION,
        GAUSSIAN_BLUR,
        CANNY_FILTER,
        GLITCH_EFFECT
    };

private:
    Mode currentMode;
    int brightnessSlider;

public:
    KeyProcessor();
    void processKey(int key);
    Mode getMode() const;
    int getBrightness() const;
    void setBrightness(int value);
};

#endif
