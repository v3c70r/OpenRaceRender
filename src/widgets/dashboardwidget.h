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
    float m_fRedlineRPM = 5000.0f;

    const size_t ODB_SPEED_INDEX = 22;
    const size_t GPS_SPEED_INDEX = 11;
    const size_t ENGINE_SPEED_IDX = 21;
};
