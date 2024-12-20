#include "NodeManager.h"

static GLuint GenerateEmptyTexture() {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  std::vector<unsigned char> data = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0

  };
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture;
}

static GLuint LoadTexture(const char* filename) {
  int width, height, channels;

  unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
  if (!data) {
    std::cout << "STBI Unable to load " << filename << std::endl;

    return GenerateEmptyTexture();
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  // Set texture parameters (optional)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);
  return texture;
}

namespace NED {
NodeManager::NodeManager() {
  SetNodesMenu([this]() { this->BuildNodeMenuFromRegistry(); });

  m_CurrentNetwork = &m_NodeNetwork;
  REGISTER_PARAM_TYPE(NED::ParamLabel);
  REGISTER_PARAM_TYPE(NED::ParamGroup);
  REGISTER_PARAM_TYPE(NED::ParamComboBox);
  REGISTER_PARAM_TYPE(NED::ParamFile);

  REGISTER_PARAM_TYPE(NED::ParamFloat);
  REGISTER_PARAM_TYPE(NED::ParamInt);
  REGISTER_PARAM_TYPE(NED::ParamBool);
  REGISTER_PARAM_TYPE(NED::ParamString);
  REGISTER_PARAM_TYPE(NED::ParamVec2);
  REGISTER_PARAM_TYPE(NED::ParamVec3);

  ParamChangeSubscribe<float>();
  ParamChangeSubscribe<int>();
  ParamChangeSubscribe<bool>();
  ParamChangeSubscribe<glm::vec2>();
  ParamChangeSubscribe<glm::vec3>();
  ParamChangeSubscribe<std::string>();
  ParamChangeSubscribe<std::wstring>();
}

NodeManager::~NodeManager() {}

void NodeManager::InitGLFWEvents() {
  static auto& dispatcher = EventManager::GetInstance();

  glfwSetWindowUserPointer(GetGLFWWindow(), &m_WindowData);

  glfwSetMouseButtonCallback(GetGLFWWindow(), [](GLFWwindow* window, int button, int action, int mods) {
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

  glfwSetCursorPosCallback(GetGLFWWindow(), [](GLFWwindow* window, double xpos, double ypos) {
    MouseMoveEvent moveEvent((float)xpos, (float)ypos);
    dispatcher.Dispatch(moveEvent);
  });

  glfwSetKeyCallback(GetGLFWWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      KeyPressEvent pressEvent(key, mods);
      dispatcher.Dispatch(pressEvent);
    }
  });

  glfwSetFramebufferSizeCallback(GetGLFWWindow(), [](GLFWwindow* window, int width, int height) {
    WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
    data->width = width;
    data->height = height;
    glViewport(0, 0, width, height);
  });

  glfwSetDropCallback(GetGLFWWindow(), [](GLFWwindow* window, int count, const char** paths) {
    DropFileEvent event(paths[0]);
    dispatcher.Dispatch(event);
  });
  dispatcher.Subscribe(EventType::CurrentNodeChanged, [this](const NED::Event& event) {
    auto ev = static_cast<const NED::CurrentNodeChangedEvent&>(event);
    std::cout << "CurrentNodeChanged" << std::endl;
    // auto action = std::make_shared<SelectionChangedAction>(ev.node_manager, ev.old_selection, ev.new_selection);
    // action->message = std::format("SelectionChanged from {} to {}", ev.old_selection.size(),
    // ev.new_selection.size()); ActionManager::GetInstance().executeCommand(action);
  });
  dispatcher.Subscribe(EventType::SelectionChanged, [this](const NED::Event& event) {
    auto ev = static_cast<const NED::SelectionChangedEvent&>(event);
    auto action = std::make_shared<SelectionChangedAction>(ev.node_manager, ev.old_selection, ev.new_selection);
    action->message = std::format("SelectionChanged from {} to {}", ev.old_selection.size(), ev.new_selection.size());
    ActionManager::GetInstance().executeCommand(action);
  });
  dispatcher.Subscribe(EventType::MouseClick, [this](const NED::Event& event) { this->OnMouseClick(event); });
  dispatcher.Subscribe(EventType::MouseDoubleClick,
                       [this](const NED::Event& event) { this->OnMouseDoubleClick(event); });
  dispatcher.Subscribe(EventType::MouseRelease, [this](const NED::Event& event) { this->OnMouseRelease(event); });
  dispatcher.Subscribe(EventType::MouseMove, [this](const NED::Event& event) { this->OnMouseMove(event); });
  dispatcher.Subscribe(EventType::KeyPress, [this](const NED::Event& event) { this->OnKeyPress(event); });
  dispatcher.Subscribe(EventType::DropFile, [this](const NED::Event& event) {
    auto drop_ev = static_cast<const NED::DropFileEvent&>(event);
    GotoRootNetwork();
    auto path = std::filesystem::path(drop_ev.path);

    auto net = load_yaml_file(path);
    LoadFileEvent event2(m_SavePath.string().c_str());
    EventManager::GetInstance().Dispatch(event2);

    this->GetRootNetwork() = net;

    this->m_SavePath = path;
    glfwSetWindowTitle(this->GetGLFWWindow(), path.string().c_str());
  });
  dispatcher.Subscribe(EventType::NodeConnection, [this](const Event& event) {
    Evaluate();
    auto ev = static_cast<const NodeConnectionEvent&>(event);
    auto action = std::make_shared<NodeConnectAction>(ev.new_parent_node.get(), ev.old_parent_node.get(),
                                                      ev.child_node.get(), ev.child_index);
    action->message = std::format("Node Connect");
    ActionManager::GetInstance().executeCommand(action);
  });
  dispatcher.Subscribe(EventType::NodeDisconnection, [this](const Event& event) {
    Evaluate();
    auto ev = static_cast<const NodeDisconnectionEvent&>(event);

    auto action = std::make_shared<NodeDisconnectAction>(ev.parent_node.get(), ev.child_node.get(), ev.child_index);
    action->message = std::format("Node Disconnect");
    ActionManager::GetInstance().executeCommand(action);
  });

  dispatcher.Subscribe(EventType::NodeCreated, [this](const Event& event) {
    auto ev = static_cast<const NodeCreatedEvent&>(event);
    std::cout << "Node to be Created : " << ev.node_type_name << std::endl;
    auto action = std::make_shared<NodeCreateAction>(this, m_CurrentNetwork, ev.node_type_name, ev.node_position);
    action->message = std::format("Node to be Created  : {}", ev.node_type_name);
    ActionManager::GetInstance().executeCommand(action, false);
  });
  dispatcher.Subscribe(EventType::NodeDeleted, [this](const Event& event) {
    auto ev = static_cast<const NodeDeletedEvent&>(event);
    auto action = std::make_shared<NodeDeleteAction>(this, m_CurrentNetwork, ev.node);
    action->message = std::format("Node Deleted  : {}", ev.node->title);
    ActionManager::GetInstance().executeCommand(action, false);
  });
}

void NodeManager::InitIcons() {
  // empty icon
  GLuint empty_tex_id = GenerateEmptyTexture();
  m_Icons.insert({"", empty_tex_id});
  for (auto& res : m_NodeIconsResources) {
    GLuint id = LoadTexture(res.path.string().c_str());
    m_Icons.insert({res.name, id});
  }
}

void NodeManager::AddIcon(std::string name, std::filesystem::path path) {
  m_Icons.insert({name, LoadTexture(path.string().c_str())});
}

void NodeManager::CreateAllNodes() {
  static NodeFactoryRegistry& registry = NodeFactoryRegistry::GetInstance();

  std::unordered_map<std::string, std::vector<NodeFactoryRegistryItem>> whole_thing;
  // collect items by category
  for (auto& factory : registry.GetFactories()) {
    whole_thing[factory.second.category_name].push_back(factory.second);
  }

  float x = 10.0f;
  float y = 10.0f;
  for (auto& [category_name, params] : whole_thing) {
    for (auto& item : params) {
      auto node = registry.Create(item.type_name.c_str());
      if (node != nullptr) {
        node->position = ImVec2((float)x, (float)y) - m_ViewProps.scrolling - m_ViewProps.canvasPos;
        node->parent_node = m_CurrentNetworkOwner;
        this->m_CurrentNetwork->AddNode(node);
      }
      y += node->size.y + 40.0f;
    }
    y = 0.0f;
    x += 200.0f;
  }
}

void NodeManager::BuildImGuiMainMenuBar() {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New", "Ctrl+N")) {
      ResetAll();
    }

    if (ImGui::MenuItem("Save", "Ctrl+S")) {
      SaveAll();
    }

    if (ImGui::MenuItem("Load", "Ctrl+O")) {
      LoadAll();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Clear All Nodes")) {
      GetNodes().clear();
    }

    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Edit")) {
    if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
      ActionManager::GetInstance().undo();
    }
    if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
      ActionManager::GetInstance().redo();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Clone", "Ctrl+D", false, m_CurrentNode != nullptr)) {
      auto factory_node = NodeFactoryRegistry::GetInstance().Clone(m_CurrentNode);
      GetCurrentNetwork()->AddNode(factory_node);
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("View")) {
    if (ImGui::MenuItem("Center All", "F")) {
      ViewFrameAll();
    }

    ImGui::MenuItem("Show Grid", NULL, &m_ViewProps.display_grid);
    ImGui::MenuItem("Show Mouse Coords", NULL, &m_ViewProps.show_mouse_coords);
    ImGui::Separator();

    ImGui::MenuItem("Display Node Params", NULL, &m_ViewProps.nodeParamsOpened);
    ImGui::MenuItem("Display Action Manager", NULL, &m_ViewProps.actionManagerOpened);
    ImGui::EndMenu();
  }
}

void NodeManager::ResetAll() {
  GetRootNetwork().nodes.clear();
  GetRootNetwork().outuput_node = nullptr;
  m_CurrentNode = nullptr;
  m_SavePath = std::filesystem::path("");
  ActionManager::GetInstance().Reset();
  glfwSetWindowTitle(GetGLFWWindow(), m_SavePath.string().c_str());
}

void NodeManager::SetNodesMenu(std::function<void()> func) { m_NodesMenu = func; }

void NodeManager::BuildNodeMenuFromRegistry() {
  static NodeFactoryRegistry& registry = NodeFactoryRegistry::GetInstance();

  std::unordered_map<std::string, std::vector<NodeFactoryRegistryItem>> whole_thing;

  // Collect items by category
  for (auto& factory : registry.GetFactories()) {
    whole_thing[factory.second.category_name].push_back(factory.second);
  }

  // Helper lambda to split category strings
  auto split_category = [](const std::string& category) -> std::vector<std::string> {
    std::vector<std::string> parts;
    std::stringstream ss(category);
    std::string part;
    while (std::getline(ss, part, '/')) {
      parts.push_back(part);
    }
    return parts;
  };

  // Recursive function to create hierarchical menus
  std::function<void(const std::vector<std::string>&, const std::vector<NodeFactoryRegistryItem>&)> create_menu;
  create_menu = [&](const std::vector<std::string>& path, const std::vector<NodeFactoryRegistryItem>& items) {
    if (path.empty()) return;

    if (ImGui::BeginMenu(path.front().c_str())) {
      if (path.size() == 1) {
        // Add items to this menu
        for (const auto& item : items) {
          if (ImGui::MenuItem(item.label.c_str(), nullptr, false, true)) {
            auto node = registry.Create(item.type_name.c_str(), m_CurrentNetworkOwner);
            if (node != nullptr) {
              double x, y;
              glfwGetCursorPos(this->GetGLFWWindow(), &x, &y);
              node->position = ImVec2((float)x, (float)y) - m_ViewProps.scrolling - m_ViewProps.canvasPos;
              node->parent_node = m_CurrentNetworkOwner;

              NodeCreatedEvent event(this->m_CurrentNetwork, node->m_TypeName, node->position);
              EventManager::GetInstance().Dispatch(event);
            }
          }
        }
      } else {
        // Continue creating submenus
        create_menu({path.begin() + 1, path.end()}, items);
      }
      ImGui::EndMenu();
    }
  };

  // Process each category
  for (auto& [category_name, items] : whole_thing) {
    std::vector<std::string> path = split_category(category_name);
    create_menu(path, items);
  }
}

void NodeManager::DrawNodes() {
  if (m_CurrentNetwork == nullptr) {
    return;
  }

  m_ViewProps.canvasPos = ImGui::GetCursorScreenPos();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  // display something to recognize m_OutputNode
  for (auto node : GetNodes()) {
    if (node == m_CurrentNetwork->outuput_node) {
      draw_list->AddCircleFilled(ToScreenSpace(node->position + (node->size / 2.0f) * m_ViewProps.zoom),
                                 35.0f * m_ViewProps.zoom, NODE_COLOR::BROWN);
      break;
    }
  }
  // draw connections first
  for (auto node : GetNodes()) {
    auto ptr = static_cast<AbstractNode*>(node.get());
    for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
      if (ptr->GetInput(i) != nullptr) {
        auto other = ptr->GetInput(i);
        auto input_conn = ptr->GetInputConnector(i);
        ImVec2 p0 = ToScreenSpace(node->position + input_conn->relative_pos);
        ImVec2 other_pos = ToScreenSpace(other->position + ImVec2(other->size.x / 2.0f, other->size.y));

        float x_sep = glm::abs(other_pos.x - p0.x);
        float y_sep = other_pos.y - p0.y;
        float ctrl_y_offset = y_sep * glm::min(1.0f, (x_sep / 300.0f));
        ImVec2 ctrl1 = p0 + ImVec2(0, ctrl_y_offset);
        ImVec2 ctrl2 = other_pos - ImVec2(0, ctrl_y_offset);

        draw_list->AddBezierCubic(p0, ctrl1, ctrl2, other_pos, NODE_COLOR::GREY,
                                  2.0f);  // ImDrawList API uses screen coordinates()
      }
    }

    // for multi input
    float step_size = (node->size.x * 0.8f) / node->GetMultiInputCount();
    ImVec2 start_x = node->position + ImVec2((node->size.x * 0.1f), -8.0f);
    for (uint32_t i = 0; i < node->GetMultiInputCount(); i++) {
      if (ptr->GetMultiInput(i) != nullptr) {
        auto other = ptr->GetMultiInput(i);
        ImVec2 p0 = ToScreenSpace(start_x + ImVec2((step_size * i) + (step_size / 2.0f), 0));
        ImVec2 other_pos = ToScreenSpace(other->position + ImVec2(other->size.x / 2.0f, other->size.y));

        float x_sep = glm::abs(other_pos.x - p0.x);
        float y_sep = other_pos.y - p0.y;
        float ctrl_y_offset = y_sep * glm::min(1.0f, (x_sep / 300.0f));
        ImVec2 ctrl1 = p0 + ImVec2(0, ctrl_y_offset);
        ImVec2 ctrl2 = other_pos - ImVec2(0, ctrl_y_offset);

        draw_list->AddBezierCubic(p0, ctrl1, ctrl2, other_pos, NODE_COLOR::GREY,
                                  2.0f);  // ImDrawList API uses screen coordinates()
      }
    }
  }

  if (m_ConnectionProcedure.started) {
    if (m_ConnectionProcedure.child_node->IsMultiInput()) {
      ImVec2 input_conn_pos = m_ConnectionProcedure.child_node->position;
      // ImVec2 connector_pos = input_conn_pos + m_ConnectionProcedure.output_node->position;
      ImVec2 p0 = ToScreenSpace(input_conn_pos);
      double x, y;
      glfwGetCursorPos(m_GLFWWindow, &x, &y);
      ImVec2 cursor_pos = ImVec2((float)x, (float)y);
      ImVec2 p1 = cursor_pos;
      draw_list->AddBezierCubic(p0, p0 - ImVec2(0, 100), p1 + ImVec2(0, 100), p1, NODE_COLOR::ORANGE, 2.0f);
    } else {
      ImVec2 input_conn_pos =
          m_ConnectionProcedure.child_node->GetInputConnector(m_ConnectionProcedure.child_index)->relative_pos;
      ImVec2 connector_pos = input_conn_pos + m_ConnectionProcedure.child_node->position;
      ImVec2 p0 = ToScreenSpace(connector_pos);
      double x, y;
      glfwGetCursorPos(m_GLFWWindow, &x, &y);
      ImVec2 cursor_pos = ImVec2((float)x, (float)y);
      ImVec2 p1 = cursor_pos;
      draw_list->AddBezierCubic(p0, p0 - ImVec2(0, 100), p1 + ImVec2(0, 100), p1, NODE_COLOR::ORANGE, 2.0f);
    }
  }

  for (auto node : GetNodes()) {
    auto ptr = static_cast<AbstractNode*>(node.get());

    // input 'connectors'
    for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
      auto input_conn = ptr->GetInputConnector(i);
      ImVec2 cp = ToScreenSpace(node->position + input_conn->relative_pos);
      draw_list->AddCircleFilled(cp, 5.0f * m_ViewProps.zoom, NODE_COLOR::WHITE);
      if (input_conn->hovered) {
        draw_list->AddCircle(cp, 5.0f * m_ViewProps.zoom, NODE_COLOR::ORANGE, 0, 3.0f);
      }
    }

    if (node->IsMultiInput()) {
      ImVec2 p_left = ToScreenSpace(node->position + ImVec2(node->size.x * 0.1f, 2));
      ImVec2 p_right = ToScreenSpace(node->position + ImVec2(node->size.x - node->size.x * 0.1f, 2));

      draw_list->AddRectFilled(p_left - ImVec2(0, 10.0f), p_right, NODE_COLOR::WHITE, 5.0f);
      if (IsNodeMultiInputConnectorHovered(node)) {
        draw_list->AddRect(p_left - ImVec2(0, 10.0f), p_right, NODE_COLOR::RED, 5.0f, 0, 3.0f);
      }
    }

    // output 'connector'
    ImVec2 cp = node->position + ImVec2(node->size.x / 2.0f, node->size.y);
    cp = ToScreenSpace(cp);
    draw_list->AddCircleFilled(cp, 5.0f * m_ViewProps.zoom, NODE_COLOR::WHITE);

    ImVec2 min = ToScreenSpace(node->position);
    ImVec2 max = min + (node->size * m_ViewProps.zoom);
    draw_list->AddRectFilled(min, max, node->color, 3.0f);

    if (node->icon_name != "") {
      ImVec2 uv0(0, 0);  // Top-left of the texture
      ImVec2 uv1(1, 1);  // Bottom-right of the texture
      ImVec2 node_center = min + (node->size * 0.5f * m_ViewProps.zoom);
      ImVec2 icon_min = node_center - ImVec2(15.0f, 15.0f) * m_ViewProps.zoom;
      ImVec2 icon_max = node_center + ImVec2(15.0f, 15.0f) * m_ViewProps.zoom;
      // glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)m_Icons[node->icon_name]);
      // m_Icons.at(node->icon_name);
      if (m_Icons.at((const char*)node->icon_name) != 0) {
        draw_list->AddImage((ImTextureID)(intptr_t)m_Icons.at((const char*)node->icon_name), icon_min, icon_max, uv0,
                            uv1);
      }
    }

    if (node->selected) ImGui::PushFont(m_BoldFont);
    draw_list->AddText(max + ImVec2(5.0f, -25.0f), IM_COL32(255, 255, 255, 255), node->title.c_str());
    if (node->selected) ImGui::PopFont();

    if (!node->highlighted) {
      draw_list->AddRect(min, max, NODE_COLOR::DARK_GREY, 3.0f);
    } else {
      draw_list->AddRect(min, max, NODE_COLOR::LIGHT_GREY, 3.0f);
    }

    if (node->selected) {
      draw_list->AddRect(min, max, IM_COL32(200, 200, 60, 100), 3.0f, 0, 3.0f);
    }
  }

  DisplayNavBar();
}

void NodeManager::DrawCanvas() {
  m_ViewProps.canvasHovered = ImGui::IsWindowHovered();

  m_ViewProps.canvasSize = ImGui::GetWindowSize();
  static ImVector<ImVec2> points;

  ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();     // ImDrawList API uses screen coordinates!!!
  ImVec2 canvas_sz = ImGui::GetContentRegionAvail();  // Resize canvas to what's available
  if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
  if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
  ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

  // Draw border and background color
  ImGuiIO& io = ImGui::GetIO();
  if (io.MouseWheel > 0.0f && m_ViewProps.canvasHovered) {
    m_ViewProps.zoom *= 1.1f;
  } else if (io.MouseWheel < 0.0f && m_ViewProps.canvasHovered) {
    m_ViewProps.zoom /= 1.1f;
  }

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
  draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

  if (m_ViewProps.nodes_menu_opened) {
    ImGui::OpenPopup("context", 1);
    m_ViewProps.nodes_menu_opened = false;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
  if (ImGui::BeginPopup("context")) {
    ImGui::PushStyleColor(ImGuiCol_Text, NODE_COLOR::YELLOW);
    ImGui::Text("Add Node");
    ImGui::PopStyleColor(1);

    m_NodesMenu();

    ImGui::EndPopup();
  }
  ImGui::PopStyleVar();
  // Draw grid
  draw_list->PushClipRect(canvas_p0, canvas_p1, true);

  // 0, 0 marker
  float maker_size = 10.0f;
  draw_list->AddLine(ToScreenSpace(ImVec2(0, maker_size / 2.0f)), ToScreenSpace(ImVec2(0, -maker_size / 2.0f)),
                     NODE_COLOR::YELLOW, 1.0f);
  draw_list->AddLine(ToScreenSpace(ImVec2(maker_size / 2.0f, 0)), ToScreenSpace(ImVec2(-maker_size / 2.0f, 0)),
                     NODE_COLOR::YELLOW, 1.0f);
  if (m_ViewProps.display_grid) {
    const float GRID_STEP = 50.0f;
    for (float x = fmodf(m_ViewProps.scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
      draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y),
                         IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(m_ViewProps.scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
      draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y),
                         IM_COL32(200, 200, 200, 40));
  }

  DrawNodes();

  // selection rectangle
  if (m_ViewProps.rectangleSelectionStarted) {
    draw_list->AddRect(ToScreenSpace(m_ViewProps.rectangleSelectionStartPoint),
                       ToScreenSpace(m_ViewProps.rectangleSelectionEndPoint), NODE_COLOR::YELLOW);
  }
  // debug draw
  // ImVec2 raw_pos = io.MousePos;
  // if (m_ViewProps.show_mouse_coords) {
  //  auto canvas_space = ToCanvasSpace(raw_pos);
  //  int x, y;
  //  glfwGetWindowPos(m_GLFWWindow, &x, &y);
  //  auto screen_space = ImVec2((float)x, (float)y) + raw_pos;
  //  std::string txt =
  //      "canvas space (" + std::to_string((int)canvas_space.x) + ", " + std::to_string((int)canvas_space.y) + ")";
  //  std::string txt_raw_pos =
  //      "window space(" + std::to_string((int)raw_pos.x) + ", " + std::to_string((int)raw_pos.y) + ")";
  //  std::string txt_screen_pos =
  //      "screen space(" + std::to_string((int)screen_space.x) + ", " + std::to_string((int)screen_space.y) + ")";
  //  draw_list->AddText(raw_pos + ImVec2(20, 0), IM_COL32(255, 255, 255, 255), (const char*)txt.c_str());
  //  draw_list->AddText(raw_pos + ImVec2(20, 20), IM_COL32(255, 255, 255, 255), (const char*)txt_raw_pos.c_str());
  //  draw_list->AddText(raw_pos + ImVec2(20, 40), IM_COL32(255, 255, 255, 255), (const char*)txt_screen_pos.c_str());
  //}

  // All drawing finishes here
  draw_list->PopClipRect();
}

void NodeManager::DisplayCoordSpacesDebug() {
  ImVec2 raw_pos = ImGui::GetMousePos();
  ;
  auto canvas_space = ToCanvasSpace(raw_pos);
  int x, y;
  glfwGetWindowPos(m_GLFWWindow, &x, &y);
  auto screen_space = ImVec2((float)x, (float)y) + raw_pos;
  std::string txt =
      "canvas space (" + std::to_string((int)canvas_space.x) + ", " + std::to_string((int)canvas_space.y) + ")";
  std::string txt_raw_pos =
      "window space(" + std::to_string((int)raw_pos.x) + ", " + std::to_string((int)raw_pos.y) + ")";
  std::string txt_screen_pos =
      "screen space(" + std::to_string((int)screen_space.x) + ", " + std::to_string((int)screen_space.y) + ")";
  // draw_list->AddText(raw_pos + ImVec2(20, 0), IM_COL32(255, 255, 255, 255), (const char*)txt.c_str());
  // draw_list->AddText(raw_pos + ImVec2(20, 20), IM_COL32(255, 255, 255, 255), (const char*)txt_raw_pos.c_str());
  // draw_list->AddText(raw_pos + ImVec2(20, 40), IM_COL32(255, 255, 255, 255), (const char*)txt_screen_pos.c_str());
  auto mouse_pos = ImGui::GetMousePos();
  ImGui::SetNextWindowPos(mouse_pos);
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs |
                           ImGuiWindowFlags_NoDocking;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
  ImGui::Begin("Coord Spaces", NULL, flags);

  ImGui::Text("%s", txt.c_str());
  ImGui::Text("%s", txt_raw_pos.c_str());
  ImGui::Text("%s", txt_screen_pos.c_str());
  ImGui::End();
  ImGui::PopStyleVar();
}

ImVec2 NodeManager::ToCanvasSpace(ImVec2 pos) const {
  return (pos - m_ViewProps.scrolling - m_ViewProps.canvasPos) * (1.0f / m_ViewProps.zoom);
}

ImVec2 NodeManager::ToScreenSpace(ImVec2 pos) const {
  return (pos + m_ViewProps.scrolling + m_ViewProps.canvasPos) * (m_ViewProps.zoom);
}

void NodeManager::DisplayActionManager() {
  if (!UI::Begin("Action Manager", &m_ViewProps.actionManagerOpened, 0)) {
    // Early out if the window is collapsed, as an optimization.
    UI::End();
    return;
  }

  ImGui::BeginTable("ActionManagerTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV);
  auto& mngr = ActionManager::GetInstance();

  ImGui::TableNextColumn();
  ImGui::Text("Undos");
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
  if (ImGui::BeginListBox("##undos", ImVec2(0, ImGui::GetContentRegionAvail().y - 100))) {
    for (auto& undo_message : mngr.GetUndoMessages()) {
      ImGui::Text("%s", undo_message.data());
    }

    ImGui::EndListBox();
  }

  ImGui::PopItemWidth();

  ImGui::TableNextColumn();
  ImGui::Text("Redos");
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
  if (ImGui::BeginListBox("##redos", ImVec2(0, ImGui::GetContentRegionAvail().y - 100))) {
    for (auto& redo_message : mngr.GetRedoMessages()) {
      ImGui::Text("%s", redo_message.data());
    }

    ImGui::EndListBox();
  }

  ImGui::PopItemWidth();

  ImGui::EndTable();

  static int cur_action = 0;
  static int temp_action = 0;
  if (mngr.GetUndoMessages().size() > 0 || mngr.GetRedoMessages().size() > 0) {
    ImGui::Text("Timeline");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::SliderInt("##Timeline", &temp_action, 0,
                         (int)mngr.GetUndoMessages().size() + (int)mngr.GetRedoMessages().size())) {
      int diff = cur_action - temp_action;

      cur_action = temp_action;

      if (diff > 0) {
        for (int i = 0; i < diff; i++) {
          mngr.redo();
        }
      } else if (diff < 0) {
        for (int i = 0; i < -diff; i++) {
          mngr.undo();
        }
      }
    }
  }

  if (ImGui::Button("Clear")) {
    mngr.Reset();
  }
  ImGui::End();
}

void NodeManager::DisplayNavBar() {
  int nstylevar = 0;
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
  nstylevar++;
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  nstylevar++;
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
  nstylevar++;
  std::vector<NodeNetwork*> subnetworks;
  if (m_CurrentNetwork->owner != nullptr) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    auto cursor = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();

    float height = 30.0f;
    draw_list->AddRectFilled(cursor, ImVec2(cursor.x + ImGui::GetContentRegionAvail().x, cursor.y + height),
                             NODE_COLOR::DARK_GREY);

    if (ImGui::Button("Root")) {
      GotoRootNetwork();
    }
    auto cur_net = m_CurrentNetwork;
    while (cur_net->owner != nullptr) {
      if (cur_net->owner->IsSubnet() == false) {
        break;
      }

      subnetworks.push_back(cur_net);

      if (cur_net->owner->parent_node == nullptr) {
        break;
      }
      cur_net = &cur_net->owner->parent_node->node_network;
    }
  }

  std::reverse(subnetworks.begin(), subnetworks.end());
  for (size_t i = 0; i < subnetworks.size(); i++) {
    auto net = subnetworks[i];
    ImGui::SameLine();
    ImGui::Text("/");
    ImGui::SameLine();
    if (i < subnetworks.size() - 1) {
      if (ImGui::Button(net->owner->title.c_str())) {
        m_CurrentNetwork = net;
      }
    } else {
      ImGui::Text("%s", net->owner->title.c_str());
    }
  }

  ImGui::PopStyleVar(nstylevar);
}

std::vector<AbstractNode*> NodeManager::GetSelectedNodes() {
  std::vector<AbstractNode*> selection;
  for (auto node : GetNodes()) {
    if (node->selected) {
      selection.push_back(node.get());
    }
  }

  return selection;
}

void NodeManager::UpdateSelection() {
  auto selected_nodes = GetSelectedNodes();
  if (Utils::compare_selections(selected_nodes, m_ViewProps.selected_nodes) == false) {
    SelectionChangedEvent selectionChangedEvent(this, selected_nodes, m_ViewProps.selected_nodes);
    EventManager::GetInstance().Dispatch(selectionChangedEvent);
    m_ViewProps.selected_nodes = selected_nodes;
  }
}
void NodeManager::SetCurrentNode(std::shared_ptr<AbstractNode> node) {
  auto old_current = m_CurrentNode;
  m_CurrentNode = node;
  if (m_CurrentNode != old_current) {
    CurrentNodeChangedEvent currentChangedEvent(m_CurrentNode.get(), old_current.get());
    EventManager::GetInstance().Dispatch(currentChangedEvent);
  }
}
void NodeManager::tree_view_recurse(NodeNetwork* network) {
  for (auto node : network->nodes) {
    if (node->IsSubnet()) {
      if (ImGui::TreeNode(node->title.c_str())) {
        tree_view_recurse(&node->node_network);

        ImGui::TreePop();
      }
    } else {
      if (ImGui::Selectable(node->title.c_str(), false)) {
        if (node->parent_node != nullptr) {
          std::cout << node->parent_node->title << std::endl;
          m_CurrentNetworkOwner = node->parent_node;
          m_CurrentNetwork = &node->parent_node->node_network;
          m_CurrentNetwork->owner = node->parent_node->get_shared_ptr();
        } else {
          GotoRootNetwork();
        }
      }
    }
  }
}
void NodeManager::DisplayTreeView() {
  ImGui::Begin("Tree View");

  tree_view_recurse(&GetRootNetwork());
  ImGui::End();
}

void NodeManager::DisplayNodeParams(std::shared_ptr<AbstractNode> node) {
  // static bool NodeParams_opened = true;
  if (!UI::Begin("Params", &m_ViewProps.nodeParamsOpened, 0)) {
    // Early out if the window is collapsed, as an optimization.
    UI::End();
    return;
  }

  if (node != nullptr) {
    std::string temp_name = node->title;
    std::string name_copy = std::string(node->title);

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    if (ImGui::InputText("Node Name", &name_copy, flags)) {
      std::unordered_set<std::string> names;

      for (auto other_node : m_CurrentNetwork->nodes) {
        if (other_node != node) {
          names.insert(other_node->title);
        }
      }
      node->title = generate_unique_name(name_copy, names);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, NODE_COLOR::LIGHT_GREY);
    ImGui::Text("Node Type: %s", node->m_TypeName);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    if (ImGui::Button("Options")) {
      std::cout << "Options" << std::endl;
      OpenNodeParamsOptionsPopup();
    }
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    for (auto param : node->m_ParamLayout.params) {
      param->Display();
    }
  }

  ImGui::End();
}

void NodeManager::params_options_buttons(std::shared_ptr<NodeParam> param, int inc) {
  static int current_item = -1;

  auto group_p = std::dynamic_pointer_cast<ParamGroup>(param);
  if (group_p != nullptr) {
    for (auto _param : group_p->params) {
      params_options_buttons(_param, inc++);
    }
  } else {
    if (ImGui::Selectable(param->m_Label, current_item == inc)) {
      m_ViewProps.currentParam = param;
      current_item = inc;
    }
  }
  inc++;
}

void NodeManager::DisplayNodeParamsOptions() {
  static bool s_ParamOptionsOpened = true;

  if (ImGui::BeginPopupModal("Node Params Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("There ... some important content...");
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;
    if (ImGui::BeginTable("ItemPropertiesTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {
      ImGui::TableNextColumn();
      for (auto param : m_CurrentNode->m_ParamLayout.params) {
        params_options_buttons(param);
      }
      ImGui::TableNextColumn();

      if (m_ViewProps.currentParam != nullptr) {
        m_ViewProps.currentParam->DisplayOptions();
      }

      ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50.0f);
    if (ImGui::Button("close##modal")) {
      ImGui::CloseCurrentPopup();
      m_ViewProps.nodeParamsOptionsOpened = false;
      m_ViewProps.currentParam = nullptr;
    }
    ImGui::EndPopup();
  }
}

void NodeManager::OpenNodeParamsOptionsPopup() { m_ViewProps.nodeParamsOptionsOpened = true; }

void NodeManager::Evaluate() {
  if (m_CurrentNetwork != nullptr) {
    if (m_CurrentNetwork->outuput_node != nullptr) {
      m_CurrentNetwork->outuput_node->Update();
    }
  }
}

void NodeManager::SetOutputNode(std::shared_ptr<AbstractNode> node) {
  // m_OutputNode = node;
  if (m_CurrentNetwork != nullptr) {
    m_CurrentNetwork->outuput_node = node;
  }
}

bool NodeManager::IsNodeHovered(std::shared_ptr<AbstractNode> node) {
  ImVec2 min = ToScreenSpace(node->position);
  ImVec2 max = min + node->size;
  return ImGui::IsMouseHoveringRect(min, max, false);
}

bool NodeManager::IsInputConnectorHovered(std::shared_ptr<AbstractNode> node, uint32_t index) {
  double cursor_x, cursor_y;
  glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
  bool hovered = false;

  auto ptr = static_cast<AbstractNode*>(node.get());
  InputConnector* connector = ptr->GetInputConnector(index);
  ImVec2 min = ToScreenSpace(node->position + connector->relative_pos -
                             ImVec2(connector->width / 2.0f, connector->width / 2.0f));
  ImVec2 max = min + ImVec2(connector->width, connector->width);
  return ImGui::IsMouseHoveringRect(min, max, false);
}

bool NodeManager::IsNodeMultiInputConnectorHovered(std::shared_ptr<AbstractNode> node) {
  if (!node->IsMultiInput()) return false;

  auto p_min = ToScreenSpace(node->position + ImVec2(node->size.x * 0.1f, -10.0f));
  auto p_max = p_min + ImVec2(node->size.x * 0.9f, 10.0f);
  if (ImGui::IsMouseHoveringRect(p_min, p_max, false)) {
    return true;
  }

  return false;
}

void NodeManager::ApplyConnectionProcedure() {
  if (m_ConnectionProcedure.child_node == m_ConnectionProcedure.parent_node) {
    return;
  }
  if (m_ConnectionProcedure.is_mutli_input) {
    if (m_ConnectionProcedure.parent_node == nullptr) {
      AbstractNode* last_input_node =
          m_ConnectionProcedure.child_node->m_MultiInput[m_ConnectionProcedure.child_node->m_MultiInput.size() - 1];
      NodeDisconnectionEvent event(last_input_node->get_shared_ptr(), 0, m_ConnectionProcedure.child_node,
                                   m_ConnectionProcedure.child_index);
      EventManager::GetInstance().Dispatch(event);
      ResetConnectionProcedure();

    } else {
      NodeConnectionEvent event(m_ConnectionProcedure.parent_node, 0, m_ConnectionProcedure.child_node,
                                m_ConnectionProcedure.child_index);
      auto input_op = m_ConnectionProcedure.child_node->GetInput(m_ConnectionProcedure.child_index);
      if (input_op != nullptr) {
        event.old_parent_node = input_op->get_shared_ptr();
      } else {
        event.old_parent_node = nullptr;
      }
      EventManager::GetInstance().Dispatch(event);
      ResetConnectionProcedure();
    }
  } else {
    NodeConnectionEvent event(m_ConnectionProcedure.parent_node, 0, m_ConnectionProcedure.child_node,
                              m_ConnectionProcedure.child_index);
    auto input_op = m_ConnectionProcedure.child_node->GetInput(m_ConnectionProcedure.child_index);
    if (input_op != nullptr) {
      event.old_parent_node = input_op->get_shared_ptr();
    } else {
      event.old_parent_node = nullptr;
    }
    EventManager::GetInstance().Dispatch(event);
    ResetConnectionProcedure();
  }
}

void NodeManager::ResetConnectionProcedure() {
  m_ConnectionProcedure.started = false;
  m_ConnectionProcedure.is_mutli_input = false;
  m_ConnectionProcedure.parent_node = nullptr;
  m_ConnectionProcedure.child_node = nullptr;
  m_ConnectionProcedure.child_index = 0;
}

void NodeManager::SaveAll() {
  if (m_SavePath.empty()) {
    auto path = Utils::open_file_explorer({{"Node-Editor Network Files", m_FileExtension}}, true);
    if (path.extension().string() != std::string("." + std::string(m_FileExtension))) {
      std::cout << "Wrong File extension -> " << path.extension() << std::endl;
      std::cout << "Good extension is : " << m_FileExtension << std::endl;
      std::cout << "path parent : " << path.parent_path() << std::endl;

      // force good extension
      path = path.parent_path() / (path.stem().string() + std::string("." + std::string(m_FileExtension)));
      // check if path already exist ... after adding extension
      if (std::filesystem::exists(path)) {
        std::cout << "path already exist : " << path << std::endl;
        return;
      }
    }
    m_SavePath = path;
    glfwSetWindowTitle(m_GLFWWindow, path.string().c_str());
  }
  save_all(m_SavePath, GetRootNetwork());

  SaveFileEvent event(m_SavePath.string().c_str());
  EventManager::GetInstance().Dispatch(event);
}

void NodeManager::LoadAll() {
  if (m_SavePath.empty()) {
    auto path = Utils::open_file_explorer({{"Node-Editor Network Files", m_FileExtension}});
    m_SavePath = path;
    glfwSetWindowTitle(m_GLFWWindow, path.string().c_str());
  }

  if (!m_SavePath.empty()) {
    NodeNetwork net = load_yaml_file(m_SavePath);
    LoadFileEvent event(m_SavePath.string().c_str());
    EventManager::GetInstance().Dispatch(event);
    m_NodeNetwork = net;
    ViewFrameAll();
  }
}

void NodeManager::LoadFromFile(std::filesystem::path path) {
  if (!path.empty()) {
    m_SavePath = path;
    NodeNetwork net = load_yaml_file(path);
    LoadFileEvent event(m_SavePath.string().c_str());
    EventManager::GetInstance().Dispatch(event);
    m_NodeNetwork = net;
    ViewFrameAll();
    glfwSetWindowTitle(m_GLFWWindow, path.string().c_str());
  }
}

void NodeManager::ViewFrameAll() {
  ImVec2 center = Utils::get_nodes_center(GetNodes());
  m_ViewProps.scrolling = -center + m_ViewProps.canvasSize / 2.0f;
}

void NodeManager::OnMouseMove(const Event& event) {
  if (!m_ViewProps.canvasHovered) return;

  const MouseMoveEvent& moveEvent = static_cast<const MouseMoveEvent&>(event);
  static ImVec2 old_pos = ImVec2(0, 0);
  ImVec2 delta = ImVec2(moveEvent.x, moveEvent.y) - old_pos;
  std::shared_ptr<AbstractNode> hovered_node = nullptr;

  if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
    m_ViewProps.scrolling += delta;
  }

  if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_ViewProps.rectangleSelectionStarted) {
    m_ViewProps.rectangleSelectionEndPoint = ToCanvasSpace(ImVec2(moveEvent.x, moveEvent.y));
  }

  for (auto node : GetNodes()) {
    node->highlighted = false;
    if (node->selected && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !m_ViewProps.rectangleSelectionStarted) {
      node->position += delta;
    }
    if (IsNodeHovered(node)) {
      hovered_node = node;
    }

    for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
      auto ptr = static_cast<AbstractNode*>(node.get());
      InputConnector* connector = ptr->GetInputConnector(i);
      connector->hovered = IsInputConnectorHovered(node, i);
    }
  }
  old_pos = ImVec2(moveEvent.x, moveEvent.y);

  if (hovered_node != nullptr) {
    hovered_node->highlighted = true;
  }

  float start_x = m_ViewProps.rectangleSelectionStartPoint.x;
  float start_y = m_ViewProps.rectangleSelectionStartPoint.y;
  float end_x = m_ViewProps.rectangleSelectionEndPoint.x;
  float end_y = m_ViewProps.rectangleSelectionEndPoint.y;

  if (m_ViewProps.rectangleSelectionStarted) {
    for (auto node : GetNodes()) {
      auto sel_rect =
          Utils::selection_rect(m_ViewProps.rectangleSelectionStartPoint, m_ViewProps.rectangleSelectionEndPoint);
      node->selected = Utils::node_in_rect(node.get(), sel_rect);
    }
  }
}

void NodeManager::OnMouseClick(const Event& event) {
  if (!m_ViewProps.canvasHovered) return;

  const MouseClickEvent& clickEvent = static_cast<const MouseClickEvent&>(event);
  bool clicked_something = false;
  for (auto node : GetNodes()) {
    bool node_hovered = IsNodeHovered(node);
    if (node_hovered) {
      SetCurrentNode(node);
      clicked_something = true;
      m_ViewProps.node_clicked = node;
      m_ViewProps.node_clicked_position = node->position;
    }

    if (node_hovered && node->selected == false && m_ViewProps.rectangleSelectionStarted == false) {
      if (ImGui::GetIO().KeyCtrl == false) {
        Utils::deselect_all(GetNodes());
      }
      node->selected = true;
    } else {
    }

    if (m_ConnectionProcedure.started && node_hovered) {
      clicked_something = true;
      m_ConnectionProcedure.parent_node = node;

      ApplyConnectionProcedure();
      Evaluate();

      DISPATCH_EDITOR_UPDATE_EVENT();
    }
    if (!node->IsMultiInput()) {
      for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
        auto ptr = static_cast<AbstractNode*>(node.get());
        InputConnector* connector = ptr->GetInputConnector(i);
        if (connector->hovered) {
          clicked_something = true;
          connector->grabbed = true;

          m_ConnectionProcedure.started = true;
          m_ConnectionProcedure.child_node = node;
          m_ConnectionProcedure.child_index = i;
        }
      }
    } else {
      if (IsNodeMultiInputConnectorHovered(node)) {
        clicked_something = true;
        m_ConnectionProcedure.started = true;
        m_ConnectionProcedure.is_mutli_input = true;

        m_ConnectionProcedure.child_node = node;
        // m_ConnectionProcedure.child_index = 0;
      }
    }
  }

  if (!clicked_something) {
    Utils::deselect_all(GetNodes());
    if (m_ConnectionProcedure.started) {
      if (m_ConnectionProcedure.is_mutli_input) {  // multi input
        ApplyConnectionProcedure();
      } else {
        NodeDisconnectionEvent event(
            m_ConnectionProcedure.child_node->GetInput(m_ConnectionProcedure.child_index)->get_shared_ptr(), 0,
            m_ConnectionProcedure.child_node, m_ConnectionProcedure.child_index);
        EventManager::GetInstance().Dispatch(event);
        m_ConnectionProcedure.started = false;
        m_ConnectionProcedure.child_node->ResetInput(m_ConnectionProcedure.child_index);  // redundant ?
        ResetConnectionProcedure();
      }
    }

    m_ViewProps.rectangleSelectionStarted = true;
    m_ViewProps.rectangleSelectionStartPoint = ToCanvasSpace(ImVec2(clickEvent.x, clickEvent.y));
    m_ViewProps.rectangleSelectionEndPoint = m_ViewProps.rectangleSelectionStartPoint;
  }
}

void NodeManager::OnMouseRelease(const Event& event) {
  const MouseReleaseEvent& clickEvent = static_cast<const MouseReleaseEvent&>(event);

  auto now = std::chrono::system_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastCLickReleaseTime).count() < 300) {
    if (m_ViewProps.canvasHovered) {
      double x, y;
      glfwGetCursorPos(GetGLFWWindow(), &x, &y);
      MouseDoubleClickEvent doubleClickEvent(clickEvent.button, (float)x, (float)y);
      EventManager::GetInstance().Dispatch(doubleClickEvent);
    }
  }
  m_LastCLickReleaseTime = now;

  if (m_ViewProps.node_clicked != nullptr) {
    std::vector<AbstractNode*> selected_nodes = GetSelectedNodes();

    if (selected_nodes.size() == 1) {
      if (m_ViewProps.node_clicked_position != m_ViewProps.node_clicked->position) {
        auto move_action = std::make_shared<MoveNodeAction>(
            m_ViewProps.node_clicked.get(), m_ViewProps.node_clicked_position, m_ViewProps.node_clicked->position);
        ActionManager::GetInstance().executeCommand(std::move(move_action));
      }
    } else if (selected_nodes.size() > 1) {
      auto offset = m_ViewProps.node_clicked->position - m_ViewProps.node_clicked_position;

      std::vector<ImVec2> from_positions;
      std::vector<ImVec2> to_positions;
      for (auto node : selected_nodes) {
        from_positions.push_back(node->position - offset);
        to_positions.push_back(node->position);
      }
      auto move_action = std::make_shared<MoveMultipleNodesAction>(selected_nodes, from_positions, to_positions);
      move_action->message = std::format("moving {} nodes", selected_nodes.size());
      ActionManager::GetInstance().executeCommand(std::move(move_action));
    }
  }

  for (auto node : GetNodes()) {
    if (node->selected) {
      SetCurrentNode(node);
      break;
    }
  }

  UpdateSelection();

  m_ViewProps.rectangleSelectionStarted = false;
  m_ViewProps.node_clicked = nullptr;
}

void NodeManager::OnKeyPress(const Event& event) {
  // if (!m_ViewProps.canvasHovered) return;
  const KeyPressEvent& keyEvent = static_cast<const KeyPressEvent&>(event);

  switch (keyEvent.key) {
    case GLFW_KEY_BACKSPACE:
      if (m_CurrentNode != nullptr && m_ViewProps.canvasHovered) {
        m_CurrentNetwork->RemoveNode(m_CurrentNode.get());
        NodeDeletedEvent event(m_CurrentNetwork, m_CurrentNode);
        EventManager::GetInstance().Dispatch(event);
        SetCurrentNode(nullptr);
      }
      break;
    case GLFW_KEY_TAB:
      if (m_ViewProps.canvasHovered) m_ViewProps.nodes_menu_opened = true;
      break;
    case GLFW_KEY_ENTER:
      if (m_ViewProps.canvasHovered) {
        if (m_CurrentNode) {
          SetOutputNode(m_CurrentNode);
          EventManager::GetInstance().Dispatch(ManagerUpdateEvent());
        }
      }
      break;
    case GLFW_KEY_F:
      if (m_ViewProps.canvasHovered) ViewFrameAll();
      break;
    case GLFW_KEY_S:
      if (keyEvent.mods & GLFW_MOD_CONTROL) {
        SaveAll();
      }
      break;
    case GLFW_KEY_W: /* FIXME : Z on AZERTY keyboard !!!*/
      if (keyEvent.mods & GLFW_MOD_CONTROL) {
        ActionManager::GetInstance().undo();
      }
      break;
    case GLFW_KEY_Y:
      if (keyEvent.mods & GLFW_MOD_CONTROL) {
        ActionManager::GetInstance().redo();
      }
      break;

    default:
      break;
  }
}

};  // namespace NED