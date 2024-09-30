#ifndef IMGUINODE_H
#define IMGUINODE_H

#pragma once
#include <imgui.h>
#include <memory>
#include <vector>
#include <array>
class ImGuiNode
{
public:
    ImGuiNode(const char * _title);
    ~ImGuiNode();

    void Render(ImDrawList* draw_data, ImVec2 offset);
    bool IsHovered();
    bool IsNodeDragged();

    inline void SetInput(uint32_t index, std::shared_ptr<ImGuiNode> node) { 
        if( index < 0 || index > 3) return;
        inputs[index] = node; 
    }

    inline std::shared_ptr<ImGuiNode>GetInput(uint32_t index) {
        if( index < 0 || index > 3) return nullptr;
        return inputs[index];
    }
public:

    const char* title;
    ImVec2 position;
    ImVec2 size;
    ImVec2 offset; // this should be unique per node network, but that'll do for now

    bool selected = false;
    bool grabbed = false;
    bool highlighted = false;

private:
    std::array<std::shared_ptr<ImGuiNode>, 4> inputs = {nullptr, nullptr, nullptr, nullptr};
};




#endif