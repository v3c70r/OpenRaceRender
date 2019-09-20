#pragma once
class LogReader;
class LogRender
{
public:
    LogRender(const LogReader& LogReader) : m_logReader(LogReader) {}

    void DrawHeaderBox();
    void DrawDataBox();

private:
    const LogReader& m_logReader;
};
