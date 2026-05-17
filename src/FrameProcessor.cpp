#include "FrameProcessor.hpp"
#include <string>
#include <vector>

FrameProcessor::FrameProcessor() : frameCount(0), fps(0.0) {
    lastTick = cv::getTickCount();
}

cv::Mat FrameProcessor::process(const cv::Mat& frame, KeyProcessor::Mode mode, int brightness) {
    cv::Mat result = frame.clone();
    frameCount++;

    int64 currentTick = cv::getTickCount();
    double timeSec = (currentTick - lastTick) / cv::getTickFrequency();
    if (timeSec >= 1.0) {
        fps = frameCount / timeSec;
        frameCount = 0;
        lastTick = currentTick;
    }

    result = result + cv::Scalar(brightness - 50, brightness - 50, brightness - 50);

    switch (mode) {
        case KeyProcessor::Mode::INVERSION:
            cv::bitwise_not(result, result);
            break;
        case KeyProcessor::Mode::GAUSSIAN_BLUR:
            cv::GaussianBlur(result, result, cv::Size(15, 15), 0);
            break;
        case KeyProcessor::Mode::CANNY_FILTER: {
            cv::Mat gray, edges;
            cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, edges, 50, 150);
            cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
            break;
        }
        case KeyProcessor::Mode::GLITCH_EFFECT: {
            std::vector<cv::Mat> channels;
            cv::split(result, channels);
            if (channels.size() >= 3) {
                cv::Mat transR = cv::Mat::zeros(channels[2].rows, channels[2].cols, channels[2].type());
                cv::Mat transB = cv::Mat::zeros(channels[0].rows, channels[0].cols, channels[0].type());
                channels[2](cv::Rect(15, 0, channels[2].cols - 15, channels[2].rows)).copyTo(transR(cv::Rect(0, 0, channels[2].cols - 15, channels[2].rows)));
                channels[0](cv::Rect(0, 0, channels[0].cols - 15, channels[0].rows)).copyTo(transB(cv::Rect(15, 0, channels[0].cols - 15, channels[0].rows)));
                channels[2] = transR;
                channels[0] = transB;
                cv::merge(channels, result);
            }
            break;
        }
        default:
            break;
    }

    std::string infoText = "FPS: " + std::to_string(static_cast<int>(fps)) + " | Mode: ";
    if (mode == KeyProcessor::Mode::NORMAL) infoText += "Normal";
    else if (mode == KeyProcessor::Mode::INVERSION) infoText += "Inversion";
    else if (mode == KeyProcessor::Mode::GAUSSIAN_BLUR) infoText += "Blur";
    else if (mode == KeyProcessor::Mode::CANNY_FILTER) infoText += "Canny";
    else if (mode == KeyProcessor::Mode::GLITCH_EFFECT) infoText += "Glitch";

    cv::putText(result, infoText, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    return result;
}
