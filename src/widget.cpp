#include "widget.h"

#include <imgui.h>

#include "logreader.h"

void AccelorationWidget::Display(const RaceRecord& rec) const
{
    const size_t ACC_X_INDEX = 14;
    const size_t ACC_Y_INDEX = 15;
    float fAccX = rec.values[ACC_X_INDEX];
    float fAccY = rec.values[ACC_Y_INDEX];

    ImGui::Begin("Acceleration");
    // TODO: Fixed size, adaptive layout

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos =
        ImGui::GetCursorScreenPos();  // ImDrawList API uses screen coordinates!

    const float SZ = 100.0f;
    float x = canvas_pos.x + SZ;
    float y = canvas_pos.y + SZ;
    y += 40;

    draw_list->AddCircle(ImVec2(x, y), SZ, 0xFFFFFFFF, 50, 1.0f);
    draw_list->AddCircle(ImVec2(x + SZ * fAccY, y - SZ * fAccX), 3.0f,
                         0XFF0000FF, 10, 3.0f);

    draw_list->AddRectFilled(ImVec2(x - SZ, y), ImVec2(x + SZ, y + 1.0f),
                             0xFFFFFFFF);  // Horizontal line
    draw_list->AddRectFilled(ImVec2(x, y - SZ), ImVec2(x + 1.0, y + SZ),
                             0xFFFFFFFF);  // Vertical line

    ImGui::Text("Acc_X: %f g", fAccX);
    ImGui::Text("Acc_Y: %f g", fAccY);
    ImGui::End();
}
