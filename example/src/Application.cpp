#include "Application.h"

namespace NED {

Application::Application() {}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(GetNativeWindow());
  glfwTerminate();
}

bool Application::Init() {
  if (!InitGLFW()) {
    return false;
  }
  GetNodeManager().SetFileExtension("str3000");

  return true;
}

};  // namespace NED