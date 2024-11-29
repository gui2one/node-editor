// #include "NodeParam.h") {
#include "NodeManager.h"

GLuint GenerateEmptyTexture() {
  std::cout << "EMPTY TEXTURE !!!!" << std::endl;

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

GLuint LoadTexture(const char *filename) {
  int width, height, channels;
  // std::cout << "!!!!!!!!!!!!!!!! " << filename << std::endl;

  unsigned char *data = stbi_load(filename, &width, &height, &channels, 4);
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
  REGISTER_PARAM_TYPE(NED::Param<std::string>);
  REGISTER_PARAM_TYPE(NED::Param<std::wstring>);
  REGISTER_PARAM_TYPE(NED::Param<int>);
  REGISTER_PARAM_TYPE(NED::Param<float>);
  REGISTER_PARAM_TYPE(NED::Param<bool>);
  REGISTER_PARAM_TYPE(NED::Param<glm::vec2>);
  // REGISTER_PARAM_TYPE(NED::Param<glm::vec3>);
  REGISTER_PARAM_TYPE(NED::ParamVec3);
}

NodeManager::~NodeManager() {}

void NodeManager::InitGLFWEvents() {
  static auto &dispatcher = EventManager::GetInstance();

  glfwSetWindowUserPointer(GetGLFWWindow(), &m_WindowData);

  glfwSetMouseButtonCallback(GetGLFWWindow(), [](GLFWwindow *window, int button, int action, int mods) {
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

  glfwSetCursorPosCallback(GetGLFWWindow(), [](GLFWwindow *window, double xpos, double ypos) {
    MouseMoveEvent moveEvent((float)xpos, (float)ypos);
    dispatcher.Dispatch(moveEvent);
  });

  glfwSetKeyCallback(GetGLFWWindow(), [](GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      KeyPressEvent pressEvent(key, mods);
      dispatcher.Dispatch(pressEvent);
    }
  });

  glfwSetFramebufferSizeCallback(GetGLFWWindow(), [](GLFWwindow *window, int width, int height) {
    WindowData *data = (WindowData *)glfwGetWindowUserPointer(window);
    data->width = width;
    data->height = height;
    glViewport(0, 0, width, height);
  });

  glfwSetDropCallback(GetGLFWWindow(), [](GLFWwindow *window, int count, const char **paths) {
    DropFileEvent event(paths[0]);
    dispatcher.Dispatch(event);
  });
  dispatcher.Subscribe(EventType::MouseClick, [this](const NED::Event &event) { this->OnMouseClick(event); });
  dispatcher.Subscribe(EventType::MouseDoubleClick,
                       [this](const NED::Event &event) { this->OnMouseDoubleClick(event); });
  dispatcher.Subscribe(EventType::MouseRelease, [this](const NED::Event &event) { this->OnMouseRelease(event); });
  dispatcher.Subscribe(EventType::MouseMove, [this](const NED::Event &event) { this->OnMouseMove(event); });
  dispatcher.Subscribe(EventType::KeyPress, [this](const NED::Event &event) { this->OnKeyPress(event); });
  dispatcher.Subscribe(EventType::DropFile, [this](const NED::Event &event) {
    auto drop_ev = static_cast<const NED::DropFileEvent &>(event);
    auto path = std::filesystem::path(drop_ev.path);
    auto net = load_yaml_file(path);
    this->GetRootNetwork() = net;

    this->m_SavePath = path;
    glfwSetWindowTitle(this->GetGLFWWindow(), path.string().c_str());
  });
  dispatcher.Subscribe(EventType::ParamChanged, [this](const Event &event) {
    // auto &manager = *this;
    auto ev_string = dynamic_cast<const ParamChangedEvent<std::string> *>(&event);
    auto ev_wstring = dynamic_cast<const ParamChangedEvent<std::wstring> *>(&event);
    auto ev_bool = dynamic_cast<const ParamChangedEvent<bool> *>(&event);
    auto ev_float = dynamic_cast<const ParamChangedEvent<float> *>(&event);
    auto ev_int = dynamic_cast<const ParamChangedEvent<int> *>(&event);
    auto ev_vec2 = dynamic_cast<const ParamChangedEvent<glm::vec2> *>(&event);
    auto ev_vec3 = dynamic_cast<const ParamChangedEvent<glm::vec3> *>(&event);
    if (ev_string != nullptr) {
      std::cout << "Param Changed : " << ev_string->param_name << "\nNew Value : " << ev_string->new_value
                << "\nOld Value : " << ev_string->old_value << std::endl;
      std::cout << "For Node : " << ev_string->node->title << std::endl;
    } else if (ev_wstring != nullptr) {
      std::string utf8_str = wide_to_utf8(ev_wstring->new_value);
      std::cout << "Param Changed : " << ev_wstring->param_name << "\nNew Value : " << utf8_str << std::endl;
    } else if (ev_bool != nullptr) {
      std::cout << "Param Changed : " << ev_bool->param_name
                << "\nNew Value : " << (ev_bool->new_value ? "true" : "false") << std::endl;
    } else if (ev_float != nullptr) {
      std::cout << "Param Changed : " << ev_float->param_name << "\nNew Value : " << ev_float->new_value << std::endl;
    } else if (ev_int != nullptr) {
      std::cout << "Param Changed : " << ev_int->param_name << "\nNew Value : " << ev_int->new_value << std::endl;
    } else if (ev_vec2 != nullptr) {
      std::cout << "Param Changed : " << ev_vec2->param_name << "\nNew Value : " << ev_vec2->new_value.x << ", "
                << ev_vec2->new_value.y << std::endl;
    } else if (ev_vec3 != nullptr) {
      std::cout << "Param Changed : " << ev_vec3->param_name << "\nNew Value : " << ev_vec3->new_value.x << ", "
                << ev_vec3->new_value.y << ", " << ev_vec3->new_value.z << std::endl;
    }
    // auto ev = dynamic_cast<const ParamChangedEvent *>(&event);
    m_OneParamChanged = true;
  });
}

void NodeManager::InitIcons() {
  // empty icon
  GLuint empty_tex_id = GenerateEmptyTexture();
  m_Icons.insert({"", empty_tex_id});
  for (auto &res : m_NodeIconsResources) {
    GLuint id = LoadTexture(res.path.string().c_str());
    m_Icons.insert({res.name, id});
  }
}

void NodeManager::AddIcon(std::string name, std::filesystem::path path) {
  m_Icons.insert({name, LoadTexture(path.string().c_str())});
}

std::shared_ptr<AbstractNode> NodeManager::FindNodeByUUID(std::string uuid) {
  for (auto node : GetNodes()) {
    if (node->uuid == uuid) {
      return node;
    }
  }
  return nullptr;
}

std::shared_ptr<NED::NodeParam> NodeManager::FindParamByName(std::shared_ptr<NED::AbstractNode> factory_node,
                                                             std::string param_name) {
  for (auto &param_item : factory_node->m_ParamLayout.params) {
    auto p_group = std::dynamic_pointer_cast<ParamGroup>(param_item);
    if (p_group != nullptr) {
      if (p_group->m_Label == param_name) {
        return p_group;
      }
      for (auto group_item : p_group->params) {
        if (group_item->m_Label == param_name) {
          return group_item;
        }
      }

    } else {
      if (param_item->m_Label == param_name) {
        return param_item;
      }
    }
  }

  return nullptr;
}

void NodeManager::CreateAllNodes() {
  static NodeFactoryRegistry &registry = NodeFactoryRegistry::GetInstance();

  std::unordered_map<std::string, std::vector<NodeFactoryRegistryItem>> whole_thing;
  // collect items by category
  for (auto &factory : registry.GetFactories()) {
    whole_thing[factory.second.category_name].push_back(factory.second);
  }

  float x = 10.0f;
  float y = 10.0f;
  for (auto &[category_name, params] : whole_thing) {
    for (auto &item : params) {
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
      GetRootNetwork().nodes.clear();
      GetRootNetwork().outuput_node = nullptr;
      m_CurrentNode = nullptr;
      m_SavePath = std::filesystem::path("");
      glfwSetWindowTitle(GetGLFWWindow(), m_SavePath.string().c_str());
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
      // AddNode(factory_node);
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

    if (ImGui::MenuItem("Goto Root")) {
      GotoRootNetwork();
    }

    ImGui::EndMenu();
  }
  // if (ImGui::BeginMenu("ImGui")) {
  //   ImGui::MenuItem("Show Demo Window", NULL, &showDemoWindow);
  //   ImGui::EndMenu();
  // }
}

void NodeManager::SetNodesMenu(std::function<void()> func) { m_NodesMenu = func; }

void NodeManager::BuildNodeMenuFromRegistry() {
  static NodeFactoryRegistry &registry = NodeFactoryRegistry::GetInstance();

  std::unordered_map<std::string, std::vector<NodeFactoryRegistryItem>> whole_thing;

  // Collect items by category
  for (auto &factory : registry.GetFactories()) {
    whole_thing[factory.second.category_name].push_back(factory.second);
  }

  // Helper lambda to split category strings
  auto split_category = [](const std::string &category) -> std::vector<std::string> {
    std::vector<std::string> parts;
    std::stringstream ss(category);
    std::string part;
    while (std::getline(ss, part, '/')) {
      parts.push_back(part);
    }
    return parts;
  };

  // Recursive function to create hierarchical menus
  std::function<void(const std::vector<std::string> &, const std::vector<NodeFactoryRegistryItem> &)> create_menu;
  create_menu = [&](const std::vector<std::string> &path, const std::vector<NodeFactoryRegistryItem> &items) {
    if (path.empty()) return;

    if (ImGui::BeginMenu(path.front().c_str())) {
      if (path.size() == 1) {
        // Add items to this menu
        for (const auto &item : items) {
          if (ImGui::MenuItem(item.label.c_str(), nullptr, false, true)) {
            auto node = registry.Create(item.type_name.c_str(), m_CurrentNetworkOwner);
            if (node != nullptr) {
              double x, y;
              glfwGetCursorPos(this->GetGLFWWindow(), &x, &y);
              node->position = ImVec2((float)x, (float)y) - m_ViewProps.scrolling - m_ViewProps.canvasPos;
              node->parent_node = m_CurrentNetworkOwner;
              this->m_CurrentNetwork->AddNode(node);
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
  for (auto &[category_name, items] : whole_thing) {
    std::vector<std::string> path = split_category(category_name);
    create_menu(path, items);
  }
}

void NodeManager::DrawNodes() {
  if (m_CurrentNetwork == nullptr) {
    return;
  }
  m_ViewProps.canvasPos = ImGui::GetCursorScreenPos();
  ImDrawList *draw_list = ImGui::GetWindowDrawList();

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
    auto ptr = static_cast<AbstractNode *>(node.get());
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
    if (m_ConnectionProcedure.output_node->IsMultiInput()) {
      ImVec2 input_conn_pos = m_ConnectionProcedure.output_node->position;
      // ImVec2 connector_pos = input_conn_pos + m_ConnectionProcedure.output_node->position;
      ImVec2 p0 = ToScreenSpace(input_conn_pos);
      double x, y;
      glfwGetCursorPos(m_GLFWWindow, &x, &y);
      ImVec2 cursor_pos = ImVec2((float)x, (float)y);
      ImVec2 p1 = cursor_pos;
      draw_list->AddBezierCubic(p0, p0 - ImVec2(0, 100), p1 + ImVec2(0, 100), p1, NODE_COLOR::ORANGE, 2.0f);
    } else {
      ImVec2 input_conn_pos =
          m_ConnectionProcedure.output_node->GetInputConnector(m_ConnectionProcedure.output_index)->relative_pos;
      ImVec2 connector_pos = input_conn_pos + m_ConnectionProcedure.output_node->position;
      ImVec2 p0 = ToScreenSpace(connector_pos);
      double x, y;
      glfwGetCursorPos(m_GLFWWindow, &x, &y);
      ImVec2 cursor_pos = ImVec2((float)x, (float)y);
      ImVec2 p1 = cursor_pos;
      draw_list->AddBezierCubic(p0, p0 - ImVec2(0, 100), p1 + ImVec2(0, 100), p1, NODE_COLOR::ORANGE, 2.0f);
    }
  }

  for (auto node : GetNodes()) {
    auto ptr = static_cast<AbstractNode *>(node.get());

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
      if (m_Icons.at((const char *)node->icon_name) != 0) {
        draw_list->AddImage((ImTextureID)(intptr_t)m_Icons.at((const char *)node->icon_name), icon_min, icon_max, uv0,
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
  ImGuiIO &io = ImGui::GetIO();
  if (io.MouseWheel > 0.0f) {
    m_ViewProps.zoom *= 1.1f;
  } else if (io.MouseWheel < 0.0f) {
    m_ViewProps.zoom /= 1.1f;
  }

  ImDrawList *draw_list = ImGui::GetWindowDrawList();
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
  ImVec2 raw_pos = io.MousePos;
  if (m_ViewProps.show_mouse_coords) {
    auto converted_pos = ToCanvasSpace(raw_pos);
    std::string txt = "(" + std::to_string((int)converted_pos.x) + ", " + std::to_string((int)converted_pos.y) + ")";
    draw_list->AddText(raw_pos + ImVec2(20, 0), IM_COL32(255, 255, 255, 255), (const char *)txt.c_str());
  }

  // All drawing finishes here
  draw_list->PopClipRect();
}

void NodeManager::DisplayNodeParams(std::shared_ptr<AbstractNode> node) {
  if (node == nullptr) return;

  ImGui::Begin("Params");
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
    // std::cout << "Node name changed to " << node->title << std::endl;
  }
  ImGui::Separator();
  ImGui::Spacing();
  ImGui::Spacing();
  for (auto param : node->m_ParamLayout.params) {
    param->Display();
  }

  ImGui::End();
}

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

ImVec2 NodeManager::ToCanvasSpace(ImVec2 pos) {
  return (pos - m_ViewProps.scrolling - m_ViewProps.canvasPos) * (1.0f / m_ViewProps.zoom);
}

ImVec2 NodeManager::ToScreenSpace(ImVec2 pos) {
  return (pos + m_ViewProps.scrolling + m_ViewProps.canvasPos) * (m_ViewProps.zoom);
}

bool NodeManager::IsNodeHovered(std::shared_ptr<AbstractNode> node) {
  ImVec2 min = ToScreenSpace(node->position);
  ImVec2 max = min + node->size;
  double cursor_x, cursor_y;
  glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
  bool hovered = false;
  if (cursor_x > min.x && cursor_x < max.x && cursor_y > min.y && cursor_y < max.y) {
    hovered = true;
  }

  return hovered;
}

bool NodeManager::IsInputConnectorHovered(std::shared_ptr<AbstractNode> node, uint32_t index) {
  double cursor_x, cursor_y;
  glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
  bool hovered = false;

  auto ptr = static_cast<AbstractNode *>(node.get());
  InputConnector *connector = ptr->GetInputConnector(index);
  ImVec2 connector_pos = ToScreenSpace(node->position + connector->relative_pos);

  float padding = 1.5f;
  if (cursor_x > connector_pos.x - connector->width * padding &&
      cursor_x < connector_pos.x + connector->width * padding &&
      cursor_y > connector_pos.y - connector->width * padding &&
      cursor_y < connector_pos.y + connector->width * padding) {
    hovered = true;
  }

  return hovered;
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
  if (m_ConnectionProcedure.output_node == m_ConnectionProcedure.input_node) {
    return;
  }
  if (m_ConnectionProcedure.is_mutli_input) {
    if (m_ConnectionProcedure.input_node == nullptr) {
      m_ConnectionProcedure.output_node->RemoveLastInput();
      ResetConnectionProcedure();
      NodeConnectionEvent event(nullptr, 0, m_ConnectionProcedure.output_node, 0);
      EventManager::GetInstance().Dispatch(event);

    } else {
      m_ConnectionProcedure.output_node->AppendInput(m_ConnectionProcedure.input_node);
      ResetConnectionProcedure();
      NodeConnectionEvent event(m_ConnectionProcedure.input_node, 0, m_ConnectionProcedure.output_node,
                                m_ConnectionProcedure.output_index);
      EventManager::GetInstance().Dispatch(event);
    }
  } else {
    m_ConnectionProcedure.output_node->SetInput(m_ConnectionProcedure.output_index, m_ConnectionProcedure.input_node);
    ResetConnectionProcedure();
    NodeConnectionEvent event(m_ConnectionProcedure.input_node, 0, m_ConnectionProcedure.output_node,
                              m_ConnectionProcedure.output_index);
    EventManager::GetInstance().Dispatch(event);
  }
}

void NodeManager::ResetConnectionProcedure() {
  m_ConnectionProcedure.started = false;
  m_ConnectionProcedure.is_mutli_input = false;
  m_ConnectionProcedure.input_node = nullptr;
  m_ConnectionProcedure.output_node = nullptr;
  m_ConnectionProcedure.output_index = 0;
}

void NodeManager::SaveAll() {
  if (m_SavePath.empty()) {
    auto path = Utils::open_file_explorer({{"Node-Editor Network Files", m_FileExtension}});
    m_SavePath = path;
    glfwSetWindowTitle(m_GLFWWindow, path.string().c_str());
  }
  save_all(m_SavePath, GetRootNetwork());
}

void NodeManager::LoadAll() {
  if (m_SavePath.empty()) {
    auto path = Utils::open_file_explorer({{"Node-Editor Network Files", m_FileExtension}});
    // auto path = Utils::open_file_explorer();
    m_SavePath = path;
    glfwSetWindowTitle(m_GLFWWindow, path.string().c_str());
  }

  if (!m_SavePath.empty()) {
    NodeNetwork net = load_yaml_file(m_SavePath);
    m_NodeNetwork = net;
    ViewFrameAll();
  }
}

void NodeManager::LoadFromFile(std::filesystem::path path) {
  if (!path.empty()) {
    m_SavePath = path;
    NodeNetwork net = load_yaml_file(path);
    m_NodeNetwork = net;
    ViewFrameAll();
    glfwSetWindowTitle(m_GLFWWindow, path.string().c_str());
  }
}

bool valueInRange(float value, float min, float max) { return (value >= min) && (value <= max); }

bool rectOverlap(Rect A, Rect B) {
  bool xOverlap = valueInRange(A.x, B.x, B.x + B.width) || valueInRange(B.x, A.x, A.x + A.width);

  bool yOverlap = valueInRange(A.y, B.y, B.y + B.height) || valueInRange(B.y, A.y, A.y + A.height);

  return xOverlap && yOverlap;
}

void NodeManager::OnMouseMove(const Event &event) {
  if (!m_ViewProps.canvasHovered) return;

  const MouseMoveEvent &moveEvent = static_cast<const MouseMoveEvent &>(event);
  static ImVec2 old_pos = ImVec2(0, 0);
  ImVec2 delta = ImVec2(moveEvent.x, moveEvent.y) - old_pos;
  std::shared_ptr<AbstractNode> hovered_node = nullptr;

  // if (m_ViewProps.node_clicked != nullptr) {
  //   // m_ViewProps.node_clicked->position += delta;
  //   std::cout << "Moving node : " << m_ViewProps.node_clicked->title << "";
  //   std::cout << "-- Start pos = " << m_ViewProps.node_clicked_position.x << ", " <<
  //   m_ViewProps.node_clicked_position.y
  //             << std::endl;
  // }

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
      auto ptr = static_cast<AbstractNode *>(node.get());
      InputConnector *connector = ptr->GetInputConnector(i);
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
      bool inside_x = false;
      bool inside_y = false;

      float node_x = node->position.x;
      float node_y = node->position.y;
      float node_w = node->size.x;
      float node_h = node->size.y;

      Rect node_rect;
      node_rect.x = node->position.x;
      node_rect.y = node->position.y;
      node_rect.width = node->size.x;
      node_rect.height = node->size.y;

      Rect selection_rect;

      if (start_x < end_x) {
        selection_rect.x = start_x;
        selection_rect.width = std::abs(end_x - start_x);
      } else if (start_x > end_x) {
        selection_rect.width = std::abs(start_x - end_x);
        selection_rect.x = end_x;
      } else {
        selection_rect.x = start_x;
        selection_rect.width = 1.0f;
      }

      if (start_y < end_y) {
        selection_rect.y = start_y;
        selection_rect.height = std::abs(end_y - start_y);
      } else if (start_y > end_y) {
        selection_rect.height = std::abs(start_y - end_y);
        selection_rect.y = end_y;
      } else {
        selection_rect.y = start_y;
        selection_rect.height = 1.0f;
      }

      node->selected = rectOverlap(node_rect, selection_rect);
    }
  }
}

void NodeManager::OnMouseClick(const Event &event) {
  if (!m_ViewProps.canvasHovered) return;

  const MouseClickEvent &clickEvent = static_cast<const MouseClickEvent &>(event);
  bool clicked_something = false;
  for (auto node : GetNodes()) {
    bool node_hovered = IsNodeHovered(node);
    if (node_hovered) {
      m_CurrentNode = node;
      clicked_something = true;
      m_ViewProps.node_clicked = node;
      m_ViewProps.node_clicked_position = node->position;
      // std::cout << "Node clicked: " << m_ViewProps.node_clicked->title << std::endl;
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
      m_ConnectionProcedure.input_node = node;

      ApplyConnectionProcedure();
      Evaluate();
    }
    if (!node->IsMultiInput()) {
      for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
        auto ptr = static_cast<AbstractNode *>(node.get());
        InputConnector *connector = ptr->GetInputConnector(i);
        if (connector->hovered) {
          clicked_something = true;
          connector->grabbed = true;

          m_ConnectionProcedure.started = true;
          m_ConnectionProcedure.output_node = node;
          m_ConnectionProcedure.output_index = i;
        }
      }
    } else {
      if (IsNodeMultiInputConnectorHovered(node)) {
        clicked_something = true;
        m_ConnectionProcedure.started = true;
        m_ConnectionProcedure.is_mutli_input = true;

        m_ConnectionProcedure.output_node = node;
        // m_ConnectionProcedure.output_index = 0;
      }
    }
  }

  if (!clicked_something) {
    Utils::deselect_all(GetNodes());
    if (m_ConnectionProcedure.started) {
      if (m_ConnectionProcedure.is_mutli_input) {  // multi input
        ApplyConnectionProcedure();
      } else {
        m_ConnectionProcedure.started = false;
        m_ConnectionProcedure.output_node->ResetInput(m_ConnectionProcedure.output_index);
        ResetConnectionProcedure();
      }
    }

    m_ViewProps.rectangleSelectionStarted = true;
    m_ViewProps.rectangleSelectionStartPoint = ToCanvasSpace(ImVec2(clickEvent.x, clickEvent.y));
    m_ViewProps.rectangleSelectionEndPoint = m_ViewProps.rectangleSelectionStartPoint;
  }
}

void NodeManager::OnMouseRelease(const Event &event) {
  const MouseReleaseEvent &clickEvent = static_cast<const MouseReleaseEvent &>(event);

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
    if (m_ViewProps.node_clicked_position != m_ViewProps.node_clicked->position) {
      auto move_action = std::make_unique<MoveNodeAction>(m_ViewProps.node_clicked, m_ViewProps.node_clicked_position,
                                                          m_ViewProps.node_clicked->position);
      ActionManager::GetInstance().executeCommand(std::move(move_action));
    }
  }
  for (auto node : GetNodes()) {
    if (m_ConnectionProcedure.started && IsNodeHovered(node)) {
      m_ConnectionProcedure.input_node = node;

      ApplyConnectionProcedure();
      Evaluate();
    }
  }

  if (m_OneParamChanged) {
    Evaluate();
    m_OneParamChanged = false;
    ManagerUpdateEvent event;
    EventManager::GetInstance().Dispatch(event);
  }

  for (auto node : GetNodes()) {
    if (node->selected) {
      m_CurrentNode = node;
      break;
    }
  }
  m_ViewProps.rectangleSelectionStarted = false;
  m_ViewProps.node_clicked = nullptr;
}

void NodeManager::OnKeyPress(const Event &event) {
  if (!m_ViewProps.canvasHovered) return;
  const KeyPressEvent &keyEvent = static_cast<const KeyPressEvent &>(event);

  switch (keyEvent.key) {
    case GLFW_KEY_BACKSPACE:
      if (m_CurrentNode != nullptr) {
        auto it = std::find(GetNodes().begin(), GetNodes().end(), m_CurrentNode);
        GetNodes().erase(it);
        m_CurrentNode = nullptr;
      }
      break;
    case GLFW_KEY_TAB:
      m_ViewProps.nodes_menu_opened = true;
      break;
    case GLFW_KEY_ENTER:
      if (m_CurrentNode) {
        SetOutputNode(m_CurrentNode);
        EventManager::GetInstance().Dispatch(ManagerUpdateEvent());
      }
      break;

    case GLFW_KEY_S:
      if (keyEvent.mods & GLFW_MOD_CONTROL) {
        SaveAll();
      }
      break;

    default:
      break;
  }
  if (keyEvent.key == GLFW_KEY_F) {
    ViewFrameAll();
  }
}

void NodeManager::ViewFrameAll() {
  ImVec2 center = Utils::get_nodes_center(GetNodes());
  m_ViewProps.scrolling = -center + m_ViewProps.canvasSize / 2.0f;
}

};  // namespace NED