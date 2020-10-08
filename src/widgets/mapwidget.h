#pragma once
#include "../widget.h"
#include "../logreader.h"
#include <vector>

class MapWidget : public Widget
{
public:
    MapWidget(const std::string &name)
        : Widget(name, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar,
                 {215.0f, 260.0f}, {0.85f, 0.1f})
    {
    }
    void Display(const RaceRecord& rec) const final;
    void DisplaySettings() final;
private:
    std::vector<SVec2> m_vMapTrajectory;
};