cmake_minimum_required(VERSION 3.20)

project(SimpleFFmpegApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(FF_VideoInfo)

add_executable(SimpleFFmpegApp
    main.cpp
)

target_link_libraries(SimpleFFmpegApp PRIVATE
    FF_VideoInfo
)

ff_video_info_copy_runtime(SimpleFFmpegApp)