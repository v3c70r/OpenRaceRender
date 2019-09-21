#include "logrender.h"
#include "logreader.h"
#include "imgui.h"

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
    for (const auto& str: strHeaders)
        cheaders.push_back(const_cast<char*>(str.c_str()));

    static int item_current = 0;
    ImGui::Combo("", &item_current, cheaders.data(), strHeaders.size());
    const RaceRecord& rec = m_logReader.GetLowerBoundRecord(m_fTime);
    ImGui::Text("%f\n", rec.values[item_current]);
    ImGui::End();
}

void LogRender::DrawTimeSlider()
{
    ImGui::Begin("TimeSlider");
    ImGui::SliderFloat("", &m_fTime, m_logReader.GetMinTimeStamp(), m_logReader.GetMaxTimeStamp(), "%.2f");
    ImGui::End();

};

