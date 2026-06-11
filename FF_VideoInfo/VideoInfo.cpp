#include "VideoInfo.h"

#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

}

bool VideoInfo::printInfo(const std::string& filePath)
{
    AVFormatContext* formatContext = nullptr;

    const int openResult = avformat_open_input(&formatContext, filePath.c_str(), nullptr, nullptr);

    if (openResult < 0)
    {
        std::cerr << "Could not open file: " << filePath << std::endl;
        return false;
    }

    const int streamInfoResult = avformat_find_stream_info(formatContext, nullptr);

    if (streamInfoResult < 0)
    {
        std::cerr << "Could not read stream information." << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }

    std::cout << "File: " << filePath << std::endl;

    if (formatContext->duration != AV_NOPTS_VALUE)
    {
        const double durationSeconds = static_cast<double>(formatContext->duration) / AV_TIME_BASE;

        std::cout << "Duration: " << durationSeconds << " seconds" << std::endl;
    }
    else
    {
        std::cout << "Duration: unknown" << std::endl;
    }

    std::cout << "Streams: " << formatContext->nb_streams << std::endl;

    for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
    {
        const AVStream*          stream      = formatContext->streams[i];
        const AVCodecParameters* codecParams = stream->codecpar;

        std::cout << "Stream #" << i << ": ";

        if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            std::cout << "Video, " << codecParams->width << "x" << codecParams->height;
        }
        else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            std::cout << "Audio, " << codecParams->sample_rate << " Hz";
        }
        else
        {
            std::cout << "Other";
        }

        const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);

        if (codec)
        {
            std::cout << ", codec: " << codec->name;
        }

        std::cout << std::endl;
    }

    avformat_close_input(&formatContext);
    return true;
}