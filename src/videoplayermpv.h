#pragma once

extern "C"{
#include <mpv/client.h>
#include <mpv/render_gl.h>
}
#include <string>
#include <cassert>

#include <iostream>

class VideoPlayerMPV
{
public:
    VideoPlayerMPV()
    {
        m_pHandle = mpv_create();
        assert(m_pHandle);
    }
    ~VideoPlayerMPV()
    {
        mpv_terminate_destroy(m_pHandle);
    }

    // Initialize OpenGL Context
    void InitGLContext(void* (*GetProcAddress)(void*, const char*))
    {
        // See https://github.com/mpv-player/mpv-examples/blob/master/libmpv/qt_opengl/mpvwidget.cpp
        mpv_opengl_init_params gl_init_params{GetProcAddress, nullptr, nullptr};
        mpv_render_param params[]{
            {MPV_RENDER_PARAM_API_TYPE,
             const_cast<char*>(MPV_RENDER_API_TYPE_OPENGL)},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
            {MPV_RENDER_PARAM_INVALID, nullptr}};
        mpv_render_context_create(&m_pRenderContext, m_pHandle, params);
    }

    void DestroyGLContext()
    {
       mpv_render_context_free(m_pRenderContext); 
    }

    void Render(int nFbo, int nWidth, int nHeight)
    {

        if (!m_bIsVideoLoaded) return;
        if (!m_bIsPlaying) return;

        mpv_opengl_fbo fbo{nFbo, nWidth, nHeight, 0};
        int flip_y{1};

        mpv_render_param params[] = {{MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
                                     {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
                                     {MPV_RENDER_PARAM_INVALID, nullptr}};

        assert(mpv_render_context_render(m_pRenderContext, params) >= 0);
    }

    void SetTime(float time)
    {
        if (!m_bIsVideoLoaded) return;

        char strTime[24];
        // TODO: Handle seek bound check
        if (time < 99999)
        {
            sprintf(strTime, "%d", int(time));
            const char* cmd[] = {"seek", strTime, "absolute", nullptr};
            assert(mpv_command(m_pHandle, cmd) >= 0);
        }
    }

    void TogglePlaying()
    {
        m_bIsPlaying = !m_bIsPlaying;
    }

    void SetPlaying(bool bPlaying)
    {
        m_bIsPlaying = bPlaying;
    }

    void LoadVideo(std::string path)
    {
        //int val = 1;
        assert(mpv_initialize(m_pHandle) >= 0);
        const char* cmd[] = {"loadfile", path.c_str(), nullptr};
        assert(mpv_command(m_pHandle, cmd) >= 0);
        m_bIsVideoLoaded = true;

        // TODO: Load video infos
        //int res = -1;
        //while (res < 0)
        //{
        //    res = mpv_get_property(m_pHandle, "duration", MPV_FORMAT_DOUBLE,
        //                           &m_fDuration);
        //    std::cout << res << std::endl;
        //}
    }

private:
    mpv_handle* m_pHandle = nullptr;
    mpv_render_context* m_pRenderContext = nullptr;
    bool m_bIsPlaying = false;
    bool m_bIsVideoLoaded = false;

    // status
    float m_fDuration = 0.0;
    
};
