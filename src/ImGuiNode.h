#ifndef IMGUINODE_H
#define IMGUINODE_H

#pragma once
// #include <imgui.h>
#include <memory>
#include <iostream>
#include <vector>
#include <array>

// #include "NodeParam.h"

constexpr uint32_t MAX_N_INPUTS = 4;
namespace NodeEditor {

// forward declaration
class NodeParam;

enum NODE_COLOR
{
    BLACK = (ImU32)IM_COL32(0, 0, 0, 255),
    BLUE = (ImU32)IM_COL32(0, 0, 255, 255),
    DARK_BLUE = (ImU32)IM_COL32(0, 0, 100, 255),
    BROWN = (ImU32)IM_COL32(165, 42, 42, 255),
    CYAN = (ImU32)IM_COL32(0, 255, 255, 255),
    GREEN = (ImU32)IM_COL32(0, 255, 0, 255),
    DARK_GREEN = (ImU32)IM_COL32(0, 100, 0, 255),
    LIGHT_GREY = (ImU32)IM_COL32(160, 160, 160, 255),
    GREY = (ImU32)IM_COL32(128, 128, 128, 255),
    DARK_GREY = (ImU32)IM_COL32(32, 32, 32, 255),
    INDIGO = (ImU32)IM_COL32(75, 0, 130, 255),
    LIME = (ImU32)IM_COL32(0, 255, 0, 255),
    LIGHT_BLUE = (ImU32)IM_COL32(173, 216, 230, 255),
    MAGENTA = (ImU32)IM_COL32(255, 0, 255, 255),
    MAROON = (ImU32)IM_COL32(128, 0, 0, 255),
    ORANGE = (ImU32)IM_COL32(255, 128, 0, 255),
    PINK = (ImU32)IM_COL32(255, 192, 203, 255),
    PURPLE = (ImU32)IM_COL32(128, 0, 128, 255),
    RED = (ImU32)IM_COL32(255, 0, 0, 255),
    TURQUOISE = (ImU32)IM_COL32(64, 224, 208, 255),
    WHITE = (ImU32)IM_COL32(255, 255, 255, 255),
    YELLOW = (ImU32)IM_COL32(255, 255, 0, 255)
};

struct InputConnector
{
    ImVec2 relative_pos;
    uint32_t index;
    float width = 5.0f; 
    bool hovered = false;
    bool grabbed = false;
};

class ImGuiNode
{
public:
    ImGuiNode(const char *_title);
    ~ImGuiNode();

    virtual void Update() = 0;

    inline void SetInput(uint32_t index, std::shared_ptr<ImGuiNode> node)
    {
        if (index < 0 || index > 3)
            return;
        inputs[index] = node;
    }

    inline void ResetInput(uint32_t index){
        if (index < 0 || index > 3)
            return;
        inputs[index] = nullptr;
    }
    inline std::shared_ptr<ImGuiNode> GetInput(uint32_t index)
    {
        if (index < 0 || index > 3)
            return nullptr;
        return inputs[index];
    }

    inline uint32_t GetNumAvailableInputs() { return m_NumAvailableInputs; }
    inline InputConnector *GetInputConnector(uint32_t index)
    {
        if (index < 0 || index >= GetNumAvailableInputs())
        {
            std::cout << "Problem with GetInputConnector" << std::endl;

            return nullptr;
        }
        return &m_InputConnectors[index];
    }

protected:
    inline void SetNumAvailableInputs(uint32_t num)
    {
        if (num > MAX_N_INPUTS)
        {
            std::cout << "Too many inputs" << std::endl;
            num = MAX_N_INPUTS;
        }
        m_NumAvailableInputs = num;
        InitInputConnectors();
    }

    void InitInputConnectors();

public:


    const char *title;
    NODE_COLOR color;
    ImVec2 position;
    ImVec2 size;
    std::vector<std::shared_ptr<NodeParam>> m_Params;

    bool selected = false;
    bool grabbed = false;
    bool highlighted = false;

private:
    uint32_t m_NumAvailableInputs = 1;

private:
    std::array<std::shared_ptr<ImGuiNode>, MAX_N_INPUTS> inputs = {nullptr, nullptr, nullptr, nullptr};
    std::vector<InputConnector> m_InputConnectors;
};

class OUTPUT_NODE : public ImGuiNode
{
public:
    OUTPUT_NODE(const char *_title) : ImGuiNode(_title)
    {
        SetNumAvailableInputs(1);
    }

    void Update() override;
};
};
#endif