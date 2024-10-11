#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once
#include <thread>
#include <imgui.h>
#include "node_editor.h"
#include "Serialize.h"
#include "params.h"
namespace NodeEditor {

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
    void Run();

public :
    ImVec2 m_Origin;
    WindowData m_WindowData;

    ImFont* m_RegularFont;
    ImFont* m_BoldFont;    

private:
    void ImGuiInit(GLFWwindow* window);
    void ImGuiBeginFrame();
    void ImGuiEndFrame();

private:
    GLFWwindow* m_NativeWindow;
    NodeManager m_NodeManager;

};

};
#endif