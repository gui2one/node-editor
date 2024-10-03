#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#pragma once

#include <vector>
#include <memory>
#include <iostream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "ImGuiNode.h"
#include "EventManager.h"
#include "GLFW/glfw3.h"


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

    inline void SetNodes(std::vector<std::shared_ptr<ImGuiNode>> _nodes) { nodes = _nodes; }
    inline void AddNode(std::shared_ptr<ImGuiNode> _node) { nodes.push_back(_node); }

    inline std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() { return nodes; }
    inline const std::vector<std::shared_ptr<ImGuiNode>>& GetNodes() const { return nodes; }

    void SetNodesMenu(std::function<void()> func);
    void DrawNodes();
    void DrawCanvas();

    inline void SetGLFWWindow(GLFWwindow* _window) { m_GLFWWindow = _window; }
    inline GLFWwindow* GetGLFWWindow() { return m_GLFWWindow; }

    void Evaluate(std::function<void()> func);


    bool IsNodeHovered(std::shared_ptr<ImGuiNode> node);
    bool IsInputConnectorHovered(std::shared_ptr<ImGuiNode> node, uint32_t index);
    void ResetConnectionProcedure();

    void OnMouseMove(const Event& event);
    void OnMouseClick(const Event& event);
    void OnMouseRelease(const Event& event);
    void OnKeyPress(const Event& event);

    void ViewFrameAll();

public:
    ImVec2 m_Origin;
    ImVec2 m_Scrolling = ImVec2(0,0);
    bool m_CanvasHovered = false;
    ImVec2 m_CanvasSize;
private:
    std::vector<std::shared_ptr<ImGuiNode>> nodes;
    GLFWwindow* m_GLFWWindow;
    std::function<void()> m_NodesMenu = [](){};

    ConnectionProcedure m_ConnectionProcedure;
};


#endif