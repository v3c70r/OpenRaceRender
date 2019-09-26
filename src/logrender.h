#pragma once
class LogReader;
class LogRender
{
public:
    LogRender(const LogReader& LogReader);

    void DrawHeaderBox();
    void DrawDataBox();
    void DrawTimeSlider();
    void DrawRevBox();
    void DrawThrottleBrakeBox();
    void DrawSpeedBox();

    void Update(float dt)
    {
        m_fTime += dt;
    }


private:
    const LogReader& m_logReader;
    bool m_bIsPaused = false;
    float m_fTime;
};
