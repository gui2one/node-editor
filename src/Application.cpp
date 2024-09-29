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
    m_NativeWindow = glfwCreateWindow(1280, 720, "Starter Project", NULL, NULL);

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
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
void Application::Update()
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

        ImGui::Begin("Canvas test");
            // manager.Update();
            // canvas_demo();
        ImGui::End();
        ImGuiEndFrame();

        glfwSwapBuffers(m_NativeWindow);
    }
}
