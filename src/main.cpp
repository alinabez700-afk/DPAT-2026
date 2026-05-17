#include <iostream>
#include <opencv2/opencv.hpp>
#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "FaceDetector.hpp"

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

    FaceDetector detector("models/deploy.prototxt", "models/res10_300x300_ssd_iter_140000.caffemodel");
    detector.start();

    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;

    std::string winName = "Lab 7: Multi-threaded Face Detection";
    cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);

    int initialSliderVal = keyProcessor.getBrightness();
    cv::createTrackbar("Brightness", winName, &initialSliderVal, 100, on_trackbar, &keyProcessor);

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;

        detector.updateFrame(frame);
        cv::Mat processedFrame = frameProcessor.process(frame, keyProcessor.getMode(), keyProcessor.getBrightness());

        if (keyProcessor.getMode() == KeyProcessor::Mode::FACE_DETECTION) {
            std::vector<cv::Rect> faces = detector.getFaces();
            for (const auto& rect : faces) {
                cv::rectangle(processedFrame, rect, cv::Scalar(0, 0, 255), 3);
                cv::putText(processedFrame, "Face", cv::Point(rect.x, rect.y - 10), 
                            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
            }
        }

        cv::imshow(winName, processedFrame);

        int key = cv::waitKey(30);
        if (key == 27) break;
        keyProcessor.processKey(key);
    }

    detector.stop();
    cv::destroyAllWindows();
    return 0;
}
