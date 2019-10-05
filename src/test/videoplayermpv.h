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
    //VideoPlayerMPV();

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
    void LoadVideo(std::string path)
    {
        m_pHandle = mpv_create();
        assert(m_pHandle);
        (mpv_set_option_string(m_pHandle, "input-default-bindings", "yes"));
        mpv_set_option_string(m_pHandle, "input-vo-keyboard", "yes");
        int val = 1;
        (mpv_set_option(m_pHandle, "osc", MPV_FORMAT_FLAG, &val));
        (mpv_initialize(m_pHandle));
        const char* cmd[] = {"loadfile", path.c_str(), NULL};
        (mpv_command(m_pHandle, cmd));
    }

private:
    mpv_handle* m_pHandle = nullptr;
    mpv_render_context* m_pRenderContext = nullptr;
};
