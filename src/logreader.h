#pragma once
#include <fstream>
#include <vector>
struct RaceRecord
{
    float timestamp;
    std::vector<float> values;
    RaceRecord(float ts, const std::vector<float>& vs)
        : timestamp(ts), values(vs)
    {
    }
};

class LogReader
{
public:
    LogReader(const std::string& fileName);
    std::string GetDebugStr() const;
    const std::vector<std::string>& GetHeaders() const { return m_vHeaders;}
private:
    // Line Comment
    using LinedComment = std::pair<size_t, std::string>;
    std::vector<LinedComment> m_vComments;
    std::vector<std::string> m_vHeaders;
    std::vector<RaceRecord> m_vRecords;
    static std::vector<std::string> SplitString(const std::string& input, char delimiter = ',');
    static std::vector<float> SplitFloats(const std::string& input, char delimiter = ',');

};

class LogRender
{
public:
    LogRender(const LogReader& LogReader) : m_logReader(LogReader) {}

    void DrawHeaderBox();
    void DrawDataBox();

private:
    const LogReader m_logReader;
};
