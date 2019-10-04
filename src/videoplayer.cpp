#include "videoplayer.h"
static int g_refCount = 0;

bool VideoPlayer::LoadFile(std::string path)
{
    int ret = 0;
    ret = avformat_open_input(&m_pFormatContext, path.c_str(), nullptr, nullptr);
    if (ret < 0) return false;
    ret = avformat_find_stream_info(m_pFormatContext, nullptr);
    if (ret < 0) return false;

    // Check video stream
    if (OpenCodecContext(&m_nVideoStreamIndex, &m_pVideoCodecCtx, m_pFormatContext, AVMEDIA_TYPE_VIDEO) >= 0)
    {
        m_pVideoStream = m_pFormatContext->streams[m_nVideoStreamIndex];
        m_nWidth = m_pVideoCodecCtx->width;
        m_nHeight = m_pVideoCodecCtx->height;
        m_ePixelFormat = m_pVideoCodecCtx->pix_fmt;
        ret = av_image_alloc(m_pVideoDstData, m_pVideoDstLinesize, m_nWidth, m_nHeight,
                             m_ePixelFormat, 1);
        if (ret < 0) return false;
        m_nVideoDstBufsize = ret;
    }

    // Check audio stream
    if (OpenCodecContext(&m_nAudioStreamIndex, &m_pAudioCodecCtx, m_pFormatContext, AVMEDIA_TYPE_VIDEO) >= 0)
    {
        m_pAudioStream = m_pFormatContext->streams[m_nAudioStreamIndex];
    }

    // Dmup format info
    av_dump_format(m_pFormatContext, 0, path.c_str(), 0);

    m_pFrame = av_frame_alloc();
    if (!m_pFrame) return false;
    av_init_packet(&m_packet);
    m_packet.data = nullptr;
    m_packet.size = 0;

    return true;
}

void VideoPlayer::ShutDown()
{
    if (m_pFormatContext != nullptr)
    {
        avformat_close_input(&m_pFormatContext);
    }
}

void VideoPlayer::ReadFrame()
{
    av_read_frame(m_pFormatContext, &m_packet);
    do
    {

    } while (m_packet.size > 0);
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

int VideoPlayer::DecodePacket(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = m_packet.size;
    *got_frame = 0;
    if (m_packet.stream_index == m_nVideoStreamIndex)
    {
        /* decode video frame */
        ret = avcodec_decode_video2(m_pVideoCodecCtx, m_pFrame, got_frame, &m_packet);
        if (ret < 0)
        {
            //fprintf(stderr, "Error decoding video frame (%s)\n",
            //        make_error_str(ret));
            return ret;
        }
        if (*got_frame)
        {
            if (m_pFrame->width != m_nWidth || m_pFrame->height != m_nHeight ||
                m_pFrame->format != m_ePixelFormat)
            {
                /* To handle this change, one could call av_image_alloc again
                 * and decode the following frames into another rawvideo file.
                 */
                fprintf(stderr,
                        "Error: Width, height and pixel format have to be "
                        "constant in a rawvideo file, but the width, height or "
                        "pixel format of the input video changed:\n"
                        "old: width = %d, height = %d, format = %s\n"
                        "new: width = %d, height = %d, format = %s\n",
                        m_nWidth, m_nHeight, av_get_pix_fmt_name(m_ePixelFormat),
                        m_pFrame->width, m_pFrame->height,
                        av_get_pix_fmt_name((AVPixelFormat)m_pFrame->format));
                return -1;
            }
            printf("video_frame%s n:%d coded_n:%d\n", cached ? "(cached)" : "",
                   m_nVideoFrameCount++, m_pFrame->coded_picture_number);
            /* copy decoded frame to destination buffer:
             * this is required since rawvideo expects non aligned data */
            av_image_copy(m_pVideoDstData, m_pVideoDstLinesize,
                          (const uint8_t **)(m_pFrame->data), m_pFrame->linesize,
                          m_ePixelFormat, m_nWidth, m_nHeight);
            /* write to rawvideo file */
            //fwrite(m_pVideoDstData[0], 1, m_nVideoDstBufsize, video_dst_file);
        }
    }
    else if (m_packet.stream_index == m_nAudioStreamIndex)
    {
        /* decode audio m_pFrame */
        ret = avcodec_decode_audio4(m_pAudioCodecCtx, m_pFrame, got_frame, &m_packet);
        if (ret < 0)
        {
            //fprintf(stderr, "Error decoding audio m_pFrame (%s)\n",
            //        make_error_str(ret));
            return ret;
        }
        /* Some audio decoders decode only part of the packet, and have to be
         * called again with the remainder of the packet data.
         * Sample: fate-suite/lossless-audio/luckynight-partial.shn
         * Also, some decoders might over-read the packet. */
        decoded = FFMIN(ret, m_packet.size);
        if (*got_frame)
        {
            size_t unpadded_linesize =
                m_pFrame->nb_samples *
                av_get_bytes_per_sample((AVSampleFormat)m_pFrame->format);
            printf("audio_frame%s n:%d nb_samples:%d",  // pts:%s\n",
                   cached ? "(cached)" : "", m_nAudioFrameCount++,
                   m_pFrame->nb_samples
                   // av_ts2timestr(m_pFrame->pts, &audio_dec_ctx->time_base)
            );
            /* Write the raw audio data samples of the first plane. This works
             * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
             * most audio decoders output planar audio, which uses a separate
             * plane of audio samples for each channel (e.g.
             * AV_SAMPLE_FMT_S16P). In other words, this code will write only
             * the first audio channel in these cases. You should use
             * libswresample or libavfilter to convert the frame
             * to packed data. */
            //fwrite(m_pFrame->extended_data[0], 1, unpadded_linesize,
            //       audio_dst_file);
        }
    }
    /* If we use frame reference counting, we own the data and need
     * to de-reference it when we don't use it anymore */
    if (*got_frame && g_refCount) av_frame_unref(m_pFrame);
    return decoded;
}
