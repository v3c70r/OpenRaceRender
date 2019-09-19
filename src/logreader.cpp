#include "logreader.h"
#include "imgui.h"
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
                m_vHeaders = SplitString(line);
                isHeaderVisited = true;
            }
            else
            {
                // Read values
                if (line.size() > 0) 
                {
                    std::vector<float> rec = SplitFloats(line);
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
void LogRender::DrawHeaderBox()
{
    ImGui::Begin("Header Picker");
    ImGui::End();
}
void LogRender::DrawDataBox()
{
    ImGui::Begin("DataBox");
    std::vector<char*> cheaders;
    const std::vector<std::string>& strHeaders = m_logReader.GetHeaders();
    for (const auto& str: strHeaders)
        cheaders.push_back(const_cast<char*>(str.c_str()));

    static int item_current = 0;
    ImGui::Combo("", &item_current, cheaders.data(), strHeaders.size());
    ImGui::End();
}

