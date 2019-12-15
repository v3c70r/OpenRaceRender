#include "debugrender.h"
#include "logreader.h"
#include "imgui_plot/imgui_plot.h"
#include <vector>
#include <array>

DebugRender::DebugRender(const LogReader& LogReader)
    : m_logReader(LogReader), m_fTime(m_logReader.GetMinTimeStamp())
{
}


void DebugRender::DrawDataBox()
{
    ImGui::Begin("DataBox");

    static bool bIsEdited = false;
    //ImGui::Begin("TimeSlider");

    {
        // Slider
        ImGui::SliderFloat("", &m_fTime, m_logReader.GetMinTimeStamp(),
                           m_logReader.GetMaxTimeStamp(), "%.2f");
        bIsEdited = ImGui::IsItemEdited();
    }


    std::vector<char*> cheaders;
    const std::vector<std::string>& strHeaders = m_logReader.GetHeaders();
    for (const auto& str : strHeaders)
        cheaders.push_back(const_cast<char*>(str.c_str()));

    static int item_current = 0;
    ImGui::Combo("", &item_current, cheaders.data(), strHeaders.size());
    ImGui::Text("Selection index %d", item_current);



    RaceRecord rec = m_logReader.GetLowerBoundRecord(m_fTime);
    ImGui::Text("%f\n", rec.values[item_current]);

    const RaceRecord& minRec = m_logReader.GetMinRecord();
    const RaceRecord& maxRec = m_logReader.GetMaxRecord();
    {
        std::array<float, 200> arr;
        for (int i = 0; i < arr.size(); i++)
        {
            float t = (i - 100) * 0.01;
            RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime + t);
            float val = rec.values[item_current];
            arr[i] = val;
        }
        ImGui::PlotConfig conf;
        conf.values.ys = arr.data();
        conf.values.count = arr.size();
        conf.scale.min = minRec.values[item_current];
        conf.scale.max = maxRec.values[item_current];
        conf.tooltip.show = true;
        conf.frame_size = ImVec2(400, 100);
        conf.line_thickness = 2.f;

        ImGui::Plot("plot", conf);
    }
    ImGui::End();
}

void DebugRender::DrawDataDebug()
{
    // This widgets can draw and pan different data set

}
