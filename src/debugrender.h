#pragma once
#include <vector>
class LogReader;
class DebugRender
{
public:
    DebugRender(const LogReader& LogReader);
    void DrawDataBox();
    void DrawDataDebug();


private:
    const LogReader& m_logReader;
    int m_nSelectedItem = 0;
    float m_fTime;
    std::vector<char*> m_vHeaders;
    std::vector<std::vector<float>> m_vvCachedData;
};
