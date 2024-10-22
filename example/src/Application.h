#ifndef APPLICATION_H
#define APPLICATION_H

#include <thread>
#include <imgui.h>
#include "node_editor.h"

#include "params.h"
#include <fstream>
#include <filesystem>
#include "yaml_serialize.h"
namespace NodeEditor {

class Application
{
public:
    Application();
    ~Application();

    inline NodeManager& GetNodeManager() { return m_NodeManager; }
    bool Init();
    void Run();

public :

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