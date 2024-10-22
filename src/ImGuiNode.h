#ifndef IMGUINODE_H
#define IMGUINODE_H

#include <imgui.h>
#include <memory>
#include <iostream>
#include <vector>
#include <array>
#include "utils.h"

#include <yaml-cpp/yaml.h>


constexpr uint32_t MAX_N_INPUTS = 4;

namespace NodeEditor {

// forward declaration
class NodeParam;
struct NodeNetwork;

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

struct ParamLayoutItem{
    std::string name;
    std::shared_ptr<NodeParam> param;
};
struct ParamLayout{
    std::vector<ParamLayoutItem> items;
    ParamLayoutItem& Get(size_t idx);
};

class ImGuiNode : public std::enable_shared_from_this<ImGuiNode>
{
public:
    ImGuiNode(std::string _title);
    ~ImGuiNode();

    virtual void Update() = 0; // implemented lower, in the Node<T> class
    virtual void Generate() = 0; // user defined method. i.e the work the node is doint for the user app 
    
    YAML::Node YAMLSerialize();
    
    void SetInput(uint32_t index, std::shared_ptr<ImGuiNode> node);

    void ResetInput(uint32_t index);
    std::shared_ptr<ImGuiNode> GetInput(uint32_t index);

    inline uint32_t GetNumAvailableInputs() { return m_NumAvailableInputs; }
    InputConnector *GetInputConnector(uint32_t index);

    inline size_t GetMultiInputCount() { return m_MultiInput.size(); }
    inline std::shared_ptr<ImGuiNode> GetMultiInput(size_t index) { return m_MultiInput[index]; }
    inline void AppendInput(std::shared_ptr<ImGuiNode> node) { m_MultiInput.push_back(node); }
    void RemoveLastInput();
    inline void ActivateMultiInput() { m_IsMultiInput = true; }
    inline bool IsMultiInput() { return m_IsMultiInput; }

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

    std::string uuid;
    std::string title;
    NODE_COLOR color;
    ImVec2 position;
    ImVec2 size;
    ParamLayout m_ParamLayout;

    bool selected = false;
    bool grabbed = false;
    bool highlighted = false;

private:
    uint32_t m_NumAvailableInputs = 1;
    bool m_IsMultiInput = false;

private:
    std::array<std::shared_ptr<ImGuiNode>, MAX_N_INPUTS> inputs = {nullptr, nullptr, nullptr, nullptr};
    std::vector<std::shared_ptr<ImGuiNode>> m_MultiInput;
    std::vector<InputConnector> m_InputConnectors;
};

using NODE_COLLECTION = std::vector<std::shared_ptr<ImGuiNode>>; 
struct NodeNetwork{

    std::shared_ptr<ImGuiNode> outuput_node;
    std::vector<std::shared_ptr<ImGuiNode>> nodes;

    void AddNode(std::shared_ptr<ImGuiNode> _node) { nodes.push_back(_node); }
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

class SubnetNode : public ImGuiNode
{
public:
    SubnetNode():ImGuiNode("subnet")
    {
        SetNumAvailableInputs(4);
    }


public:
    NodeNetwork node_network;
};

template <typename T> class Node : public T {

public:
  Node(const char *_title) {
    auto node = static_cast<ImGuiNode *>(this);
    node->title = _title;
  }

  void Update() {
    auto node = static_cast<ImGuiNode *>(this);
    auto op = static_cast<T *>(this);
    auto subnet_ptr = dynamic_cast<SubnetNode*>(op);
    if(!node->IsMultiInput()) {
        
        for (uint32_t i = 0; i < MAX_N_INPUTS; i++) {
        if (node->GetInput(i) != nullptr) {
            node->GetInput(i)->Update(); /* Important !!*/
        }
        }
    }else{
        for (uint32_t i = 0; i < node->GetMultiInputCount(); i++) {
            if (node->GetMultiInput(i) != nullptr) {
                node->GetMultiInput(i)->Update(); /* Important !!*/
            }
        }

    }

    if(subnet_ptr != nullptr) {
        subnet_ptr->Update();
    }
    op->Generate();
  }

  T *ToOperator() { return static_cast<T *>(this); }
};

};

#endif