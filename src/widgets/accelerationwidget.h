#pragma once
#include "../widget.h"
class AccelerationWidget : public Widget
{
public:
    AccelerationWidget(const std::string& name);
    void Display(const RaceRecord& rec) const final;
private:
    const float CIRCLE_SIZE = 100.0f;
};
