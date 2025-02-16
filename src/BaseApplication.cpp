#include "BaseApplication.h"

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  bool exit = false;
  const char *file = __FILE__;
  int line = __LINE__;
  // ignore non-significant error/warning codes
  // if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return ; //LOG_WARN("OpenGL Warning id {}", id);
  if (id == 131185 /* level : Notification */) return;
  std::cout << "\nOpenGL Error" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "Source: Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Source: Other";
      break;
  }
  std::cout << std::endl;

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Type: Other";
      break;
  }
  std::cout << std::endl;

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "Severity: high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "Severity: medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "Severity: low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "Severity: notification";
      break;
  }
  std::cout << std::endl;
  std::cout << std::endl;

// #define EXIT_ON_OPENGL_ERROR
#ifdef EXIT_ON_OPENGL_ERROR

  std::exit(EXIT_FAILURE);

#endif
}
namespace NED {
BaseApplication::~BaseApplication() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(GetNativeWindow());
  glfwTerminate();
}
bool BaseApplication::InitGLFW() {
  if (!glfwInit()) {
    printf("problem with GLFW\n");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  auto win = glfwCreateWindow(1920, 800, "no title yet", NULL, NULL);
  SetNativeWindow(win);
  if (GetNativeWindow() == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(GetNativeWindow());
  GetNodeManager().SetGLFWWindow(GetNativeWindow());

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return false;
  }
// #define MODERN_OGL_DEBUG
#ifdef MODERN_OGL_DEBUG
  m_DebugData = new DebugData();

  // check debug context
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    std::cout << "---- OpenGL DEBUG Context created ----" << std::endl;

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
      // initialize debug output
    }
  }
#endif
  InitCallbacks();
  GetNodeManager().EventsSubscribe();
  GetNodeManager().InitIcons();

  ImGuiInit(GetNativeWindow());

  glViewport(0, 0, 1920, 1080);
  glfwSwapInterval(0);

  return true;
}
void BaseApplication::InitCallbacks() {
  static auto &dispatcher = EventManager::GetInstance();

  glfwSetWindowUserPointer(GetNativeWindow(), &m_AppData);

  glfwSetMouseButtonCallback(GetNativeWindow(), [](GLFWwindow *window, int button, int action, int mods) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if (action == GLFW_PRESS) {
      MouseClickEvent clickEvent(button, (float)mouseX, (float)mouseY);
      dispatcher.Dispatch(clickEvent);
    } else if (action == GLFW_RELEASE) {
      MouseReleaseEvent releaseEvent(button);
      dispatcher.Dispatch(releaseEvent);
    }
  });

  glfwSetCursorPosCallback(GetNativeWindow(), [](GLFWwindow *window, double xpos, double ypos) {
    MouseMoveEvent moveEvent((float)xpos, (float)ypos);
    dispatcher.Dispatch(moveEvent);
  });

  glfwSetKeyCallback(GetNativeWindow(), [](GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      KeyPressEvent pressEvent(key, mods);
      dispatcher.Dispatch(pressEvent);
    }
  });

  glfwSetFramebufferSizeCallback(GetNativeWindow(), [](GLFWwindow *window, int width, int height) {
    auto *data = (AppData *)glfwGetWindowUserPointer(window);
    data->width = width;
    data->height = height;
    glViewport(0, 0, width, height);
  });

  glfwSetDropCallback(GetNativeWindow(), [](GLFWwindow *window, int count, const char **paths) {
    DropFileEvent event(paths[0]);
    dispatcher.Dispatch(event);
  });
  glfwSetWindowCloseCallback(GetNativeWindow(), [](GLFWwindow *window) {
    auto app_data = (AppData *)glfwGetWindowUserPointer(window);
    glfwSetWindowShouldClose(window, false);
    app_data->modal_confirm_opened = true;
  });
}
void BaseApplication::ImGuiInit(GLFWwindow *window) {
  // init ImGui
  // Setup Dear ImGui context

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  m_RegularFont = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Regular.ttf", 16);
  m_BoldFont = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-ExtraBold.ttf", 16);
  io.FontDefault = m_RegularFont;
  io.Fonts->Build();
  GetNodeManager().SetFonts(m_RegularFont, m_BoldFont);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; /* I need to remake the whole positioning system to enabled */

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char *glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version);

  // ImGui::GetStyle().FramePadding = ImVec2(6.0f, 6.0f);

  ImVec4 *colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.45f, 0.03f, 0.03f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.68f, 0.41f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
  colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
  colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
  colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
  colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

  ////////////
  // end imgui config
  ///////////
}

void BaseApplication::ImGuiBeginFrame() {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(
      NULL, NULL, ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode /*|ImGuiDockNodeFlags_NoResize*/);
}

void BaseApplication::ImGuiEndFrame() {
  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  ImGui::EndFrame();

  ImGuiIO &io = ImGui::GetIO();

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow *backup_current_context = glfwGetCurrentContext();

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    glfwMakeContextCurrent(backup_current_context);
  }
}

void BaseApplication::ConfirmDialog() {
  if (ImGui::BeginPopupModal("Confirm", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Quit the program ?");
    if (ImGui::Button("Yes", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
      m_AppData.modal_confirm_opened = false;
      glfwSetWindowShouldClose(GetNativeWindow(), true);
    }
    ImGui::SameLine();
    if (ImGui::Button("No", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
      m_AppData.modal_confirm_opened = false;
    }
    ImGui::EndPopup();
  }
}

void BaseApplication::Run() {
  while (!glfwWindowShouldClose(GetNativeWindow())) {
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
    if (m_AppData.modal_confirm_opened) {
      ImGui::OpenPopup("Confirm");
      glfwPostEmptyEvent();
    }
    ConfirmDialog();

    manager.DisplayNodeParamsOptions();
    if (manager.m_ViewProps.nodeParamsOptionsOpened) {
      ImGui::OpenPopup("Node Params Options");
      glfwPostEmptyEvent();
    }
    // main menu bar

    if (ImGui::BeginMainMenuBar()) {
      manager.BuildImGuiMainMenuBar();
      ImGui::EndMainMenuBar();
    }

    if (manager.m_ViewProps.nodeParamsOpened) {
      manager.DisplayNodeParams(manager.m_CurrentNode);
    }

    if (manager.m_ViewProps.actionManagerOpened) {
      manager.DisplayActionManager();
    }

    if (manager.m_ViewProps.show_mouse_coords) {
      manager.DisplayCoordSpacesDebug();
    }
    manager.DisplayTreeView();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    UI::Begin("Canvas test");

    manager.DrawCanvas();

    UI::End();
    ImGui::PopStyleVar();

    m_LoopFunction();

    if (run_ticks > 0 && first_opened) {
      manager.ViewFrameAll();

      first_opened = false;
    } else {
      run_ticks++;
    }

    ImGuiEndFrame();
    glfwSwapBuffers(GetNativeWindow());
    glfwWaitEventsTimeout(0.032);  // 30 fps
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
};  // namespace NED
