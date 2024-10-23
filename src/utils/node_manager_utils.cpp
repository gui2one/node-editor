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
    std::filesystem::path open_file_explorer()
    {

#ifdef _WIN32
        OPENFILENAMEA ofn;
        char szFile[260] = "";
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = 260;
        ofn.lpstrFilter = "All\0*.*\0";
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