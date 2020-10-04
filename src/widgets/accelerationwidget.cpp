#include "accelerationwidget.h"

#include "logreader.h"

AccelerationWidget::AccelerationWidget(const std::string& name)
    : Widget(name, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar,
             {215.0f, 260.0f}, {0.85, 0.7})
{
}
void AccelerationWidget::Display(const RaceRecord& rec) const
{
    const size_t ACC_X_INDEX = 14;
    const size_t ACC_Y_INDEX = 15;
    float fAccX = rec.values[ACC_X_INDEX];
    float fAccY = rec.values[ACC_Y_INDEX];

    ImGui::SetNextWindowSize(m_vWindowSize);
    ImGui::SetNextWindowPos(GetWindowPos());
    if(ImGui::Begin("Acceleration", nullptr, m_nWindowFlags))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();  // ImDrawList API uses
                                                          // screen coordinates!

        float x = canvas_pos.x + CIRCLE_SIZE;
        float y = canvas_pos.y + CIRCLE_SIZE;
        y += 40;

        draw_list->AddCircle(ImVec2(x, y), CIRCLE_SIZE, 0xFFFFFFFF, 50, 1.0f);
        draw_list->AddCircle(ImVec2(x + CIRCLE_SIZE * fAccY, y - CIRCLE_SIZE * fAccX), 3.0f,
                             0XFF0000FF, 10, 3.0f);

        draw_list->AddRectFilled(ImVec2(x - CIRCLE_SIZE, y), ImVec2(x + CIRCLE_SIZE, y + 1.0f),
                                 0xFFFFFFFF);  // Horizontal line
        draw_list->AddRectFilled(ImVec2(x, y - CIRCLE_SIZE), ImVec2(x + 1.0, y + CIRCLE_SIZE),
                                 0xFFFFFFFF);  // Vertical line

        ImGui::Text("Acc_X: %f g", fAccX);
        ImGui::Text("Acc_Y: %f g", fAccY);
        ImGui::End();
    }
}
