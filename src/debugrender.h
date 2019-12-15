#pragma once
class LogReader;
class DebugRender
{
public:
    DebugRender(const LogReader& LogReader);
    void DrawDataBox();
    void DrawDataDebug();


private:
    const LogReader& m_logReader;
    float m_fTime;
};
