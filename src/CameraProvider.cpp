#include "CameraProvider.hpp"

CameraProvider::CameraProvider(int deviceId) {
    cap.open(deviceId);
cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(cv::CAP_PROP_CONVERT_RGB, 1);
}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) {
        cap.release();
    }
}

bool CameraProvider::isOpened() const {
    return cap.isOpened();
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    if (cap.isOpened()) {
        cap.read(frame);
    }
    return frame;
}
