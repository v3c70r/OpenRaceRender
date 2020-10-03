#pragma once
#include <string>
struct RaceRecord;
class Widget
{
public:
    Widget(const std::string& name) : m_sName(name){}
    virtual ~Widget(){}
    virtual void Display(const RaceRecord& rec) const = 0;

private:
    const std::string m_sName;
};

class AccelorationWidget : public Widget
{
public:
    AccelorationWidget(const std::string& name) : Widget(name){};
    void Display(const RaceRecord& rec) const final;
    ~AccelorationWidget() override {}
};
