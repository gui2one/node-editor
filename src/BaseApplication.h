#ifndef NODE_EDITOR_BASE_APPLICATION_H
#define NODE_EDITOR_BASE_APPLICATION_H
#pragma once
#include "NodeManager.h"
#include "ui_utils.h"
namespace NED {
struct DebugData {
  int value;
};

struct AppData {
  bool shouldClose = false;
  bool modal_confirm_opened = false;

  int width;
  int height;
};
class BaseApplication {
 public:
  BaseApplication() {};
  ~BaseApplication();

  bool InitGLFW();
  inline NodeManager& GetNodeManager() { return m_NodeManager; }
  inline GLFWwindow* GetNativeWindow() { return m_NativeWindow; }
  inline void SetNativeWindow(GLFWwindow* window) { m_NativeWindow = window; }

  void InitCallbacks();
  void ImGuiInit(GLFWwindow* window);
  void ImGuiBeginFrame();
  void ImGuiEndFrame();

  void ConfirmDialog();

  void UserFunction(std::function<void()> func) { m_LoopFunction = func; }
  void Run();

 private:
  GLFWwindow* m_NativeWindow;
  NodeManager m_NodeManager;

 private:
  DebugData* m_DebugData;

 public:
  ImFont* m_RegularFont;
  ImFont* m_BoldFont;
  AppData m_AppData;

  std::function<void()> m_LoopFunction;
};

}  // namespace NED
#endif  // NODE_EDITOR_BASE_APPLICATION_H