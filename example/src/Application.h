#ifndef APPLICATION_H
#define APPLICATION_H

#include <imgui.h>

#include <filesystem>
#include <fstream>
#include <thread>

#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"
#include "utils.h"
#include "yaml_serialize.h"

namespace NodeEditor {
struct DebugData
{
    int value;
};
class Application {
 public:
  Application();
  ~Application();

  inline NodeManager& GetNodeManager() { return m_NodeManager; }
  bool Init();
  void Run();

 public:
  ImFont* m_RegularFont;
  ImFont* m_BoldFont;

 private:
  void ImGuiInit(GLFWwindow* window);
  void ImGuiBeginFrame();
  void ImGuiEndFrame();

 private:
  GLFWwindow* m_NativeWindow;
  NodeManager m_NodeManager;

  DebugData* m_DebugData;
};

};  // namespace NodeEditor
#endif