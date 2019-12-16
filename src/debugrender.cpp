#include "debugrender.h"

#include <array>
#include <vector>

#include "imgui_plot/imgui_plot.h"
#include "logreader.h"

DebugRender::DebugRender(const LogReader& LogReader)
    : m_logReader(LogReader), m_fTime(m_logReader.GetMinTimeStamp())
{
    const std::vector<std::string>& strHeaders = m_logReader.GetHeaders();
    for (const auto& str : strHeaders)
    {
        m_vHeaders.push_back(const_cast<char*>(str.c_str()));
    }
    float fEndTime = m_logReader.GetMaxRecord().timestamp;
    float fBeginTime = m_logReader.GetMinRecord().timestamp;
    m_vvCachedData.resize(m_vHeaders.size());
    for (float fTime = fBeginTime; fTime < fEndTime; fTime += 0.33)
    {
        RaceRecord rec = m_logReader.GetInterpolatedRecord(fTime);
        for (size_t i = 0; i< rec.values.size(); i++)
        {
            m_vvCachedData[i].push_back(rec.values[i]);
        }
    }
}

void DebugRender::DrawDataBox()
{
    ImGui::Begin("DataBox", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    //{
    //    ImGui::SliderFloat("", &m_fTime, m_logReader.GetMinTimeStamp(),
    //                       m_logReader.GetMaxTimeStamp(), "%.2f");
    //}
    //ImGui::SameLine();

    ImGui::Combo("", &m_nSelectedItem, m_vHeaders.data(),
                 m_logReader.GetHeaders().size());
    ImGui::Text("Selection index %d", m_nSelectedItem);

    RaceRecord rec = m_logReader.GetLowerBoundRecord(m_fTime);
    ImGui::Text("%f\n", rec.values[m_nSelectedItem]);

    const RaceRecord& minRec = m_logReader.GetMinRecord();
    const RaceRecord& maxRec = m_logReader.GetMaxRecord();
    {
         static uint32_t selection_start = 0, selection_length = 0;

        const std::vector<float>& vItemValue = m_vvCachedData[m_nSelectedItem];
        ImGui::PlotConfig conf;
        conf.values.ys = vItemValue.data();
        conf.values.count = vItemValue.size();
        conf.scale.min = minRec.values[m_nSelectedItem];
        conf.scale.max = maxRec.values[m_nSelectedItem];
        conf.tooltip.show = true;
        //conf.frame_size = ImVec2(ImGui::GetContentRegionAvail().x, 100);
        conf.frame_size = ImVec2(1024, 100);
        conf.line_thickness = 1.f;
        conf.selection.show = true;
        conf.selection.start = &selection_start;
        conf.selection.length = &selection_length;

        ImGui::Plot("plot", conf);

        // Draw second plot with the selection
        // reset previous values
        conf.values.ys_list = nullptr;
        conf.selection.show = false;

        conf.grid_x.show = true;
        conf.grid_x.size = 128;
        conf.grid_x.subticks = 4;
        conf.grid_y.show = true;
        conf.grid_y.size = 0.5f;
        conf.grid_y.subticks = 5;
        // set new ones
        conf.values.ys = vItemValue.data();
        conf.values.offset = selection_start;
        conf.values.count = selection_length;
        conf.line_thickness = 2.f;
        ImGui::Plot("plot2", conf);
    }
    ImGui::End();
}

void DebugRender::DrawDataDebug()
{
    // This widgets can draw and pan different data set
}
