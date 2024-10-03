#include "Application.h"

Application::Application()
{

}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_NativeWindow);
    glfwTerminate();
}

bool Application::Init()
{
    if (!glfwInit())
    {
        printf("problem with GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_NativeWindow = glfwCreateWindow(m_WindowData.width, m_WindowData.height, m_WindowData.title, NULL, NULL);
    m_NodeManager.SetGLFWWindow(m_NativeWindow);
    
    if (m_NativeWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_NativeWindow);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glfwSetWindowUserPointer(m_NativeWindow, &m_WindowData);
    
    auto& node_manager = this->GetNodeManager();
    static auto& dispatcher = EventManager::GetInstance(); 
    
    glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow *window, int button, int action, int mods) {
        // WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS) {
            MouseClickEvent clickEvent(button);
            dispatcher.Dispatch(clickEvent);
        }else if( action == GLFW_RELEASE) {
            MouseReleaseEvent releaseEvent(button);
            dispatcher.Dispatch(releaseEvent);
        }   
    });

    glfwSetCursorPosCallback(m_NativeWindow, [](GLFWwindow *window, double xpos, double ypos) {

        MouseMoveEvent moveEvent((float)xpos, (float)ypos);
        dispatcher.Dispatch(moveEvent);
    });
    
    glfwSetFramebufferSizeCallback(m_NativeWindow, [](GLFWwindow *window, int width, int height) {
        WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
        data->width = width;
        data->height = height;
        glViewport(0, 0, width, height);
    });

    // add event listeners !!!
    dispatcher.Subscribe(EventType::MouseClick, [&node_manager](const Event& event) {
        node_manager.OnMouseClick(event);
    });
    dispatcher.Subscribe(EventType::MouseRelease, [&node_manager](const Event& event) {
        node_manager.OnMouseRelease(event);
    });
    dispatcher.Subscribe(EventType::MouseMove, [&node_manager](const Event& event) {
        node_manager.OnMouseMove(event);
    });
    
    ImGuiInit(m_NativeWindow);

    glViewport(0, 0, 640, 360);
    glfwSwapInterval(0);

    return true; 
}

void Application::ImGuiInit(GLFWwindow *window)
{
    // init ImGui
    // Setup Dear ImGui context

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // io.Fonts->AddFontFromFileTTF(ORBITONS_RES_DIR "/fonts/JetBrainsMono-Regular.ttf", 16);

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

void Application::ImGuiBeginFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(NULL, NULL, ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode /*|ImGuiDockNodeFlags_NoResize*/);
}

void Application::ImGuiEndFrame()
{
    // Rendering
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ImGui::EndFrame();

    ImGuiIO &io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        glfwMakeContextCurrent(backup_current_context);
    }
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_NativeWindow))
    {
        glfwPollEvents();
        ImGuiBeginFrame();

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static bool showDemoWindow = false;
        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Canvas test");

        m_NodeManager.DrawCanvas();

        ImGui::End();
        ImGui::PopStyleVar();
        
        ImGuiEndFrame();

        glfwSwapBuffers(m_NativeWindow);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
