#include "logrender.h"
#include "logreader.h"
#include "imgui.h"
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

