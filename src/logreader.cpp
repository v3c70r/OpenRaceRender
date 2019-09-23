#include "logreader.h"
#include <limits>
#include <sstream>

bool operator<(const RaceRecord& thisRec, const RaceRecord& other)
{
    return thisRec.timestamp < other.timestamp;
}
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
                                          std::numeric_limits<float>::min());
            }
            else
            {
                // Read values
                if (line.size() > 0)
                {
                    std::vector<float> rec = SplitFloats(line);
                    const float& timestamp = rec[0];
                    m_vRecords.emplace_back(rec[0], rec);

                    // update min max values
                    m_minRecord.timestamp = timestamp < m_minRecord.timestamp
                                                ? timestamp
                                                : m_minRecord.timestamp;
                    m_maxRecord.timestamp = timestamp > m_maxRecord.timestamp
                                                ? timestamp
                                                : m_maxRecord.timestamp;

                    for (size_t i = 0; i < rec.size(); i++)
                    {
                        m_minRecord.values[i] =
                            timestamp < m_minRecord.values[i]
                                ? timestamp
                                : m_minRecord.values[i];

                        m_maxRecord.values[i] =
                            timestamp > m_maxRecord.values[i]
                                ? timestamp
                                : m_maxRecord.values[i];
                    }
                }
            }
        }
        lineCount++;
    }
}

std::string LogReader::GetDebugStr() const
{
    std::stringstream ss;
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
    return ss.str();
}

RaceRecord LogReader::GetInterpolatedRecord(float timestamp) const
{
    auto source =
        std::lower_bound(m_vRecords.begin(), m_vRecords.end(), timestamp);
    RaceRecord res = RaceRecord(timestamp, source->values);
    // Return last one if the value is greater than last
    if (source == m_vRecords.end()) return res;

    auto target = source++;
    float ratio = (timestamp - source->timestamp) /
                  (target->timestamp - source->timestamp);
    for (size_t i = 0; i < source->values.size(); i++)
    {
        res.values[i] =
            source->values[i] + (target->values[i] - source->values[i]) * ratio;
    }
    return res;
}

const RaceRecord& LogReader::GetLowerBoundRecord(float timestamp) const
{
    if (timestamp > m_minRecord.timestamp || timestamp < m_maxRecord.timestamp)
        return *m_vRecords.begin();

    auto it = std::lower_bound(m_vRecords.begin(), m_vRecords.end(), timestamp);
    return *it;
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

