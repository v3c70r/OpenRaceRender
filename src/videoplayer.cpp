#include "videoplayer.h"
static int g_refCount = 0;

bool VideoPlayer::LoadFile(std::string path)
{
    int ret = 0;
    ret = avformat_open_input(&m_pFormatContext, path.c_str(), nullptr, nullptr);
    if (ret < 0) return ret;
    ret = avformat_find_stream_info(m_pFormatContext, nullptr);
    if (ret < 0) return ret;

    // Check video stream
    if (OpenCodecContext(&m_nVideoStreamIndex, &m_pVideoCodecCtx, m_pFormatContext, AVMEDIA_TYPE_VIDEO) >= 0)
    {
        m_pVideoStream = m_pFormatContext->streams[m_nVideoStreamIndex];
        m_nWidth = m_pVideoCodecCtx->width;
        m_nHeight = m_pVideoCodecCtx->height;
        m_ePixelFormat = m_pVideoCodecCtx->pix_fmt;


    }
    
    return true;
}

void VideoPlayer::ShutDown()
{
    if (m_pFormatContext != nullptr)
    {
        avformat_close_input(&m_pFormatContext);
    }
}

int VideoPlayer::OpenCodecContext(int* streamIdx, AVCodecContext** codecCtx,
                                AVFormatContext* pFormatCtx, AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;
    ret = av_find_best_stream(pFormatCtx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        //fprintf(stderr, "Could not find %s stream in input file '%s'\n",
        //        av_get_media_type_string(type), src_filename);
        return ret;
    }
    else
    {
        stream_index = ret;
        st = pFormatCtx->streams[stream_index];
        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec)
        {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }
        /* Allocate a codec context for the decoder */
        *codecCtx = avcodec_alloc_context3(dec);
        if (!*codecCtx)
        {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }
        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*codecCtx, st->codecpar)) < 0)
        {
            fprintf(stderr,
                    "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }
        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", g_refCount ? "1" : "0", 0);
        if ((ret = avcodec_open2(*codecCtx, dec, &opts)) < 0)
        {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *streamIdx = stream_index;
    }
    return 0;
}
