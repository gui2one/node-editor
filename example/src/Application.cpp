#include "Application.h"
namespace NodeEditor {

Application::Application() {}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(m_NativeWindow);
  glfwTerminate();
}

bool Application::Init() {
  
  if (!glfwInit()) {
    printf("problem with GLFW\n");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_NativeWindow = glfwCreateWindow(m_WindowData.width, m_WindowData.height,
                                    m_WindowData.title, NULL, NULL);
  

  if (m_NativeWindow == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(m_NativeWindow);
  m_NodeManager.SetGLFWWindow(m_NativeWindow);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return false;
  }

  glfwSetWindowUserPointer(m_NativeWindow, &m_WindowData);

  auto &node_manager = this->GetNodeManager();
  static auto &dispatcher = EventManager::GetInstance();

  glfwSetMouseButtonCallback(
      m_NativeWindow, [](GLFWwindow *window, int button, int action, int mods) {
        // WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
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

  glfwSetCursorPosCallback(m_NativeWindow,
                           [](GLFWwindow *window, double xpos, double ypos) {
                             MouseMoveEvent moveEvent((float)xpos, (float)ypos);
                             dispatcher.Dispatch(moveEvent);
                           });

  glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow *window, int key,
                                        int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      KeyPressEvent pressEvent(key);
      dispatcher.Dispatch(pressEvent);
    }
  });

  glfwSetFramebufferSizeCallback(
      m_NativeWindow, [](GLFWwindow *window, int width, int height) {
        WindowData *data = (WindowData *)glfwGetWindowUserPointer(window);
        data->width = width;
        data->height = height;
        glViewport(0, 0, width, height);
      });

  // add event listeners !!!
  dispatcher.Subscribe(EventType::MouseClick,
                       [&node_manager](const NodeEditor::Event &event) {
                         node_manager.OnMouseClick(event);
                       });
  dispatcher.Subscribe(EventType::MouseDoubleClick,
                       [&node_manager](const NodeEditor::Event &event) {
                         node_manager.OnMouseDoubleClick(event);
                        //  std::cout << "double click From Application class" << std::endl;
                         
                       });
  dispatcher.Subscribe(EventType::MouseRelease,
                       [&node_manager](const NodeEditor::Event &event) {
                         node_manager.OnMouseRelease(event);
                       });
  dispatcher.Subscribe(EventType::MouseMove,
                       [&node_manager](const NodeEditor::Event &event) {
                         node_manager.OnMouseMove(event);
                       });
  dispatcher.Subscribe(EventType::KeyPress,
                       [&node_manager](const NodeEditor::Event &event) {
                         node_manager.OnKeyPress(event);
                       });

  ImGuiInit(m_NativeWindow);

  glViewport(0, 0, 640, 360);
  glfwSwapInterval(0);

  return true;
}

void Application::ImGuiInit(GLFWwindow *window) {
  // init ImGui
  // Setup Dear ImGui context

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  
  m_RegularFont = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Regular.ttf", 16);
  m_BoldFont = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-ExtraBold.ttf", 16);
  io.FontDefault = m_RegularFont;
  io.Fonts->Build();
  m_NodeManager.SetFonts(m_RegularFont, m_BoldFont);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char *glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version);

  ////////////
  // end imgui config
  ///////////
}

void Application::ImGuiBeginFrame() {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();


  ImGui::DockSpaceOverViewport(
      NULL, NULL,
      ImGuiDockNodeFlags_None |
          ImGuiDockNodeFlags_PassthruCentralNode /*|ImGuiDockNodeFlags_NoResize*/);
}

void Application::ImGuiEndFrame() {
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

void Application::Run() {
  while (!glfwWindowShouldClose(m_NativeWindow)) {
    // glfwPollEvents();
    glfwWaitEvents();
    ImGuiBeginFrame();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static bool showDemoWindow = false;
    if (showDemoWindow) {
      ImGui::ShowDemoWindow(&showDemoWindow);
    }

    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New", "Ctrl+N")) {
        std::cout << "New file Not Implemented Yet" << std::endl;
        
      }
      if (ImGui::MenuItem("Save", "Ctrl+S")) {

        auto temp_dir = std::filesystem::temp_directory_path();

        auto save_path = temp_dir / "nodes.yaml";
        std::fstream saved_file(save_path.string(), std::ios::out);
        saved_file << serialize_nodes(m_NodeManager.GetRootNetwork().nodes);
        saved_file.close();
      }
      if( ImGui::MenuItem("Load", "Ctrl+L")) {

        auto temp_dir = std::filesystem::temp_directory_path();
        auto save_path = temp_dir / "nodes.yaml";
        std::ifstream saved_file(save_path.string());
        std::string content((std::istreambuf_iterator<char>(saved_file)), std::istreambuf_iterator<char>());
        saved_file.close();

        m_NodeManager.GotoRootNetwork();
        m_NodeManager.GetNodes().clear();
        m_NodeManager.UnsetOutputNode();
        
        auto loaded_nodes = deserialize_nodes(content);
        float x = 0.0f;
        for(auto node : loaded_nodes) {
          // node->position = ImVec2(x, 0);
          // x += 150.0f;
          m_NodeManager.AddNode(node);
        }
        m_NodeManager.ViewFrameAll();
      }
      ImGui::Separator();
      if(ImGui::MenuItem("Clear All Noes")) {
        m_NodeManager.GetNodes().clear();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {

      if (ImGui::MenuItem("Center All", "F")) {
        m_NodeManager.ViewFrameAll();
      }
      ImGui::MenuItem("Show Grid", NULL, &m_NodeManager.m_ViewProps.display_grid);
      ImGui::MenuItem("Show Mouse Coords", NULL, &m_NodeManager.m_ViewProps.show_mouse_coords);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    static bool first_opened = true;

    m_NodeManager.DisplayNodeParams(m_NodeManager.m_CurrentNode);
    ImGui::Begin("Canvas test");

    m_NodeManager.DrawCanvas();
    if (first_opened) {
      m_NodeManager.ViewFrameAll();
      first_opened = false;
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGuiEndFrame();

    glfwSwapBuffers(m_NativeWindow);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

}; // namespace NodeEditor