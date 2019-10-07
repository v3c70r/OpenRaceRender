#pragma once

extern "C"{
#include <mpv/client.h>
#include <mpv/render_gl.h>
}
#include <string>
#include <cassert>

class VideoPlayerMPV
{
public:
    VideoPlayerMPV() : m_fTime(0.0)
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
        // TODO: Pass in get proc address callback here
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
        mpv_opengl_fbo fbo{nFbo, nWidth, nHeight, 0};
        int flip_y{1};

        mpv_render_param params[] = {{MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
                                     {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
                                     {MPV_RENDER_PARAM_INVALID, nullptr}};

        assert(mpv_render_context_render(m_pRenderContext, params) >= 0);
    }

    void LoadVideo(std::string path)
    {
        (mpv_set_option_string(m_pHandle, "input-default-bindings", "yes"));
        mpv_set_option_string(m_pHandle, "input-vo-keyboard", "yes");
        int val = 1;
        (mpv_set_option(m_pHandle, "osc", MPV_FORMAT_FLAG, &val));
        (mpv_initialize(m_pHandle));
        const char* cmd[] = {"loadfile", path.c_str(), NULL};
        assert(mpv_command(m_pHandle, cmd) >= 0);
    }

private:
    mpv_handle* m_pHandle = nullptr;
    mpv_render_context* m_pRenderContext = nullptr;
};
