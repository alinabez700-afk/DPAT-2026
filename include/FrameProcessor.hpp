#ifndef FRAME_PROCESSOR_HPP
#define FRAME_PROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
private:
    int frameCount;
    double fps;
    int64 lastTick;

public:
    FrameProcessor();
    cv::Mat process(const cv::Mat& frame, KeyProcessor::Mode mode, int brightness);
};

#endif
