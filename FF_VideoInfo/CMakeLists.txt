set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Capture this module directory.
set(FF_VIDEO_INFO_DIR "${CMAKE_CURRENT_LIST_DIR}")

message(STATUS "------------------------------------------------------------")
message(STATUS "[FF_VideoInfo] Configuring FF_VideoInfo module")
message(STATUS "[FF_VideoInfo] Module dir: ${FF_VIDEO_INFO_DIR}")
message(STATUS "------------------------------------------------------------")

# -------------------- FFmpeg selection -------------------- #
if(WIN32)
    message(STATUS "[FF_VideoInfo] Platform: Windows")

    set(FFMPEG_ROOT "${FF_VIDEO_INFO_DIR}/extern")
    set(FFMPEG_INC  "${FFMPEG_ROOT}/include")
    set(FFMPEG_LIB  "${FFMPEG_ROOT}/lib")
    set(FFMPEG_DLL_DIR "${FFMPEG_ROOT}/bin")

    # Export paths so parent/root CMake can also reuse them if needed.
    set(FFMPEG_ROOT    "${FFMPEG_ROOT}"    CACHE INTERNAL "FFmpeg bundle root" FORCE)
    set(FFMPEG_INC     "${FFMPEG_INC}"     CACHE INTERNAL "FFmpeg include dir" FORCE)
    set(FFMPEG_LIB     "${FFMPEG_LIB}"     CACHE INTERNAL "FFmpeg lib dir" FORCE)
    set(FFMPEG_DLL_DIR "${FFMPEG_DLL_DIR}" CACHE INTERNAL "FFmpeg DLL dir" FORCE)

    message(STATUS "[FF_VideoInfo] FFMPEG_ROOT:    ${FFMPEG_ROOT}")
    message(STATUS "[FF_VideoInfo] FFMPEG_INC:     ${FFMPEG_INC}")
    message(STATUS "[FF_VideoInfo] FFMPEG_LIB:     ${FFMPEG_LIB}")
    message(STATUS "[FF_VideoInfo] FFMPEG_DLL_DIR: ${FFMPEG_DLL_DIR}")

    if(EXISTS "${FFMPEG_INC}")
        message(STATUS "[FF_VideoInfo] [OK] Found FFmpeg headers: ${FFMPEG_INC}")
    else()
        message(FATAL_ERROR "[FF_VideoInfo] [ERROR] FFmpeg include directory not found: ${FFMPEG_INC}")
    endif()

    if(EXISTS "${FFMPEG_LIB}")
        message(STATUS "[FF_VideoInfo] [OK] Found FFmpeg lib directory: ${FFMPEG_LIB}")
    else()
        message(FATAL_ERROR "[FF_VideoInfo] [ERROR] FFmpeg lib directory not found: ${FFMPEG_LIB}")
    endif()

    if(EXISTS "${FFMPEG_DLL_DIR}")
        message(STATUS "[FF_VideoInfo] [OK] Found FFmpeg DLL directory: ${FFMPEG_DLL_DIR}")
    else()
        message(FATAL_ERROR "[FF_VideoInfo] [ERROR] FFmpeg DLL directory not found: ${FFMPEG_DLL_DIR}")
    endif()

    if(MINGW)
        message(STATUS "[FF_VideoInfo] Compiler: MinGW")
        message(STATUS "[FF_VideoInfo] Using .dll.a import libraries")

        set(FFMPEG_LIBS
            "${FFMPEG_LIB}/libavcodec.dll.a"
            "${FFMPEG_LIB}/libavdevice.dll.a"
            "${FFMPEG_LIB}/libavformat.dll.a"
            "${FFMPEG_LIB}/libavutil.dll.a"
            "${FFMPEG_LIB}/libswresample.dll.a"
            "${FFMPEG_LIB}/libswscale.dll.a"
        )
    else()
        message(STATUS "[FF_VideoInfo] Compiler: MSVC or other Windows compiler")
        message(STATUS "[FF_VideoInfo] Using .lib import libraries")

        set(FFMPEG_LIBS
            "${FFMPEG_LIB}/avcodec.lib"
            "${FFMPEG_LIB}/avdevice.lib"
            "${FFMPEG_LIB}/avformat.lib"
            "${FFMPEG_LIB}/avutil.lib"
            "${FFMPEG_LIB}/swresample.lib"
            "${FFMPEG_LIB}/swscale.lib"
        )
    endif()

    foreach(FFMPEG_LIB_FILE ${FFMPEG_LIBS})
        if(EXISTS "${FFMPEG_LIB_FILE}")
            message(STATUS "[FF_VideoInfo] [OK] Found library: ${FFMPEG_LIB_FILE}")
        else()
            message(FATAL_ERROR "[FF_VideoInfo] [ERROR] Missing library: ${FFMPEG_LIB_FILE}")
        endif()
    endforeach()

else()
    message(STATUS "[FF_VideoInfo] Platform: Linux/macOS")
    message(STATUS "[FF_VideoInfo] Searching FFmpeg using pkg-config")

    find_package(PkgConfig QUIET)

    if(NOT PKG_CONFIG_FOUND)
        message(FATAL_ERROR "[FF_VideoInfo] pkg-config is required to find FFmpeg.")
    endif()

    pkg_check_modules(FFMPEG QUIET IMPORTED_TARGET
        libavcodec
        libavdevice
        libavformat
        libavutil
        libswresample
        libswscale
    )

    if(NOT FFMPEG_FOUND)
        message(FATAL_ERROR "[FF_VideoInfo] FFmpeg dev packages not found via pkg-config.")
    endif()

    set(FFMPEG_INC "${FFMPEG_INCLUDE_DIRS}")
    set(FFMPEG_LIBS PkgConfig::FFMPEG)

    message(STATUS "[FF_VideoInfo] [OK] Found FFmpeg using pkg-config")
endif()

# -------------------- Module library -------------------- #
add_library(FF_VideoInfo STATIC
    VideoInfo.cpp
    VideoInfo.h
)

target_include_directories(FF_VideoInfo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${FFMPEG_INC}
)

target_link_libraries(FF_VideoInfo
    PUBLIC
        ${FFMPEG_LIBS}
)

if(WIN32)
    target_link_libraries(FF_VideoInfo
        PUBLIC
            ws2_32
            bcrypt
            secur32
            avrt
            shlwapi
    )

    if(MINGW)
        target_link_libraries(FF_VideoInfo
            PUBLIC
                winmm
        )
    endif()
endif()

message(STATUS "[FF_VideoInfo] [OK] FF_VideoInfo static library configured")

# -------------------- Runtime copy helper -------------------- #
function(ff_video_info_copy_runtime TARGET_NAME)
    if(NOT WIN32)
        message(STATUS "[FF_VideoInfo] Runtime copy skipped: not Windows")
        return()
    endif()

    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "[FF_VideoInfo] ff_video_info_copy_runtime called for unknown target: ${TARGET_NAME}")
    endif()

    if(NOT EXISTS "${FFMPEG_DLL_DIR}")
        message(FATAL_ERROR "[FF_VideoInfo] FFmpeg DLL dir not found: ${FFMPEG_DLL_DIR}")
    endif()

    message(STATUS "[FF_VideoInfo] Runtime DLL copy enabled for target: ${TARGET_NAME}")
    message(STATUS "[FF_VideoInfo] Runtime source dir: ${FFMPEG_DLL_DIR}")

    set(FFMPEG_DLLS
        avcodec-62.dll
        avdevice-62.dll
        avfilter-11.dll
        avformat-62.dll
        avutil-60.dll
        swresample-6.dll
        swscale-9.dll
    )

    foreach(DLL ${FFMPEG_DLLS})
        set(DLL_PATH "${FFMPEG_DLL_DIR}/${DLL}")

        if(EXISTS "${DLL_PATH}")
            message(STATUS "[FF_VideoInfo] [OK] Will copy runtime DLL: ${DLL}")

            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL_PATH}"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/${DLL}"
                COMMENT "[FF_VideoInfo] Copying ${DLL} to output directory"
            )
        else()
            message(WARNING "[FF_VideoInfo] FFmpeg DLL not found: ${DLL_PATH}")
        endif()
    endforeach()
endfunction()