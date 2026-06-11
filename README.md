# FFmpeg CMake Bundled Example

A simple C++20 project showing how to link FFmpeg with CMake on Windows using a bundled FFmpeg package.

The goal of this repository is to demonstrate a clean project structure where the FFmpeg headers, import libraries, and runtime DLLs are kept together inside the project.

This makes the project easier to build, move, and share without requiring a global FFmpeg installation.

> **Note:** This repository uses Git LFS because some bundled FFmpeg DLL files are large.
> After cloning, make sure Git LFS is installed and run `git lfs pull`.

## Clone Instructions

Install Git LFS first:

```bash
git lfs install
```

Clone the repository:

```bash
git clone git@github.com:Aram-Vn/ffmpeg-cmake-bundled-example.git
cd ffmpeg-cmake-bundled-example
```

Pull the Git LFS files:

```bash
git lfs pull
```

After cloning, the bundled FFmpeg dependency should exist here:

```text
FF_VideoInfo/extern/bin
FF_VideoInfo/extern/include
FF_VideoInfo/extern/lib
```

If the `bin`, `include`, or `lib` folders are missing or contain small Git LFS pointer files instead of real binaries, run:

```bash
git lfs pull
```

## Project Structure

```text
SimpleFFmpegApp
│   CMakeLists.txt
│   main.cpp
│
└───FF_VideoInfo
    │   CMakeLists.txt
    │   VideoInfo.cpp
    │   VideoInfo.h
    │
    └───extern
        ├───bin
        ├───include
        └───lib
```

## What This Project Demonstrates

This project demonstrates:

* how to download and organize FFmpeg for C++ development
* how to link FFmpeg with CMake
* how to support MinGW and MSVC import libraries
* how to copy FFmpeg runtime DLLs after build
* how to keep third-party dependency logic inside a separate CMake module
* how to create a reusable `FF_VideoInfo` module

## FFmpeg Folder Layout

FFmpeg is stored inside:

```text
FF_VideoInfo/extern
```

The expected layout is:

```text
extern
├── bin
├── include
└── lib
```

Meaning:

```text
bin      -> FFmpeg runtime DLL files
include  -> FFmpeg header files
lib      -> FFmpeg import libraries
```

For MinGW, the `lib` folder should contain files like:

```text
libavformat.dll.a
libavcodec.dll.a
libavutil.dll.a
```

For MSVC, the `lib` folder should contain files like:

```text
avformat.lib
avcodec.lib
avutil.lib
```

The `bin` folder should contain runtime DLLs like:

```text
avformat-62.dll
avcodec-62.dll
avutil-60.dll
swscale-9.dll
swresample-6.dll
```

## Downloading FFmpeg

Download FFmpeg from:

```text
https://www.gyan.dev/ffmpeg/builds/
```

For this example, use the shared build that contains development files:

```text
ffmpeg-release-full-shared.7z
```

After extracting it, copy the needed folders into:

```text
FF_VideoInfo/extern
```

So the final result should be:

```text
FF_VideoInfo/extern/bin
FF_VideoInfo/extern/include
FF_VideoInfo/extern/lib
```

## Building

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

Then run:

```bash
./build/SimpleFFmpegApp.exe path/to/video.mp4
```

With Visual Studio generators, the executable may be inside:

```text
build/Debug
```

or:

```text
build/Release
```

## Example Usage

```bash
./SimpleFFmpegApp.exe test.mp4
```

Example output:

```text
File: test.mp4
Duration: 12.53 seconds
Streams: 2
Stream #0: Video, 1920x1080, codec: h264
Stream #1: Audio, 48000 Hz, codec: aac
```

## Why Bundle FFmpeg Inside the Project?

Instead of depending on a system-wide FFmpeg installation, this project keeps FFmpeg inside the repository structure.

This has several advantages:

* the project is easier to move between machines
* the CMake paths are predictable
* the correct FFmpeg version is used every time
* the build does not depend on the user's global `PATH`
* runtime DLLs can be copied automatically after build

The main idea is:

```text
include -> needed at compile time
lib     -> needed at link time
bin     -> needed at runtime
```

## CMake Design

The root `CMakeLists.txt` stays simple:

```cmake
add_subdirectory(FF_VideoInfo)

add_executable(SimpleFFmpegApp
    main.cpp
)

target_link_libraries(SimpleFFmpegApp
    PRIVATE
        FF_VideoInfo
)

ff_video_info_copy_runtime(SimpleFFmpegApp)
```

All FFmpeg-specific logic is inside:

```text
FF_VideoInfo/CMakeLists.txt
```

The module handles:

* FFmpeg path setup
* Windows detection
* MinGW vs MSVC library selection
* include directories
* link libraries
* required Windows system libraries
* runtime DLL copying

## Large Files and Git LFS

This repository bundles FFmpeg inside:

```text
FF_VideoInfo/extern
```

Some FFmpeg DLL files are large, so this repository uses Git LFS.

The large binary files are tracked with Git LFS using patterns like:

```bash
git lfs track "FF_VideoInfo/extern/bin/*.dll"
git lfs track "FF_VideoInfo/extern/bin/*.exe"
git lfs track "FF_VideoInfo/extern/lib/*.lib"
git lfs track "FF_VideoInfo/extern/lib/*.dll.a"
git lfs track "FF_VideoInfo/extern/lib/*.def"
```

After cloning the repository, install Git LFS and pull the dependency files:

```bash
git lfs install
git lfs pull
```

## Common Runtime Error

If you see an error like:

```text
error while loading shared libraries: avformat-62.dll: cannot open shared object file: No such file or directory
```

it means the application linked successfully, but the FFmpeg runtime DLLs were not found.

The DLL files must be next to the final executable or available in `PATH`.

This project solves that by copying FFmpeg DLLs after build using a CMake helper function:

```cmake
ff_video_info_copy_runtime(SimpleFFmpegApp)
```

## Notes

This repository is intended as a small learning example.

It does not implement a full video player. It only reads basic video information using FFmpeg APIs.

The main purpose is to show a clean and reusable way to organize FFmpeg dependencies in a C++/CMake project.
