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


struct SVec2
{
    float x, y;
    SVec2 operator-(const SVec2& other) const
    {
        return SVec2({x - other.x, y - other.y});
    }
    SVec2 operator/(const float& val) const { return SVec2({x / val, y / val}); }
};

struct SAABB
{
    SVec2 upper;
    SVec2 lower;
};


class LogReader
{
public:
    LogReader(const std::string& fileName);
    std::string GetDebugStr() const;
    const std::vector<std::string>& GetHeaders() const { return m_vHeaders;}
    RaceRecord GetInterpolatedRecord(float timestamp) const;
    const RaceRecord& GetLowerBoundRecord(float timestamp) const;
    const RaceRecord& GetRecord(size_t idx) const {return m_vRecords.at(idx);}
    size_t GetRecordSize() const{return m_vRecords.size();}

    float GetMinTimeStamp() const { return m_minRecord.timestamp; }
    float GetMaxTimeStamp() const { return m_maxRecord.timestamp; }

    const RaceRecord& GetMinRecord() const { return m_minRecord; }
    const RaceRecord& GetMaxRecord() const { return m_maxRecord; }

    SAABB GetAABB() const { return m_boundingBox; }
    const std::vector<SVec2>& GetTrajectory() const { return m_vTrajectory;}
    std::vector<SVec2> GetNormalizedTrajectory() const;

    size_t GetNumberOfLaps() const { return m_vLapStartIndex.size(); }
    std::vector<RaceRecord> GetLapRecords(size_t nLap) const;
    

private:
    void UpdateTrajectory();
    static std::vector<std::string> SplitString(const std::string& input, char delimiter = ',');
    static std::vector<float> SplitFloats(const std::string& input, char delimiter = ',');
private:
    // Line Comment
    using LinedComment = std::pair<size_t, std::string>;
    std::vector<LinedComment> m_vComments;
    std::vector<std::string> m_vHeaders;
    std::vector<RaceRecord> m_vRecords;
    std::vector<size_t> m_vLapStartIndex;   // Indicates the starting and end index of the record in each lap

    RaceRecord m_minRecord; // Contains all of the min values for each items
    RaceRecord m_maxRecord; // Contains all of the max values for each items
    SAABB m_boundingBox;
    std::vector<SVec2> m_vTrajectory;

    // Hardcoded lap and time index in the record
    const int TIME_IDX = 0;
    const int LAP_IDX = 2;
};

