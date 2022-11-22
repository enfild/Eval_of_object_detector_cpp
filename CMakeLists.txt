cmake_minimum_required(VERSION 3.16)
project(eval_detector CXX)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_executable(eval_detector 
    main.cxx 
    eval_detector.cxx 
    eval_detector.h)