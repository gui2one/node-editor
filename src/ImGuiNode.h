#ifndef IMGUINODE_H
#define IMGUINODE_H

#pragma once
#include <imgui.h>
class ImGuiNode
{
public:
    ImGuiNode(const char * _title);
    ~ImGuiNode();

    void Render(ImDrawList* draw_data, ImVec2 offset);
    bool IsHovered();
    bool IsNodeDragged();
public:

    const char* title;
    ImVec2 position;
    ImVec2 size;
    ImVec2 offset; // this should be unique per node network, but that'll do for now

    bool selected = false;
    bool grabbed = false;
    bool highlighted = false;
};

#endif