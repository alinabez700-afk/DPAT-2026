#include <iostream>
#include <opencv2/opencv.hpp>
#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"

void on_trackbar(int val, void* userdata) {
    KeyProcessor* kp = static_cast<KeyProcessor*>(userdata);
    kp->setBrightness(val);
}

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Error: Camera not found!" << std::endl;
        return -1;
    }

    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;

    std::string winName = "Lab 6: OpenCV Processing";
    cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);

    int initialSliderVal = keyProcessor.getBrightness();
    cv::createTrackbar("Brightness", winName, &initialSliderVal, 100, on_trackbar, &keyProcessor);

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) {
            break;
        }

        cv::Mat processedFrame = frameProcessor.process(frame, keyProcessor.getMode(), keyProcessor.getBrightness());
        cv::imshow(winName, processedFrame);

        int key = cv::waitKey(30);
        if (key == 27) {
            break;
        }
        keyProcessor.processKey(key);
    }

    cv::destroyAllWindows();
    return 0;
}
