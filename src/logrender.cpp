#include "logrender.h"
#include "imgui.h"
#include "logreader.h"
#include "widget.h"
#include <cstring>
#include <array>
#include <cmath>

LogRender::LogRender(const LogReader& LogReader)
    : m_logReader(LogReader), m_fTime(m_logReader.GetMinTimeStamp())
{
}

// Return true if is playing or can continue playing
bool LogRender::Update(float dt)
{
    if (m_bIsPlaying)
    {
        m_fTime = fmax(m_fTime, m_logReader.GetMinTimeStamp());
        if (m_fTime + dt < m_logReader.GetMaxTimeStamp())
        {
            m_fTime += dt;
            return true;
        }
        else
        {
            if (m_bIsLooping)
            {
                m_fTime = m_logReader.GetMinTimeStamp();
            }
            else
            {
                m_bIsPlaying = false;
            }
            return false;
        }
    }
    return false;
}

void LogRender::DrawThrottleBrakeBox()
{
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    //const size_t BRAKE_IDX = 17;
    const size_t THROTTLE_POS_INDEX = 23;

    float progress = rec.values[THROTTLE_POS_INDEX] / 100.0;
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));

    const size_t ENGINE_SPEED_IDX = 21;
    progress = rec.values[ENGINE_SPEED_IDX] / 9500.0;
    //sprintf("%.0f/%.0f", rec.values[ENGINE_SPEED_IDX], 9500);
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
}

void DrawLapTime()
{
}

void LogRender::DrawMap()
{

    ImGui::Begin("Map");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
    std::vector<SVec2> normalizedPoints = m_logReader.GetNormalizedTrajectory();
    for (size_t i = 0; i < normalizedPoints.size() - 1; i += 2)
    {
        ImVec2 point0(normalizedPoints[i].x * 300.0, normalizedPoints[i].y * 300.0);
        ImVec2 point1(normalizedPoints[i + 1].x * 300.0, normalizedPoints[i + 1].y * 300.0);
        draw_list->AddLine(
            ImVec2(canvas_pos.x + point0.x, canvas_pos.y + point0.y),
            ImVec2(canvas_pos.x + point1.x, canvas_pos.y + point1.y),
            IM_COL32(200, 200, 200, 255), 2.0f);
    }
    // Plot the position
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);

    int nLongitudeIdx = 8;   // x
    int nLatitudeIdx = 7;    //y
    SVec2 normalizedPos = m_logReader.GetNormalizedPosition(
        {rec.values[nLongitudeIdx], rec.values[nLatitudeIdx]});

    draw_list->AddCircle(ImVec2(canvas_pos.x + normalizedPos.x * 300,
                                canvas_pos.y + normalizedPos.y * 300),
                         4.0f, 0xFF0000FF, 50, 4.0f);
    ImGui::End();
}


void LogRender::DrawBasicInfoBox()
{
    // ODB and GPS speed
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    const size_t ODB_SPEED_INDEX = 22;
    const size_t GPS_SPEED_INDEX = 11;
    ImGui::Begin("Speed");
    ImGui::Text("ODB Speed: %d km/h", int(rec.values[ODB_SPEED_INDEX]));
    ImGui::Text("GPS Speed: %d km/h", int(rec.values[GPS_SPEED_INDEX]));

    // Engine Rev
    {
        const size_t ENGINE_SPEED_IDX = 21;
        char buf[32];
        std::sprintf(buf, "%.0f/%.0f", rec.values[ENGINE_SPEED_IDX], 9500.0f);
        float progress = rec.values[ENGINE_SPEED_IDX] / 9500.0;

        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), buf);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Engine Speed");
    }

    // Throttle position
    {
        const size_t THROTTLE_POS_INDEX = 23;
        float progress = rec.values[THROTTLE_POS_INDEX] / 100.0;
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Throttle Position");
    }
    ImGui::End();
}

float LogRender::DrawTimeSlider()
{
    static bool bIsEdited = false;
    ImGui::Begin("TimeSlider");

    {
        // Slider
        ImGui::SliderFloat("", &m_fTime, m_logReader.GetMinTimeStamp(),
                           m_logReader.GetMaxTimeStamp(), "%.2f");
        bIsEdited = ImGui::IsItemEdited();
    }
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    {
        if (m_bIsPlaying)
        {
            if (ImGui::Button("Pause"))
            {
                m_bIsPlaying = false;
            }
        }
        else
        {
            if (ImGui::Button("Play"))
            {
                m_bIsPlaying = true;
            }
        }
    }
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    {
    // Button
        if (m_bIsLooping)
        {
            if (ImGui::Button("Disable Loop"))
            {
                m_bIsLooping = false;
            }
        }
        else
        {
            if (ImGui::Button("Enable Loop"))
            {
                m_bIsLooping = true;
            }
        }
    }

    ImGui::End();

    if (bIsEdited) return m_fTime;
    else return -1.0;
};

void LogRender::DrawWidgets() const
{
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    for (const auto& widgetPair : m_mpRegisteredWidgets)
    {
        widgetPair.second->Display(rec);
    }
}

