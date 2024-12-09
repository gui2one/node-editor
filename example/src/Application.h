#ifndef APPLICATION_H
#define APPLICATION_H

#include <imgui.h>

#include <filesystem>
#include <fstream>
#include <thread>

#include "BaseApplication.h"
#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"
#include "utils.h"
#include "yaml_serialize.h"
namespace NED {

class Application : public BaseApplication {
 public:
  Application();
  ~Application();

  bool Init();
  void Run();

 public:
  ImFont* m_RegularFont;
  ImFont* m_BoldFont;

 private:
  void ImGuiInit(GLFWwindow* window);
  void ImGuiBeginFrame();
  void ImGuiEndFrame();
};

};  // namespace NED
#endif