#include "logreader.h"
#include <sstream>
#include <limits>

bool operator<(const RaceRecord& thisRec, const RaceRecord& other)
{
    return thisRec.timestamp < other.timestamp;
}
bool operator<(const RaceRecord& thisRec,
                           const float& otherTimestamp)
{
    return thisRec.timestamp < otherTimestamp;
}

//////////////////////////////////////////////////////



LogReader::LogReader(const std::string& fileName)
    : m_fMinTimeStamp(std::numeric_limits<float>::max()),
      m_fMaxTimeStamp(std::numeric_limits<float>::min())
{
    std::ifstream recordFile(fileName);
    std::string line;
    size_t lineCount = 0;
    bool isHeaderVisited = false;
    while(!recordFile.eof())
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
            }
            else
            {
                // Read values
                if (line.size() > 0) 
                {
                    std::vector<float> rec = SplitFloats(line);
                    const float& timestamp = rec[0];
                    m_fMinTimeStamp = timestamp < m_fMinTimeStamp ? timestamp : m_fMinTimeStamp;
                    m_fMaxTimeStamp = timestamp > m_fMaxTimeStamp ? timestamp : m_fMaxTimeStamp;
                    m_vRecords.emplace_back(rec[0], rec);
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
        ss<<comment.first<<"\t"<<comment.second<<"\n";
    }
    for (const auto& header: m_vHeaders)
    {
        ss<<header<<", ";
    }
    ss<<std::endl;
    for (const auto& rec: m_vRecords)
    {
        ss<<rec.values[0]<<std::endl;
    }
    return ss.str();
}

RaceRecord LogReader::GetInterpolatedRecord(float timestamp) const
{

    // TODO: Handle interpolation
    //auto source = std::lower_bound(m_vRecords.begin(), m_vRecords.end(), timestamp);
    //auto target = source++;
    return RaceRecord(timestamp, std::vector<float>());
}

const RaceRecord& LogReader::GetLowerBoundRecord(float timestamp) const
{
    if (timestamp > m_fMaxTimeStamp || timestamp < m_fMinTimeStamp)
        return *m_vRecords.begin();

    auto it = std::lower_bound(m_vRecords.begin(), m_vRecords.end(), timestamp);
    return *it;
}

//////////////////////////////////////////////////////////

std::vector<std::string> LogReader::SplitString(const std::string& input, char delimiter)
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

std::vector<float> LogReader::SplitFloats(const std::string& input, char delimiter)
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

