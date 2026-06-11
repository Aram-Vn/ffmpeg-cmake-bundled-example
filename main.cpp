#include <VideoInfo.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "  SimpleFFmpegApp <video-file>" << std::endl;
        return 0;
    }

    const std::string filePath = argv[1];

    if (!VideoInfo::printInfo(filePath))
    {
        std::cerr << "Failed to read video information." << std::endl;
        return 1;
    }

    return 0;
}