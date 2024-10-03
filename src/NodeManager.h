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
class NodeManager
{
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

    void Evaluate();

    bool IsNodeHovered(std::shared_ptr<ImGuiNode> node);

    void OnMouseMove(const Event& event);
    void OnMouseClick(const Event& event);
    void OnMouseRelease(const Event& event);

public:
    ImVec2 m_Origin;
private:
    std::vector<std::shared_ptr<ImGuiNode>> nodes;
    GLFWwindow* m_GLFWWindow;
    std::function<void()> m_NodesMenu = [](){};
};

#endif