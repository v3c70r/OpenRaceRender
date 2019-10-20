// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "filedialog/filedialog.h"
#include <stdio.h>
#include <memory>
#include <string>


#include "logreader.h"
#include "logrender.h"
#include "videoplayermpv.h"

#include <GL/gl3w.h>    // Initialize with gl3wInit()

#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
static void* GetProcAddress(void* ctx, const char* name)
{
    GL3WglProc proc = gl3wGetProcAddress(name);
    return (void*)proc;
}

static const char TITLE[] = "title";
const char* GetTitle()
{
    return TITLE;
}

int main(int argc, char** argv)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenRaceRender", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;

    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    static ImGuiStyle style = ImGui::GetStyle();
    style.Alpha = 0.3;


    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.Fonts->AddFontDefault();
    ImFont* pRoboto = io.Fonts->AddFontFromFileTTF("../Roboto/Roboto-Bold.ttf", 16.0f);
    IM_ASSERT(pRoboto != NULL);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    /////////////////// Create my stuff
    PCSX::Widgets::FileDialog fileDialg(GetTitle);
    std::string sLogFile;
    std::string sVideoFile;
    std::unique_ptr<LogReader> pReader = nullptr;
    std::unique_ptr<LogRender> pRender = nullptr;
    VideoPlayerMPV videoPlayer;
    if (argc == 3)
    {
        sLogFile = argv[1];
        sVideoFile = argv[2];
        if (std::filesystem::exists(std::filesystem::path(sLogFile)))
        {
            pReader = std::make_unique<LogReader>(sLogFile);
            pRender = std::make_unique<LogRender>(*pReader);
        }
        if (std::filesystem::exists(std::filesystem::path(sVideoFile)))
        {
            videoPlayer.LoadVideo(sVideoFile);
        }
    }

    videoPlayer.InitGLContext(GetProcAddress);
    /////////////////// 
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushFont(pRoboto);

        ////////////////////////////////
        if (pReader == nullptr)
        {
            auto selectedFiles = fileDialg.selected();
            if (!selectedFiles.empty())
            {
                pReader = std::make_unique<LogReader>(selectedFiles[0]);
                pRender = std::make_unique<LogRender>(*pReader);
            }
            fileDialg.openDialog();
        }
        else
        {
            ImGui::Text("%s\n", pReader->GetDebugStr().c_str());
            pRender->DrawDataBox();
            static float fSeekTime = -1.0;
            fSeekTime = pRender->DrawTimeSlider();
            if (fSeekTime > 0.0)
            {
                videoPlayer.SetTime(fSeekTime);
            }
            pRender->DrawBasicInfoBox();
            pRender->DrawAcceBox();
            pRender->DrawMap();
            pRender->Update(1.0f / ImGui::GetIO().Framerate);
        }
        fileDialg.draw();

        static bool bIsPlaying;
        bIsPlaying = pRender->IsPlaying();
        videoPlayer.SetPlaying(bIsPlaying);
        if (show_demo_window)
        {
            ImGui::ShowMetricsWindow(&show_demo_window);
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        ImGui::PopFont();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        videoPlayer.Render(0, display_w, display_h); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    videoPlayer.DestroyGLContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
