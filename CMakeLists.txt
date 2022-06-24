cmake_minimum_required(VERSION 3.8)

project("SRT_listener_app")
add_subdirectory(srt)
include_directories(srt)
include_directories(srt/srtcore)
add_executable(SRT_listener_app "SRT_listener_app.cpp" "Listener.hpp" ConfigFileReader.cpp ConfigFileReader.hpp ConfigReaderExceptions.hpp Listener.cpp ListenerException.hpp Server.cpp Server.hpp SRT_server_app.cpp ServerException.hpp)
target_link_libraries(SRT_listener_app srt)