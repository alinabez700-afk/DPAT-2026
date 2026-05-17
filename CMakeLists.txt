cmake_minimum_required(VERSION 3.10)
project(OpenCV_Lab6)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(opencv_lab6 
    src/main.cpp 
    src/CameraProvider.cpp 
    src/KeyProcessor.cpp 
    src/FrameProcessor.cpp
)

target_link_libraries(opencv_lab6 ${OpenCV_LIBS})
