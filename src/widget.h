#pragma once
#include <string>
#include <imgui.h>
#include <cmath>
struct RaceRecord;
class Widget
{
public:
    Widget(const std::string& name, ImGuiWindowFlags flags,
           const ImVec2& windowSize, const ImVec2& relativeWindowPos)
        : m_sName(name),
          m_nWindowFlags(flags),
          m_vWindowSize(windowSize),
          m_vRelativeWindowPos(relativeWindowPos)
    {
    }
    virtual ~Widget(){}
    virtual void Display(const RaceRecord& rec) const = 0;

protected:
    ImVec2 GetWindowPos() const
    {
        // Get window pos based on the screen size and relative
        const ImVec2& vScreenSize = ImGui::GetIO().DisplaySize;
        return {static_cast<float>(fmin(vScreenSize.x * m_vRelativeWindowPos.x,
                                        vScreenSize.x - m_vWindowSize.x)),
                static_cast<float>(fmin(vScreenSize.y * m_vRelativeWindowPos.y,
                                        vScreenSize.y - m_vWindowSize.y))};
    }

    protected:
        const std::string m_sName;
        ImGuiWindowFlags m_nWindowFlags = 0;
        ImVec2 m_vWindowSize = {215.0f, 260.0f};
        ImVec2 m_vRelativeWindowPos = {0.5, 0.5};
    };
