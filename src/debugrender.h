#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <array>
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
    std::vector<std::vector<float>> m_vvCachedRawData;
    std::vector<std::vector<float>> m_vvCachedFlooredData;
    std::vector<std::vector<float>> m_vvCachedInterpolatedData;
    uint32_t selection_start = 0; 
    uint32_t selection_length = 0;
};
