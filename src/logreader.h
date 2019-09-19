#pragma once
#include <fstream>
#include <vector>
class LogReader
{
public:
    LogReader(const std::string& fileName);
    std::string GetDebugStr() const;
private:
    // Line Comment
    using LinedComment = std::pair<size_t, std::string>;
    std::vector<LinedComment> m_vComments;
    std::vector<std::string> m_vHeaders;
    static std::vector<std::string> s_splitString(const std::string& input, char delimiter = ',');

};
