#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once

#include "node_editor.h"
#include <functional>
class Application
{
public:
    Application();
    ~Application();

    bool Init();
    
    void SetLoopFunction(std::function<void()> func);
    void Update();

private:

    void ImGuiInit(GLFWwindow* window);
    void ImGuiBeginFrame();
    void ImGuiEndFrame();

private:
    GLFWwindow* m_NativeWindow;
    std::function<void()> m_LoopFunction;
};

#endif