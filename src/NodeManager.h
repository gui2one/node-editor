#ifndef NODEMANAGER_H
#define NODEMANAGER_H
#pragma once

#include "ImGuiNode.h"
#include "EventManager.h"

namespace NodeEditor {

struct ConnectionProcedure{
    enum Direction{
        CHILD_TO_PARENT = 0,
        PARENT_TO_CHILD = 1
    };

    Direction direction = CHILD_TO_PARENT;
    bool started = false;
    std::shared_ptr<ImGuiNode> output_node;
    uint32_t output_index;
    std::shared_ptr<ImGuiNode> input_node;
};

class NodeManager{

public:
    NodeManager();
    ~NodeManager();

    inline void SetFonts(ImFont* _regular, ImFont* _bold) { m_RegularFont = _regular; m_BoldFont = _bold; }
    inline void SetNodes(std::vector<std::shared_ptr<ImGuiNode>> _nodes) { nodes = _nodes; }
    inline void AddNode(std::shared_ptr<ImGuiNode> _node) { nodes.push_back(_node); }
    std::shared_ptr<ImGuiNode> FindNodeByUUID(std::string uuid);


    inline std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() { return nodes; }
    inline const std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() const { return nodes; }

    void SetNodesMenu(std::function<void()> func);
    void DrawNodes();
    void DrawCanvas();

    void DisplayNodeParams(std::shared_ptr<ImGuiNode> node);

    inline void SetGLFWWindow(GLFWwindow* _window) { m_GLFWWindow = _window; }
    inline GLFWwindow* GetGLFWWindow() { return m_GLFWWindow; }

    void SetOutputNode(std::shared_ptr<ImGuiNode> node);
    std::shared_ptr<ImGuiNode> GetOutputNode() { return m_OutputNode; }
    
    /// @brief Computes result for all 'parents' of m_OutputNode.
    /// does nothing if m_OutputNode == nullptr
    void Evaluate();

    bool IsNodeHovered(std::shared_ptr<ImGuiNode> node);
    bool IsInputConnectorHovered(std::shared_ptr<ImGuiNode> node, uint32_t index);
    
    void ApplyConnectionProcedure();
    void ResetConnectionProcedure();

    void OnMouseMove(const Event& event);
    void OnMouseClick(const Event& event);
    void OnMouseDoubleClick(const Event& event);
    void OnMouseRelease(const Event& event);
    void OnKeyPress(const Event& event);

    void ViewFrameAll();

public:
    ImVec2 m_Origin;
    ImVec2 m_Scrolling = ImVec2(0,0);
    float m_Zoom = 1.0f;
    
    bool m_CanvasHovered = false;
    ImVec2 m_CanvasSize;
    ImVec2 m_CanvasPos;
    std::shared_ptr<ImGuiNode> m_CurrentNode = nullptr;

    ImFont * m_RegularFont;
    ImFont * m_BoldFont;

    bool m_OneParamChanged = false;
    std::chrono::time_point<std::chrono::system_clock> m_LastCLickReleaseTime;
private:
    std::vector<std::shared_ptr<ImGuiNode>> nodes;
    GLFWwindow* m_GLFWWindow;
    std::function<void()> m_NodesMenu = [](){};

    ConnectionProcedure m_ConnectionProcedure;
    std::shared_ptr<ImGuiNode> m_OutputNode = nullptr;

    bool m_OpenNodesMenu = false;
};


};
#endif