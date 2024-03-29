cmake_minimum_required(VERSION 3.5)

project(Qt-Hand-Gesture-App LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#list(APPEND CMAKE_PREFIX_PATH "~/libtorch")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories (${Boost_INCLUDE_DIR})

link_directories(${OpenCV_LIB_DIR})
link_directories(${TORCH_LIBRARIES})

add_executable(Qt-Hand-Gesture-App
    main.cpp
    mainwindow.cpp
    mainwindow.h
    capture_thread.cpp
    capture_thread.h
    shared_memory_writer.h
    shared_memory_writer.cpp
    shared_memory_reader.cpp
    shared_memory_reader.h
    gesture_predictor.h
    gesture_predictor.cpp
    hand_detector_process_launcher.h
    hand_detector_process_launcher.cpp
    timer.h
    timer.cpp
    resources.h
    constants.h
    )

target_link_libraries(Qt-Hand-Gesture-App PRIVATE Qt5::Widgets
    ${TORCH_LIBRARIES}
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    librt.so
    )

set_property(TARGET Qt-Hand-Gesture-App PROPERTY CXX_STANDARD 17)
