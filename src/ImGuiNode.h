#ifndef IMGUINODE_H
#define IMGUINODE_H

#pragma once
#include <imgui.h>
#include <memory>
#include <vector>
#include <array>

#include "ui_utils.h"


enum NODE_COLOR{
    BLACK = IM_COL32(0, 0, 0, 255),
    BLUE = IM_COL32(0, 0, 255, 255),
    DARK_BLUE = IM_COL32(0, 0, 100, 255),
    BROWN = IM_COL32(165, 42, 42, 255),
    CYAN = IM_COL32(0, 255, 255, 255),
    GREEN = IM_COL32(0, 255, 0, 255),
    DARK_GREEN = IM_COL32(0, 100, 0, 255),
    GREY = IM_COL32(128, 128, 128, 255),
    DARK_GREY = IM_COL32(32, 32, 32, 255),
    INDIGO = IM_COL32(75, 0, 130, 255),
    LIME = IM_COL32(0, 255, 0, 255),
    LIGHT_BLUE = IM_COL32(173, 216, 230, 255),
    MAGENTA = IM_COL32(255, 0, 255, 255),
    MAROON = IM_COL32(128, 0, 0, 255),
    ORANGE = IM_COL32(255, 128, 0, 255),
    PINK = IM_COL32(255, 192, 203, 255),
    PURPLE = IM_COL32(128, 0, 128, 255),
    RED = IM_COL32(255, 0, 0, 255),
    TURQUOISE = IM_COL32(64, 224, 208, 255),
    WHITE = IM_COL32(255, 255, 255, 255),
    YELLOW = IM_COL32(255, 255, 0, 255)
};

class ImGuiNode
{
public:
    ImGuiNode(const char * _title);
    ~ImGuiNode();

    virtual void Update() = 0;

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

protected :
    inline void SetNumAvailableInputs(uint32_t num) { 
        assert(num >= 0 && num <= 4);
        m_NumAvailableInputs = num; 
    }
public:
    inline uint32_t GetNumAvailableInputs() { return m_NumAvailableInputs; }
public:
    const char* title;
    NODE_COLOR color;
    ImVec2 position;
    ImVec2 size;
    ImVec2 offset; // this should be unique per node network, but that'll do for now

    bool selected = false;
    bool grabbed = false;
    bool highlighted = false;

private:
    uint32_t m_NumAvailableInputs = 1;

private:
    std::array<std::shared_ptr<ImGuiNode>, 4> inputs = {nullptr, nullptr, nullptr, nullptr};
};




#endif