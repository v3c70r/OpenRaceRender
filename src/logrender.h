#pragma once
class LogReader;
class LogRender
{
public:
    LogRender(const LogReader& LogReader);

    void DrawHeaderBox();
    void DrawDataBox();
    void DrawTimeSlider();

private:
    const LogReader& m_logReader;
    float m_fTime;
};
