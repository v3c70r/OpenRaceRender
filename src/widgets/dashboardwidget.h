#pragma once
#include "../widget.h"

// Dashboard widget displays the speed, tachometer and some other infos
class DashboardWidget : public Widget
{
public:
    DashboardWidget(const std::string& name);
    void Display(const RaceRecord& rec) const final;
    void DisplaySettings() final;
private:
    float m_fMaxRPM = 6000.0f;
};
