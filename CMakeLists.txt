cmake_minimum_required(VERSION 3.10)
project(OpenCV_Lab7)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(opencv_lab7 
    src/main.cpp 
    src/CameraProvider.cpp 
    src/KeyProcessor.cpp 
    src/FrameProcessor.cpp
    src/FaceDetector.cpp
)

target_link_libraries(opencv_lab7 ${OpenCV_LIBS} pthread)
