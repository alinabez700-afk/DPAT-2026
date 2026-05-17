#include "FaceDetector.hpp"
#include <iostream>

FaceDetector::FaceDetector(const std::string& modelProto, const std::string& modelBinary) 
    : isRunning(false), hasNewFrame(false) {
    try {
        net = cv::dnn::readNetFromCaffe(modelProto, modelBinary);
    } catch (const cv::Exception& e) {
        std::cerr << "Error loading network: " << e.what() << std::endl;
    }
}

FaceDetector::~FaceDetector() {
    stop();
}

void FaceDetector::start() {
    if (isRunning) return;
    isRunning = true;
    workerThread = std::thread(&FaceDetector::detectLoop, this);
}

void FaceDetector::stop() {
    if (!isRunning) return;
    isRunning = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(frameMutex);
    frame.copyTo(sharedFrame);
    hasNewFrame = true;
}

std::vector<cv::Rect> FaceDetector::getFaces() {
    std::lock_guard<std::mutex> lock(frameMutex);
    return sharedDetectedFaces;
}

void FaceDetector::detectLoop() {
    while (isRunning) {
        cv::Mat localFrame;
        
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            if (hasNewFrame && !sharedFrame.empty()) {
                sharedFrame.copyTo(localFrame);
                hasNewFrame = false;
            }
        }

        if (localFrame.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        cv::Mat blob = cv::dnn::blobFromImage(localFrame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false);
        net.setInput(blob);
        cv::Mat detection = net.forward();

        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        std::vector<cv::Rect> localFaces;

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > 0.5) {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * localFrame.cols);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * localFrame.rows);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * localFrame.cols);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * localFrame.rows);
                
                x1 = std::max(0, std::min(x1, localFrame.cols - 1));
                y1 = std::max(0, std::min(y1, localFrame.rows - 1));
                x2 = std::max(0, std::min(x2, localFrame.cols - 1));
                y2 = std::max(0, std::min(y2, localFrame.rows - 1));

                localFaces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
            }
        }

        {
            std::lock_guard<std::mutex> lock(frameMutex);
            sharedDetectedFaces = localFaces;
        }
    }
}
