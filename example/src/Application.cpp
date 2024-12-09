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

  GetNodeManager().ParamChangeSubscribe<double>();

  return true;
}

void Application::Run() {
  while (!glfwWindowShouldClose(GetNativeWindow())) {
    glfwWaitEvents();
    ImGuiBeginFrame();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static bool showDemoWindow = false;
    static bool first_opened = true;
    static int run_ticks = 0;
    if (showDemoWindow) {
      ImGui::ShowDemoWindow(&showDemoWindow);
    }

    auto &manager = GetNodeManager();
    // main menu bar
    auto backup_padding = ImGui::GetStyle().FramePadding;
    ImGui::GetStyle().FramePadding = ImVec2(0.0f, 10.0f);
    ImGui::BeginMainMenuBar();
    manager.BuildImGuiMainMenuBar();
    ImGui::EndMainMenuBar();
    ImGui::GetStyle().FramePadding = backup_padding;

    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    if (manager.m_ViewProps.nodeParamsOpened) {
      manager.DisplayNodeParams(manager.m_CurrentNode);
    }
    if (manager.m_ViewProps.actionManagerOpened) {
      manager.DisplayActionManager();
    }
    // ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Canvas test");

    manager.DrawCanvas();

    ImGui::End();
    ImGui::PopStyleVar();

    ImGuiEndFrame();

    if (run_ticks > 0 && first_opened) {
      manager.ViewFrameAll();
      // std::cout << "ViewFrameAll()" << std::endl;
      first_opened = false;
    } else {
      run_ticks++;
    }
    glfwSwapBuffers(GetNativeWindow());

    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

};  // namespace NED