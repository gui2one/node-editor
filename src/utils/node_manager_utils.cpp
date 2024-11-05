#include "node_manager_utils.h"

namespace NodeEditor::Utils {
    std::shared_ptr<AbstractNode> FindNodeByUUID(std::string uuid, std::vector<std::shared_ptr<AbstractNode>> nodes)
    {
        for(auto node : nodes){
            if(node->uuid == uuid){
                return node;
            }
        }
        return nullptr;
    }

    ImVec2 get_nodes_center(std::vector<std::shared_ptr<AbstractNode>>& nodes) {
        if (nodes.size() == 0)
            return ImVec2(0, 0);
        float minx = 999999999.f, miny = 999999999.f, maxx = -999999999.f,
                maxy = -999999999.f;

        for (auto node : nodes) {
            if (node->position.x < minx)
            minx = node->position.x;
            if (node->position.y < miny)
            miny = node->position.y;
            if (node->position.x > maxx)
            maxx = node->position.x;
            if (node->position.y > maxy)
            maxy = node->position.y;
        }
        float centerx, centery;
        centerx = (minx + maxx) / 2.0f + nodes[0]->size.x / 2.0f;
        centery = (miny + maxy) / 2.0f + nodes[0]->size.y / 2.0f;
        return ImVec2(centerx, centery);
    }
    void deselect_all(std::vector<std::shared_ptr<AbstractNode>> &nodes)
    {
        for(auto node : nodes){
            node->selected = false;
        }
        // return ImVec2();
    }


    const char* create_windows_file_explorer_Filter(std::vector<FileFilterItem> filters) {
        std::vector<char> filter;

        for(auto& item : filters){

        // Append description with a null character
        filter.insert(filter.end(), item.description.begin(), item.description.end());
        filter.push_back('|');
        filter.insert(filter.end(), item.extension.begin(), item.extension.end());
        filter.push_back('\0');

        // Append file extension pattern with a null character
        filter.push_back('*');
        filter.push_back('.');
        filter.insert(filter.end(), item.extension.begin(), item.extension.end());
        filter.push_back('\0');

        }
        // Append an additional null character at the end to terminate the filter
        filter.push_back('\0');

        return filter.data();
    }    
    std::filesystem::path open_file_explorer(std::vector<FileFilterItem> filters)
    {

#ifdef _WIN32
        OPENFILENAMEA ofn;
        char szFile[260] = "";
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = 260;
        ofn.lpstrFilter = create_windows_file_explorer_Filter(filters);
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_CREATEPROMPT;

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return std::filesystem::path(ofn.lpstrFile);
        }
        else
        {
            return std::filesystem::path("");
        }
#else
        return std::filesystem::path("");
#endif
    }
};