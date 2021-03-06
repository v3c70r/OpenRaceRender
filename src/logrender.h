#pragma once
#include <map>
#include <memory>
#include <string>
class LogReader;
class Widget;
class LogRender
{
public:
    LogRender(const LogReader& LogReader);

// Draw functions for different boxes
    float DrawTimeSlider();

    void SetPlaying(bool bIsPlaying)
    {
        m_bIsPlaying = bIsPlaying;
    }

    void SetLooping(bool bIsLooping)
    {
        m_bIsLooping = bIsLooping;
    }

    bool Update(float dt);
    

    bool IsPlaying() const
    {
        return m_bIsPlaying;
    }
    void SetTime(float t)
    {
        m_fTime = t;
    }

    template<typename T>
    T* RegisterWidget(const std::string& name, bool bShouldDraw = true)
    {
        m_mpRegisteredWidgets[name] = std::make_unique<T>(name);
        m_mbShouldDraw[name] = bShouldDraw;
        return static_cast<T*>(m_mpRegisteredWidgets.at(name).get());
    }

    void UnRegisterWidget(const std::string& name)
    {
        m_mpRegisteredWidgets.erase(name);
        m_mbShouldDraw.erase(name);
    }

    void DrawWidgets() const;
    void DrawWidgetSettings();
    

private:
    const LogReader& m_logReader;
    bool m_bIsPlaying = false;
    bool m_bIsLooping = false;
    float m_fTime;
    std::map<std::string, std::unique_ptr<Widget>> m_mpRegisteredWidgets;
    std::map<std::string, bool> m_mbShouldDraw;
};


