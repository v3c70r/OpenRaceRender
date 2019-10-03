#pragma once
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
}
#include <string>
#include <sstream>

class VideoPlayer
{
public:
    VideoPlayer() {}
    bool LoadFile(std::string path);
    void ShutDown();
    std::string GetDebugStr()
    {
        std::stringstream ss;
        ss<<"Duration: "<<m_pFormatContext->duration<<std::endl;
        return ss.str();
    }
private:
    int OpenCodecContext(int* streamIdx, AVCodecContext** codecCtx, AVFormatContext* formatCtx, AVMediaType type);
private:
    AVFormatContext* m_pFormatContext = nullptr;
    AVCodecContext* m_pVideoCodecCtx = nullptr;
    AVCodecContext* m_pAudioCodecCtx = nullptr;
    AVStream* m_pVideoStream = nullptr;
    AVStream* m_pAudioStream = NULL;
    AVPixelFormat m_ePixelFormat =  AV_PIX_FMT_NONE;

    int m_nVideoStreamIndex = -1;
    int m_nAudioStreamIndex = -1;
    
    int m_nWidth = 0;
    int m_nHeight = 0;
};
