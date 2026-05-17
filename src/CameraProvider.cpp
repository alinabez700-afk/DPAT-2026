#include "CameraProvider.hpp"

CameraProvider::CameraProvider(int deviceId) {
    // Відкриваємо камеру з автоматичним вибором найкращого бекенду (або V4L2 в Linux)
    cap.open(deviceId);
    
    if (cap.isOpened()) {
        // Замість жорсткого кодека MJPEG встановлюємо оптимальне розширення,
        // щоб відеопотік стабільно проходив крізь міст VMware без артефактів
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    }
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
