#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once

#include "node_editor.h"

class Application
{
public:
    Application();
    ~Application();

    bool Init();
    
    inline NodeManager& GetNodeManager() { return m_NodeManager; }
    
    void DrawCanvas();
    void SetLoopFunction(std::function<void()> func);
    void SetNodesMenu(std::function<void()> func);
    void DrawNodes();


    void MouseEvents();
    bool IsNodeHovered(std::shared_ptr<ImGuiNode> node);
    void Run();

public :
    ImVec2 m_Origin;

private:

    void ImGuiInit(GLFWwindow* window);
    void ImGuiBeginFrame();
    void ImGuiEndFrame();

private:
    GLFWwindow* m_NativeWindow;
    std::function<void()> m_LoopFunction;
    std::function<void()> m_NodesMenu = [](){};
    NodeManager m_NodeManager;
};

#endif