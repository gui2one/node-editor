#ifndef NODE_EDITOR_BASE_APPLICATION_H
#define NODE_EDITOR_BASE_APPLICATION_H
#pragma once
#include "NodeManager.h"
namespace NED {
struct DebugData {
  int value;
};
class BaseApplication {
 public:
  BaseApplication() {};

  bool InitGLFW();
  inline NodeManager& GetNodeManager() { return m_NodeManager; }
  inline GLFWwindow* GetNativeWindow() { return m_NativeWindow; }
  inline void SetNativeWindow(GLFWwindow* window) { m_NativeWindow = window; }

 private:
  GLFWwindow* m_NativeWindow;
  NodeManager m_NodeManager;

 private:
  DebugData* m_DebugData;
};

}  // namespace NED
#endif  // NODE_EDITOR_BASE_APPLICATION_H