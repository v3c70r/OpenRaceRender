#pragma once
class LogReader;
class LogRender
{
public:
    LogRender(const LogReader& LogReader);

// Draw functions for different boxes
    void DrawHeaderBox();
    void DrawDataBox();
    float DrawTimeSlider();
    void DrawRevBox();
    void DrawThrottleBrakeBox();
    void DrawSpeedBox();
    void DrawMap();

    void Update(float dt)
    {
        m_fTime += dt;
    }


private:
    const LogReader& m_logReader;
    bool m_bIsPaused = false;
    float m_fTime;
};
