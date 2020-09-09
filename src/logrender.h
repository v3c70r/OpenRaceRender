#pragma once
class LogReader;
class LogRender
{
public:
    LogRender(const LogReader& LogReader);

// Draw functions for different boxes
    void DrawBasicInfoBox();
    float DrawTimeSlider();
    void DrawAcceBox();

    void SetPlaying(bool bIsPlaying)
    {
        m_bIsPlaying = bIsPlaying;
    }

    void SetLooping(bool bIsLooping)
    {
        m_bIsLooping = bIsLooping;
    }

    void DrawRevBox();
    void DrawThrottleBrakeBox();
    void DrawMap();

    bool Update(float dt);
    

    bool IsPlaying() const
    {
        return m_bIsPlaying;
    }
    void SetTime(float t)
    {
        m_fTime = t;
    }

private:
    const LogReader& m_logReader;
    bool m_bIsPlaying = false;
    bool m_bIsLooping = false;
    float m_fTime;
};
