#include "logreader.h"
#include <sstream>

LogReader::LogReader(const std::string& fileName)
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
                m_vHeaders = s_splitString(line);
                isHeaderVisited = true;
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
    return ss.str();
}

std::vector<std::string> LogReader::s_splitString(const std::string& input, char delimiter)
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
