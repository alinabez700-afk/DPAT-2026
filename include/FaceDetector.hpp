#ifndef FACE_DETECTOR_HPP
#define FACE_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

class FaceDetector {
private:
    cv::dnn::Net net;
    std::thread workerThread;
    std::mutex frameMutex;
    
    cv::Mat sharedFrame;
    std::vector<cv::Rect> sharedDetectedFaces;
    
    std::atomic<bool> isRunning;
    std::atomic<bool> hasNewFrame;

    void detectLoop();

public:
    FaceDetector(const std::string& modelProto, const std::string& modelBinary);
    ~FaceDetector();

    void start();
    void stop();
    void updateFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getFaces();
};

#endif
