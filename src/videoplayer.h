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
    void ReadFrame();
    std::string GetDebugStr()
    {
        std::stringstream ss;
        ss<<"Duration: "<<m_pFormatContext->duration<<std::endl;
        return ss.str();
    }
private:
    int OpenCodecContext(int* streamIdx, AVCodecContext** codecCtx, AVFormatContext* formatCtx, AVMediaType type);
    int DecodePacket(int *GotFrame, int cached);
private:
    AVFormatContext* m_pFormatContext = nullptr;

    // Video stream info
    AVCodecContext* m_pVideoCodecCtx = nullptr;
    AVStream* m_pVideoStream = nullptr;
    AVPixelFormat m_ePixelFormat =  AV_PIX_FMT_NONE;
    int m_pVideoDstLinesize[4];
    uint8_t* m_pVideoDstData[4] = {NULL};
    size_t m_nVideoDstBufsize;
    int m_nVideoStreamIndex = -1;
    int m_nWidth = 0;
    int m_nHeight = 0;

    // Audio stream info
    AVCodecContext* m_pAudioCodecCtx = nullptr;
    AVStream* m_pAudioStream = nullptr;
    int m_nAudioStreamIndex = -1;

    // Decoder info
    AVFrame* m_pFrame = nullptr;
    AVPacket m_packet;

    int m_nVideoFrameCount = 0;
    int m_nAudioFrameCount = 0;
};
