cmake_minimum_required(VERSION 3.8)

project("SRT_listener_app")
add_subdirectory(srt)
include_directories(srt)
include_directories(srt/srtcore)
add_executable(SRT_listener_app "SRT_listener_app.cpp" "SRT_listener_app.h" ConfigFileReader.cpp ConfigFileReader.hpp ConfigReaderExceptions.h FIFO.h FIFO.cpp)
target_link_libraries(SRT_listener_app srt)