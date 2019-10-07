#include "logrender.h"
#include "imgui.h"
#include "logreader.h"

LogRender::LogRender(const LogReader& LogReader)
    : m_logReader(LogReader), m_fTime(m_logReader.GetMinTimeStamp())
{
}

void LogRender::DrawHeaderBox()
{
    ImGui::Begin("Header Picker");
    ImGui::End();
}
void LogRender::DrawDataBox()
{
    ImGui::Begin("DataBox");
    std::vector<char*> cheaders;
    const std::vector<std::string>& strHeaders = m_logReader.GetHeaders();
    for (const auto& str : strHeaders)
        cheaders.push_back(const_cast<char*>(str.c_str()));

    static int item_current = 0;
    ImGui::Combo("", &item_current, cheaders.data(), strHeaders.size());
    ImGui::Text("Selection index %d", item_current);
    //const RaceRecord& rec = m_logReader.GetLowerBoundRecord(m_fTime);
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    ImGui::Text("%f\n", rec.values[item_current]);
    ImGui::End();
}

void LogRender::DrawThrottleBrakeBox()
{
    const size_t ENGINE_SPEED_IDX = 21;
    //const size_t BRAKE_IDX = 17;
    const size_t THROTTLE_POS_INDEX = 23;
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);

    float progress = rec.values[THROTTLE_POS_INDEX] / 100.0;
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));

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
    for (int i = 0; i < normalizedPoints.size() - 1; i += 2)
    {
        ImVec2 point0(normalizedPoints[i].x * 10.0, normalizedPoints[i].y * 10.0);
        ImVec2 point1(normalizedPoints[i + 1].x * 10.0, normalizedPoints[i + 1].y * 10.0);
        draw_list->AddLine(
            ImVec2(canvas_pos.x + point0.x, canvas_pos.y + point0.y),
            ImVec2(canvas_pos.x + point1.x, canvas_pos.y + point1.y),
            IM_COL32(255, 255, 0, 255), 2.0f);
    }

    ImGui::End();
}


void LogRender::DrawSpeedBox()
{
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    const size_t ODB_SPEED_INDEX = 22;
    const size_t GPS_SPEED_INDEX = 11;
    ImGui::Begin("Speed");
    ImGui::Text("Speed: %d km/h", int(rec.values[ODB_SPEED_INDEX]));
    ImGui::End();
}

void LogRender::DrawTimeSlider()
{
    ImGui::Begin("TimeSlider");
    ImGui::SliderFloat("", &m_fTime, m_logReader.GetMinTimeStamp(),
                       m_logReader.GetMaxTimeStamp(), "%.2f");
    ImGui::End();
};

