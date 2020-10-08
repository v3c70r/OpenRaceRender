#include "dashboardwidget.h"

#include "logreader.h"
DashboardWidget::DashboardWidget(const std::string& name)
    : Widget(name, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar,
             {700.0f, 110.0f}, {0.05, 0.85})
{
}

void DashboardWidget::Display(const RaceRecord& rec) const
{
    ImGui::SetNextWindowSize(m_vWindowSize);
    ImGui::SetNextWindowPos(GetWindowPos());
    const size_t ODB_SPEED_INDEX = 22;
    const size_t GPS_SPEED_INDEX = 11;
    if (ImGui::Begin("Dashboard", nullptr, m_nWindowFlags))
    {
        ImGui::Text("ODB Speed: %d km/h", int(rec.values[ODB_SPEED_INDEX]));
        ImGui::Text("GPS Speed: %d km/h", int(rec.values[GPS_SPEED_INDEX]));

        // Engine Rev
        {
            const size_t ENGINE_SPEED_IDX = 21;
            char buf[32];
            snprintf(buf, 32, "%.0f/%.0f", rec.values[ENGINE_SPEED_IDX],
                     m_fMaxRPM);
            float progress = rec.values[ENGINE_SPEED_IDX] / m_fMaxRPM;

            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), buf);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("Engine Speed");
        }

        // Throttle position
        {
            const size_t THROTTLE_POS_INDEX = 23;
            float progress = rec.values[THROTTLE_POS_INDEX] / 100.0f;
            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("Throttle Position");
        }
        ImGui::End();
    }
}

void DashboardWidget::DisplaySettings()
{
    ImGui::SliderFloat("##MaxRPM", &m_fMaxRPM, 0.0f, 10000.0f, "MAX %.0f");
    ImGui::SliderFloat("##RedlineRPM", &m_fRedlineRPM, 0.0f, 10000.0f, "REDLINE %.0f");
}
