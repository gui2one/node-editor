#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once
#include <chrono>
#include <thread>
#include "node_editor.h"

struct WindowData{
    int width = 1280;
    int height = 720;

    int mouseX = 0;
    int mouseY = 0;
    const char* title = "The Node Editor | Another one ?!!";
    
};


class Application
{
public:
    Application();
    ~Application();

    inline NodeManager& GetNodeManager() { return m_NodeManager; }
    bool Init();
    void DrawCanvas();
    void SetNodesMenu(std::function<void()> func);
    void DrawNodes();

    void Run();

public :
    ImVec2 m_Origin;
    WindowData m_WindowData;

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