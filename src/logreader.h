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
    RaceRecord GetInterpolatedRecord(float timestamp) const;
    const RaceRecord& GetLowerBoundRecord(float timestamp) const;

    float GetMinTimeStamp() const { return m_minRecord.timestamp; }
    float GetMaxTimeStamp() const { return m_maxRecord.timestamp; }
private:
    static std::vector<std::string> SplitString(const std::string& input, char delimiter = ',');
    static std::vector<float> SplitFloats(const std::string& input, char delimiter = ',');
private:
    // Line Comment
    using LinedComment = std::pair<size_t, std::string>;
    std::vector<LinedComment> m_vComments;
    std::vector<std::string> m_vHeaders;
    std::vector<RaceRecord> m_vRecords;

    RaceRecord m_minRecord;
    RaceRecord m_maxRecord;


};

