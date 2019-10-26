#include "logrender.h"
#include "imgui.h"
#include "imgui_plot/imgui_plot.h"
#include "logreader.h"
#include <cstring>
#include <array>

LogRender::LogRender(const LogReader& LogReader)
    : m_logReader(LogReader), m_fTime(m_logReader.GetMinTimeStamp())
{
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

void LogRender::DrawThrottleBrakeBox()
{
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    //const size_t BRAKE_IDX = 17;
    const size_t THROTTLE_POS_INDEX = 23;

    float progress = rec.values[THROTTLE_POS_INDEX] / 100.0;
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));

    const size_t ENGINE_SPEED_IDX = 21;
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
        ImVec2 point0(normalizedPoints[i].x * 500.0, normalizedPoints[i].y * 500.0);
        ImVec2 point1(normalizedPoints[i + 1].x * 500.0, normalizedPoints[i + 1].y * 500.0);
        draw_list->AddLine(
            ImVec2(canvas_pos.x + point0.x, canvas_pos.y + point0.y),
            ImVec2(canvas_pos.x + point1.x, canvas_pos.y + point1.y),
            IM_COL32(255, 255, 0, 255), 2.0f);
    }

    ImGui::End();
}


void LogRender::DrawBasicInfoBox()
{
    // ODB and GPS speed
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    const size_t ODB_SPEED_INDEX = 22;
    const size_t GPS_SPEED_INDEX = 11;
    ImGui::Begin("Speed");
    ImGui::Text("ODB Speed: %d km/h", int(rec.values[ODB_SPEED_INDEX]));
    ImGui::Text("GPS Speed: %d km/h", int(rec.values[GPS_SPEED_INDEX]));

    // Engine Rev
    {
        const size_t ENGINE_SPEED_IDX = 21;
        char buf[32];
        std::sprintf(buf, "%.0f/%.0f", rec.values[ENGINE_SPEED_IDX], 9500.0f);
        float progress = rec.values[ENGINE_SPEED_IDX] / 9500.0;

        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), buf);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Engine Speed");
    }

    // Throttle position
    {
        const size_t THROTTLE_POS_INDEX = 23;
        float progress = rec.values[THROTTLE_POS_INDEX] / 100.0;
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Throttle Position");
    }
    ImGui::End();
}

void LogRender::DrawAcceBox()
{
    RaceRecord rec = m_logReader.GetInterpolatedRecord(m_fTime);
    const size_t ACC_X_INDEX = 14;
    const size_t ACC_Y_INDEX = 15;
    float fAccX = rec.values[ACC_X_INDEX];
    float fAccY = rec.values[ACC_Y_INDEX];

    ImGui::Begin("Acceleration");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!

    const float SZ = 100.0f;
    float x = canvas_pos.x + SZ;
    float y = canvas_pos.y + SZ;
    y += 40;

    draw_list->AddCircle(ImVec2(x, y), SZ, 0xFFFFFFFF, 50, 1.0f);
    draw_list->AddCircle(ImVec2(x + SZ * fAccY, y - SZ * fAccX), 3.0f, 0XFF0000FF, 10, 3.0f);

    draw_list->AddRectFilled(ImVec2(x - SZ, y), ImVec2(x + SZ, y + 1.0f), 0xFFFFFFFF); // Horizontal line
    draw_list->AddRectFilled(ImVec2(x, y - SZ), ImVec2(x + 1.0, y + SZ), 0xFFFFFFFF); // Vertical line

    ImGui::Text("Acc_X: %f g", fAccX);
    ImGui::Text("Acc_Y: %f g", fAccY);
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
        // Button
        if (ImGui::Button("Play / Pause"))
        {
            TogglePlaying();
        }
    }

    ImGui::End();

    if (bIsEdited) return m_fTime;
    else return -1.0;
};

