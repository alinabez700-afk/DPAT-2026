#include "FrameProcessor.hpp"
#include <chrono>

FrameProcessor::FrameProcessor() {}

cv::Mat FrameProcessor::process(const cv::Mat& frame, KeyProcessor::Mode mode, int brightness) {
    if (frame.empty()) return frame;

    cv::Mat result;
    // Створюємо чисту копію, щоб не було накладання кадрів у пам'яті
    frame.copyTo(result);
    
    // 1. Регулювання яскравості
    double alpha = brightness / 50.0; 
    result.convertTo(result, -1, alpha, 0);

    // 2. Підрахунок FPS
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    double fps = 1000.0 / std::chrono::duration<double, std::milli>(currentTime - lastTime).count();
    lastTime = currentTime;

    // 3. Накладання ефектів (Суворо за умовою)
    if (mode == KeyProcessor::Mode::INVERSION) {
        cv::bitwise_not(result, result);
    }
    else if (mode == KeyProcessor::Mode::GAUSSIAN_BLUR) {
        cv::GaussianBlur(result, result, cv::Size(15, 15), 0);
    }
    else if (mode == KeyProcessor::Mode::CANNY_FILTER) {
        cv::Mat gray, edges;
        cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, edges, 50, 150);
        cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
    }
    else if (mode == KeyProcessor::Mode::GLITCH_EFFECT) {
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
    }

    // 4. Текст інтерфейсу
    std::string infoText = "FPS: " + std::to_string(static_cast<int>(fps)) + " | Mode: ";
    if (mode == KeyProcessor::Mode::NORMAL) infoText += "Normal";
    else if (mode == KeyProcessor::Mode::INVERSION) infoText += "Inversion";
    else if (mode == KeyProcessor::Mode::GAUSSIAN_BLUR) infoText += "Blur";
    else if (mode == KeyProcessor::Mode::CANNY_FILTER) infoText += "Canny";
    else if (mode == KeyProcessor::Mode::GLITCH_EFFECT) infoText += "Glitch";
    else if (mode == KeyProcessor::Mode::FACE_DETECTION) infoText += "Face Detection";

    cv::putText(result, infoText, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    
    return result;
}
