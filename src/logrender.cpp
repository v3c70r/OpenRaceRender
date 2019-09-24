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

void LogRender::DrawTimeSlider()
{
    ImGui::Begin("TimeSlider");
    ImGui::SliderFloat("", &m_fTime, m_logReader.GetMinTimeStamp(),
                       m_logReader.GetMaxTimeStamp(), "%.2f");
    ImGui::End();
};

