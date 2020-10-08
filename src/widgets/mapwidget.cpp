#include "mapwidget.h"

#include "logreader.h"

void MapWidget::Display(const RaceRecord &rec) const
{
    ImGui::SetNextWindowSize(m_vWindowSize);
    ImGui::SetNextWindowPos(GetWindowPos());
    if (ImGui::Begin("Map", nullptr, m_nWindowFlags))
    {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos =
            ImGui::GetCursorScreenPos();  // ImDrawList API uses screen

        for (size_t i = 0; i < m_vMapTrajectory.size() - 1; i += 2)
        {
            ImVec2 point0(m_vMapTrajectory[i].x * 300.0f,
                          m_vMapTrajectory[i].y * 300.0f);
            ImVec2 point1(m_vMapTrajectory[i + 1].x * 300.0f,
                          m_vMapTrajectory[i + 1].y * 300.0f);
            draw_list->AddLine(
                ImVec2(canvas_pos.x + point0.x, canvas_pos.y + point0.y),
                ImVec2(canvas_pos.x + point1.x, canvas_pos.y + point1.y),
                IM_COL32(200, 200, 200, 255), 2.0f);
        }

        // Plot the position

        int nLongitudeIdx = 8;  // x
        int nLatitudeIdx = 7;   // y
        SVec2 normalizedPos = LogReader::GetNormalizedPosition(
            {rec.values[nLongitudeIdx], rec.values[nLatitudeIdx]},
            m_boundingBox);

        draw_list->AddCircle(ImVec2(canvas_pos.x + normalizedPos.x * 300,
                                    canvas_pos.y + normalizedPos.y * 300),
                             4.0f, 0xFF0000FF, 50, 4.0f);
        ImGui::End();
    }
}

void MapWidget::DisplaySettings() {}

void MapWidget::SetBoundingBox(const SAABB &boundingBox)
{
    m_boundingBox = boundingBox;
}
void MapWidget::SetTrajectory(const std::vector<SVec2> &vTrajectory)
{
    m_vMapTrajectory = vTrajectory;
}
