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
    void ImGuiInit(GLFWwindow* window);
    void ImGuiBeginFrame();
    void ImGuiEndFrame();
    void Update();

private:
    GLFWwindow* m_NativeWindow;
};

#endif