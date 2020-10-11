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
        ImGui::Checkbox("Loop", &m_bIsLooping);
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
