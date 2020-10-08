#include "mapwidget.h"

#include "logreader.h"

void MapWidget::Display(const RaceRecord &rec) const
{
    ImGui::SetNextWindowSize(m_vWindowSize);
    ImGui::SetNextWindowPos(GetWindowPos());

    int nLongitudeIdx = 8; // x
    int nLatitudeIdx = 7;  // y
    SVec2 normalizedPos = LogReader::GetNormalizedPosition(
        {rec.values[nLongitudeIdx], rec.values[nLatitudeIdx]},
        m_boundingBox);


    if (ImGui::Begin("Map", nullptr, m_nWindowFlags))
    {
        SVec2 vOffset = m_vOffset;
        if (m_bFollow)
        {
            ImVec2 vWindowSize = ImGui::GetWindowSize();
            vOffset.x = vWindowSize.x * 0.5f - normalizedPos.x * m_fSize;
            vOffset.y = vWindowSize.y * 0.5f - normalizedPos.y * m_fSize;
        }
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos =
            ImGui::GetCursorScreenPos();  // ImDrawList API uses screen

        for (size_t i = 0; i < m_vMapTrajectory.size() - 1; i += 2)
        {
            ImVec2 point0(m_vMapTrajectory[i].x * m_fSize,
                          m_vMapTrajectory[i].y * m_fSize);
            ImVec2 point1(m_vMapTrajectory[i + 1].x * m_fSize,
                          m_vMapTrajectory[i + 1].y * m_fSize);
            draw_list->AddLine(
                ImVec2(canvas_pos.x + point0.x + vOffset.x, canvas_pos.y + point0.y + vOffset.y),
                ImVec2(canvas_pos.x + point1.x + vOffset.x, canvas_pos.y + point1.y + vOffset.y),
                IM_COL32(200, 200, 200, 255), 2.0f);
        }

        // Plot the position
        draw_list->AddCircle(ImVec2(canvas_pos.x + normalizedPos.x * m_fSize + vOffset.x,
                                    canvas_pos.y + normalizedPos.y * m_fSize + vOffset.y),
                             4.0f, 0xFF0000FF, 50, 4.0f);
        ImGui::End();
    }
}

void MapWidget::DisplaySettings()
{
    ImGui::SliderFloat("##mapSize", &m_fSize, 0.0f, 500.0f, "ZOOM: %.3f");
    ImGui::SliderFloat("##xoffset", &m_vOffset.x, 0.0f, 500.0f);
    ImGui::SliderFloat("##yoffset", &m_vOffset.y, 0.0f, 500.0f);
    ImGui::Checkbox("Follow", &m_bFollow);
}

void MapWidget::SetBoundingBox(const SAABB &boundingBox)
{
    m_boundingBox = boundingBox;
}
void MapWidget::SetTrajectory(const std::vector<SVec2> &vTrajectory)
{
    m_vMapTrajectory = vTrajectory;
}
