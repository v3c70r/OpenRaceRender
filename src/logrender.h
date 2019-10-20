#pragma once
class LogReader;
class LogRender
{
public:
    LogRender(const LogReader& LogReader);

// Draw functions for different boxes
    void DrawDataBox();
    void DrawBasicInfoBox();
    float DrawTimeSlider();
    void DrawAcceBox();
    void TogglePlaying()
    {
        m_bIsPlaying = !m_bIsPlaying;
    }

    void DrawRevBox();
    void DrawThrottleBrakeBox();
    void DrawMap();

    void Update(float dt)
    {
        if (m_bIsPlaying) m_fTime += dt;
    }

    bool IsPlaying() const
    {
        return m_bIsPlaying;
    }


private:
    const LogReader& m_logReader;
    bool m_bIsPlaying = false;
    float m_fTime;
};
