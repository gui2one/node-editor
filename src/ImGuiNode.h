#ifndef IMGUINODE_H
#define IMGUINODE_H

#pragma once
#include <imgui.h>
class ImGuiNode
{
public:
    ImGuiNode();
    ~ImGuiNode();

    void render(ImDrawList* draw_data, ImVec2 offset);

public:

    const char* title;
    ImVec2 position;
    ImVec2 size;

    bool selected = false;
    bool hovered = false;
};

#endif