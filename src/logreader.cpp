#include "logreader.h"
#include <limits>
#include <sstream>
#include <cassert>

//bool operator<(const RaceRecord& thisRec, const RaceRecord& other)
//{
//    return thisRec.timestamp < other.timestamp;
//}
bool operator<(const RaceRecord& thisRec, const float& otherTimestamp)
{
    return thisRec.timestamp < otherTimestamp;
}

//////////////////////////////////////////////////////

LogReader::LogReader(const std::string& fileName)
    : m_minRecord(std::numeric_limits<float>::max(), std::vector<float>()),
      m_maxRecord(std::numeric_limits<float>::min(), std::vector<float>())
{
    std::ifstream recordFile(fileName);
    std::string line;
    size_t lineCount = 0;
    bool isHeaderVisited = false;
    int nPreviousLap = 0;
    while (!recordFile.eof())
    {
        std::getline(recordFile, line);
        if (line[0] == '#')
        {
            m_vComments.emplace_back(lineCount, line);
        }
        else
        {
            if (!isHeaderVisited)
            {
                m_vHeaders = SplitString(line);
                isHeaderVisited = true;

                // Min max records
                m_minRecord.values.resize(m_vHeaders.size(),
                                          std::numeric_limits<float>::max());
                m_maxRecord.values.resize(m_vHeaders.size(),
                                          std::numeric_limits<float>::lowest());

            }
            else
            {
                // Read values
                if (line.size() > 0)
                {
                    std::vector<float> rec = SplitFloats(line);
                    const float& timestamp = rec[TIME_IDX];

                    const int nLap = (int)rec[LAP_IDX];
                    if (m_vLapStartIndex.empty() || nLap != nPreviousLap)
                    {
                        m_vLapStartIndex.push_back(m_vRecords.size());
                        nPreviousLap = nLap;

                    }
                    m_vRecords.emplace_back(timestamp, rec);

                    // update min max values
                    m_minRecord.timestamp = timestamp < m_minRecord.timestamp
                                                ? timestamp
                                                : m_minRecord.timestamp;
                    m_maxRecord.timestamp = timestamp > m_maxRecord.timestamp
                                                ? timestamp
                                                : m_maxRecord.timestamp;

                    for (size_t i = 0; i < rec.size(); i++)
                    {
                        if (rec[i] < m_minRecord.values[i])
                            m_minRecord.values[i] = rec[i];

                        if (rec[i] > m_maxRecord.values[i])
                            m_maxRecord.values[i] = rec[i];
                    }
                }
            }
        }
        lineCount++;
    }

    // Update bounding box
    // Put it out of the loop for readability
    UpdateTrajectory();
}

std::string LogReader::GetDebugStr() const
{
    std::stringstream ss;
    // Debug lap start indices
    for (size_t i = 0; i < m_vLapStartIndex.size(); i++)
    {
        ss <<i<<"\t"<<m_vLapStartIndex[i]<<std::endl;
    }
    for (const auto& comment : m_vComments)
    {
        ss << comment.first << "\t" << comment.second << "\n";
    }
    for (const auto& header : m_vHeaders)
    {
        ss << header << ", ";
    }
    ss << std::endl;
    for (const auto& item : m_minRecord.values)
    {
        ss << item << "\t";
    }
    ss << std::endl;
    for (const auto& item : m_maxRecord.values)
    {
        ss << item << "\t";
    }
    ss << std::endl;
    ss<< "Bounding box: "<<m_boundingBox.lower.x<<"\t"<<m_boundingBox.lower.y<<std::endl;
    ss<< "Bounding box: "<<m_boundingBox.upper.x<<"\t"<<m_boundingBox.upper.y;
    return ss.str();
}

RaceRecord LogReader::GetInterpolatedRecord(float timestamp) const
{
    auto source =
        std::lower_bound(m_vRecords.begin() + 1, m_vRecords.end(), timestamp) - 1;
    RaceRecord res = RaceRecord(timestamp, source->values);
    assert(source->timestamp <= timestamp);
    // Return last one if the value is greater than last
    if (source == m_vRecords.end()) return res;

    auto target = source+1;
    float ratio = (timestamp - source->timestamp) /
                  (target->timestamp - source->timestamp);
    assert(ratio >= 0.0);
    for (size_t i = 0; i < source->values.size(); i++)
    {
        res.values[i] =
            source->values[i] + (target->values[i] - source->values[i]) * ratio;
    }
    return res;
}

const RaceRecord& LogReader::GetLowerBoundRecord(float timestamp) const
{
    if (timestamp < m_minRecord.timestamp || timestamp > m_maxRecord.timestamp)
        return *m_vRecords.begin();

    RaceRecord lowerBoundRecord = {timestamp, std::vector<float>()};
    auto it = std::lower_bound(m_vRecords.begin(), m_vRecords.end(), lowerBoundRecord);
    return *it;
}

std::vector<SVec2> LogReader::GetNormalizedTrajectory() const
{
    std::vector<SVec2> res;
    res.reserve(m_vTrajectory.size());
    float fXScale = m_boundingBox.upper.x - m_boundingBox.lower.x;
    float fYScale = m_boundingBox.upper.y - m_boundingBox.lower.y;
    float fScale = std::max(fYScale, fXScale);
    for (const SVec2& point: m_vTrajectory)
    {
        res.push_back((point - m_boundingBox.lower) / fScale);
    }
    return res;
}
SVec2 LogReader::GetNormalizedPosition(const SVec2 &pos) const
{
    float fXScale = m_boundingBox.upper.x - m_boundingBox.lower.x;
    float fYScale = m_boundingBox.upper.y - m_boundingBox.lower.y;
    float fScale = std::max(fYScale, fXScale);
    return SVec2(pos - m_boundingBox.lower)/fScale ;
}

std::vector<RaceRecord> LogReader::GetLapRecords(size_t nLap) const
{
    std::vector<RaceRecord>::const_iterator begin =
        m_vRecords.begin() + m_vLapStartIndex[nLap];
    std::vector<RaceRecord>::const_iterator end =
        nLap == m_vLapStartIndex.size() - 1
            ? m_vRecords.end()
            : m_vRecords.begin() + m_vLapStartIndex[nLap + 1];
    return std::vector<RaceRecord>(begin, end);
}

//////////////////////////////////////////////////////////

void LogReader::UpdateTrajectory()
{
    int nLongitudeIdx = 8;   // x
    int nLatitudeIdx = 7;    //y
    m_boundingBox = {
        {m_maxRecord.values[nLongitudeIdx], m_maxRecord.values[nLatitudeIdx]},
        {m_minRecord.values[nLongitudeIdx], m_minRecord.values[nLatitudeIdx]}
    };
    m_vTrajectory.clear();
    m_vTrajectory.reserve(m_vRecords.size());
    for (const RaceRecord& rec: m_vRecords)
    {
        float x = rec.values[nLongitudeIdx];
        float y = rec.values[nLatitudeIdx];
        m_vTrajectory.push_back(SVec2({x, y}));
    }
}
//////////////////////////////////////////////////////////

std::vector<std::string> LogReader::SplitString(const std::string& input,
                                                char delimiter)
{
    std::vector<std::string> res;
    std::stringstream ss(input);
    while (ss.good())
    {
        std::string substr;
        getline(ss, substr, delimiter);
        res.push_back(substr);
    }
    return res;
}

std::vector<float> LogReader::SplitFloats(const std::string& input,
                                          char delimiter)
{
    std::vector<float> res;
    std::stringstream ss(input);
    while (ss.good())
    {
        std::string substr;
        getline(ss, substr, delimiter);
        res.push_back(std::stof(substr));
    }
    return res;
}

//////////////////////////

