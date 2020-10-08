#pragma once
#include "../widget.h"
#include "../basictypes.h"
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
    void SetBoundingBox(const SAABB& boundingBox);
    void SetTrajectory(const std::vector<SVec2> &vTrajectory);
private:
    std::vector<SVec2> m_vMapTrajectory;
    SAABB m_boundingBox;
    float m_fSize = 200.0f;
    SVec2 m_vOffset = {0.0f, 0.0f};
    bool m_bFollow = false;
};
