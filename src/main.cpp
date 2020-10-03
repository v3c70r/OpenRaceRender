// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImFileBrowser/imfilebrowser.h"
#include <filesystem>
#include <stdio.h>
#include <memory>
#include <string>
#include "widget.h"


#include "logreader.h"
#include "logrender.h"
#include "debugrender.h"

#include <GL/gl3w.h>    // Initialize with gl3wInit()

#include <GLFW/glfw3.h>
#include <stb_image_write.h>
#include <cmath>

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
    glfwSwapInterval(0); // Enable vsync

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
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

    /////////////////// Create my stuff
    std::string sLogFile;
    std::string sVideoFile;
    std::unique_ptr<LogReader> pReader = nullptr;
    std::unique_ptr<LogRender> pRender = nullptr;
    std::unique_ptr<DebugRender> pDebugRender = nullptr;
    if (argc == 2)
    {
        sLogFile = argv[1];
        if (std::filesystem::exists(std::filesystem::path(sLogFile)))
        {
            pReader = std::make_unique<LogReader>(sLogFile);
            pRender = std::make_unique<LogRender>(*pReader);
            pDebugRender = std::make_unique<DebugRender>(*pReader);
            pRender->RegisterWidget<AccelorationWidget>("Acc");
        }
    }

    ImGui::FileBrowser fileBrowser;
    fileBrowser.SetTitle("Choose Race Render CSV files");
    fileBrowser.SetTypeFilters({".csv"});

    // Register widgets

    struct SRecordInfo
    {
        bool bIsRecording = false;
        bool bIsContextInitialized = false;
        bool bIsFinished = false;
        uint32_t nWidth = 1920;
        uint32_t nHeight = 1080;
        uint32_t nFrameCount = 0;
        float fFPS = 30.0f;
        float fElapsedTime = 0.0f;
    } recordInfo;

    /////////////////// 
    // Main loop
    std::unique_ptr<uint8_t[]> data;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushFont(pRoboto);

        ////////////////////////////////
        if (pReader == nullptr) // Show loading screen
        {
            
            fileBrowser.Open();
            fileBrowser.Display();
            if (fileBrowser.HasSelected())
            {
                pReader = std::make_unique<LogReader>(fileBrowser.GetSelected().string());
                pRender = std::make_unique<LogRender>(*pReader);
                pDebugRender = std::make_unique<DebugRender>(*pReader);
                pRender->RegisterWidget<AccelorationWidget>("Acc");
                fileBrowser.ClearSelected();
                fileBrowser.Close();
            }

            // TODO: file dialog
            //fileDialg.openDialog();
            //if (fileDialg.draw())
            //{
            //    auto selectedFiles = fileDialg.selected();
            //    if (selectedFiles.empty())
            //    {
            //        break;
            //    }
            //    else
            //    {
            //        pReader = std::make_unique<LogReader>(selectedFiles[0]);
            //        pRender = std::make_unique<LogRender>(*pReader);
            //        pDebugRender = std::make_unique<DebugRender>(*pReader);
            //    }
            //}

        }
        else
        {
            if (!recordInfo.bIsRecording)
            {
                //ImGui::BeginChild(
                //    "##Stop", ImVec2(0, 0), false,
                //    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

                if (ImGui::Button("Record"))
                {
                    recordInfo.bIsRecording = true;
                    recordInfo.bIsContextInitialized = false;
                    recordInfo.bIsFinished = false;
                    recordInfo.fElapsedTime = 0.0f;
                }
                ImGui::Text("%s\n", pReader->GetDebugStr().c_str());
                pDebugRender->DrawDataBox();
                static float fSeekTime = -1.0;
                fSeekTime = pRender->DrawTimeSlider();

                pRender->DrawBasicInfoBox();
                pRender->DrawMap();

                pRender->DrawWidgets();
                pRender->Update(1.0f / ImGui::GetIO().Framerate);
                static bool bIsPlaying;
                bIsPlaying = pRender->IsPlaying();
                //ImGui::EndChild();
                if (recordInfo.bIsFinished)
                {
                    ImGui::Begin("Record summary");
                    ImGui::Text("Recorded %d frames in %f seconds, %f FPS",
                                recordInfo.nFrameCount, recordInfo.fElapsedTime,
                                recordInfo.nFrameCount /
                                    fmax(recordInfo.fElapsedTime, 0.0001));
                    ImGui::End();
                }
            }
            else
            {
                // Handle recording
                if (ImGui::Button("Stop"))
                {
                    recordInfo.bIsRecording = false;
                    pRender->SetPlaying(false);
                }
                if (!recordInfo.bIsContextInitialized)
                {
                    // Initialize recording context
                    glfwSetWindowSize(window, recordInfo.nWidth, recordInfo.nHeight);
                    recordInfo.nFrameCount = 0;
                    data = std::make_unique<uint8_t[]>(recordInfo.nWidth * recordInfo.nWidth * 8);
                    pRender->SetTime(0.0f);
                    pRender->SetPlaying(true);
                    pRender->SetLooping(false);

                    recordInfo.bIsContextInitialized = true;
                }
                // Render boxes
                {
                    pRender->DrawBasicInfoBox();
                    pRender->DrawMap();
                    //pRender->DrawAcceBox();
                    pRender->DrawWidgets();
                    if(!pRender->Update(1.0f / recordInfo.fFPS))
                    {
                        recordInfo.bIsRecording = false;
                        recordInfo.bIsFinished = true;
                        pRender->SetPlaying(false);
                    }
                }
                glReadPixels(0, 0, recordInfo.nWidth, recordInfo.nHeight,
                             GL_RGBA, GL_UNSIGNED_BYTE, data.get());

                static char fileName[30];
                sprintf(fileName, "output/out_%05d.png", recordInfo.nFrameCount);
                stbi_write_png(fileName, recordInfo.nWidth,
                               recordInfo.nHeight, 4, data.get(),
                               sizeof(uint8_t) * 4 * recordInfo.nWidth);
                recordInfo.fElapsedTime += ImGui::GetIO().DeltaTime;
                recordInfo.nFrameCount++;
            }
        }

        ImGui::PopFont();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
