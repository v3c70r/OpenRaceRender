#include "logrender.h"

#include <array>
#include <cmath>
#include <cstring>

#include "imgui.h"
#include "logreader.h"
#include "widget.h"
#include "widgets/accelerationwidget.h"
#include "widgets/dashboardwidget.h"
#include "widgets/mapwidget.h"

LogRender::LogRender(const LogReader& logReader)
    : m_logReader(logReader), m_fTime(m_logReader.GetMinTimeStamp())
{
    RegisterWidget<AccelerationWidget>("G-force");
    RegisterWidget<DashboardWidget>("Dashboard");
    MapWidget* pMapWidget = RegisterWidget<MapWidget>("MapWidget");
    pMapWidget->SetBoundingBox(logReader.GetAABB());
    pMapWidget->SetTrajectory(logReader.GetNormalizedTrajectory());
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

void DrawLapTime() {}

void LogRender::DrawMap()
{
    ImGui::Begin("Map");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos =
        ImGui::GetCursorScreenPos();  // ImDrawList API uses screen coordinates!
    std::vector<SVec2> normalizedPoints = m_logReader.GetNormalizedTrajectory();
    for (size_t i = 0; i < normalizedPoints.size() - 1; i += 2)
    {
        ImVec2 point0(normalizedPoints[i].x * 300.0f,
                      normalizedPoints[i].y * 300.0f);
        ImVec2 point1(normalizedPoints[i + 1].x * 300.0f,
                      normalizedPoints[i + 1].y * 300.0f);
        draw_list->AddLine(
            ImVec2(canvas_pos.x + point0.x, canvas_pos.y + point0.y),
            ImVec2(canvas_pos.x + point1.x, canvas_pos.y + point1.y),
            IM_COL32(200, 200, 200, 255), 2.0f);
    }
    // Plot the position
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);

    int nLongitudeIdx = 8;  // x
    int nLatitudeIdx = 7;   // y
    SVec2 normalizedPos = LogReader::GetNormalizedPosition(
        {rec.values[nLongitudeIdx], rec.values[nLatitudeIdx]},
        m_logReader.GetAABB());

    draw_list->AddCircle(ImVec2(canvas_pos.x + normalizedPos.x * 300,
                                canvas_pos.y + normalizedPos.y * 300),
                         4.0f, 0xFF0000FF, 50, 4.0f);
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

    if (bIsEdited)
        return m_fTime;
    else
        return -1.0;
};

void LogRender::DrawWidgets() const
{
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    for (const auto& widgetPair : m_mpRegisteredWidgets)
    {
        if (m_mbShouldDraw.at(widgetPair.first))
        {
            widgetPair.second->Display(rec);
        }
    }
}

void LogRender::DrawWidgetSettings()
{
    if (ImGui::Begin("Widget Settings"))
    {
        for (const auto& widgetPair : m_mpRegisteredWidgets)
        {
            ImGui::Checkbox((std::string("##")+widgetPair.first).c_str(), &m_mbShouldDraw.at(widgetPair.first));
            ImGui::SameLine();
            if (ImGui::CollapsingHeader(widgetPair.first.c_str()))
            {
                widgetPair.second->DisplaySettings();
            }
        }
        ImGui::End();
    }
}
