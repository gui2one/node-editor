#include "NodeManager.h"
#include "NodeParam.h"
// #include "Application.h"
namespace NodeEditor {
NodeManager::NodeManager() {
  SetNodesMenu([this]() {
    static NodeFactoryRegistry& registry = NodeFactoryRegistry::instance();
    for(auto& factory : registry.getFactories()) {
      if (ImGui::MenuItem(factory.first.c_str(), NULL, false, true)) {
      auto node = registry.create(factory.first.c_str());
      double x,y;
      glfwGetCursorPos(this->GetGLFWWindow(), &x, &y);

      node->position = ImVec2((float)x, (float)y) - m_ViewProps.scrolling - m_ViewProps.canvasPos;
      this->AddNode(node);
    }
    }

  });
}

NodeManager::~NodeManager() {}

std::shared_ptr<ImGuiNode> NodeManager::FindNodeByUUID(std::string uuid)
{
  for(auto node : GetNodes()) {
    if(node->uuid == uuid) {
      return node;
    }
  }
  return nullptr;
}

void NodeManager::SetNodesMenu(std::function<void()> func) {
  m_NodesMenu = func;
}

void NodeManager::DrawNodes() {
  m_ViewProps.canvasPos = ImGui::GetCursorScreenPos();
  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  // display something to recognize m_OutputNode
  for(auto node : GetNodes()) {
    if(node == m_OutputNode) {
      draw_list->AddCircleFilled(ToScreenSpace(node->position + (node->size / 2.0f) * m_ViewProps.zoom), 35.0f * m_ViewProps.zoom,
                                 NODE_COLOR::BROWN);
      break;
    }
  }
  // draw connections first
  for (auto node : GetNodes()) {
    auto ptr = static_cast<ImGuiNode *>(node.get());
    for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {

      if (ptr->GetInput(i) != nullptr) {
        auto other = ptr->GetInput(i);
        auto input_conn = ptr->GetInputConnector(i);
        ImVec2 p0 = ToScreenSpace(node->position + input_conn->relative_pos);
        ImVec2 other_pos = ToScreenSpace(other->position + ImVec2(other->size.x / 2.0f, other->size.y));

        float y_sep = other_pos.y - p0.y;
        ImVec2 ctrl1 = p0 + ImVec2(0, y_sep);
        ImVec2 ctrl2 = other_pos - ImVec2(0, y_sep);

        draw_list->AddBezierCubic(
            p0, ctrl1, ctrl2, other_pos, NODE_COLOR::GREY,
            2.0f); // ImDrawList API uses screen coordinates()
      }
    }
  }

  if (m_ConnectionProcedure.started) {
    ImVec2 input_conn_pos = m_ConnectionProcedure.output_node
            ->GetInputConnector(m_ConnectionProcedure.output_index)
            ->relative_pos;
    ImVec2 connector_pos = input_conn_pos + m_ConnectionProcedure.output_node->position;
    ImVec2 p0 = ToScreenSpace(connector_pos);
    double x, y;
    glfwGetCursorPos(m_GLFWWindow, &x, &y);
    // std::cout << x << " " << y  << std::endl;
    ImVec2 cursor_pos = ImVec2((float)x, (float)y);
    ImVec2 p1 = cursor_pos;
    draw_list->AddBezierCubic(p0, p0 - ImVec2(0, 100), p1 + ImVec2(0, 100), p1,
                              NODE_COLOR::ORANGE, 2.0f);
  }

  for (auto node : GetNodes()) {
    auto ptr = static_cast<ImGuiNode *>(node.get());

    // input 'connectors'
    for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
      auto input_conn = ptr->GetInputConnector(i);
      ImVec2 cp = ToScreenSpace(node->position + input_conn->relative_pos);
      draw_list->AddCircleFilled(cp, 5.0f * m_ViewProps.zoom, NODE_COLOR::WHITE);
      if (input_conn->hovered) {
        draw_list->AddCircle(cp, 5.0f * m_ViewProps.zoom, NODE_COLOR::ORANGE, 0, 3.0f);
      }
    }

    // output 'connector'
    ImVec2 cp = node->position + ImVec2(node->size.x / 2.0f, node->size.y);
    cp = ToScreenSpace(cp);
    draw_list->AddCircleFilled(cp, 5.0f * m_ViewProps.zoom, NODE_COLOR::WHITE);

    ImVec2 min = ToScreenSpace(node->position);
    ImVec2 max = min + (node->size  * m_ViewProps.zoom);
    draw_list->AddRectFilled(min, max, node->color, 3.0f);

    if(node->selected) ImGui::PushFont(m_BoldFont);
    draw_list->AddText(min + ImVec2(10.0f, 10.0f), IM_COL32(255, 255, 255, 255),
                       node->title.c_str());
    if(node->selected) ImGui::PopFont();

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

  ImVec2 canvas_p0 =
      ImGui::GetCursorScreenPos(); // ImDrawList API uses screen coordinates!!!
  ImVec2 canvas_sz =
      ImGui::GetContentRegionAvail(); // Resize canvas to what's available
  if (canvas_sz.x < 50.0f)
    canvas_sz.x = 50.0f;
  if (canvas_sz.y < 50.0f)
    canvas_sz.y = 50.0f;
  ImVec2 canvas_p1 =
      ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

  // Draw border and background color
  ImGuiIO &io = ImGui::GetIO();
  if(io.MouseWheel > 0.0f){
    m_ViewProps.zoom *= 1.1f;
  } else if(io.MouseWheel < 0.0f){
    m_ViewProps.zoom /= 1.1f;
  }

  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
  draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    if(m_ViewProps.nodes_menu_opened){

      ImGui::OpenPopup("context", 1);
      m_ViewProps.nodes_menu_opened = false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    if (ImGui::BeginPopup("context"))
    {
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
  draw_list->AddLine(ToScreenSpace(ImVec2(0, maker_size/2.0f)), ToScreenSpace(ImVec2(0,-maker_size/2.0f)), NODE_COLOR::YELLOW, 1.0f);
  draw_list->AddLine(ToScreenSpace(ImVec2(maker_size/2.0f, 0)), ToScreenSpace(ImVec2(-maker_size/2.0f, 0)), NODE_COLOR::YELLOW, 1.0f);
  if (m_ViewProps.display_grid) {
    const float GRID_STEP = 50.0f;
    for (float x = fmodf(m_ViewProps.scrolling.x, GRID_STEP); x < canvas_sz.x;
         x += GRID_STEP)
      draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y),
                         ImVec2(canvas_p0.x + x, canvas_p1.y),
                         IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(m_ViewProps.scrolling.y, GRID_STEP); y < canvas_sz.y;
         y += GRID_STEP)
      draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y),
                         ImVec2(canvas_p1.x, canvas_p0.y + y),
                         IM_COL32(200, 200, 200, 40));
  }

  DrawNodes();

  // debug draw 
  ImVec2 raw_pos = io.MousePos;
  if(m_ViewProps.show_mouse_coords){
    
    auto converted_pos = ToCanvasSpace(raw_pos);  
    std::string txt = "(" + std::to_string((int)converted_pos.x) + ", " + std::to_string((int)converted_pos.y) + ")";
    draw_list->AddText(raw_pos + ImVec2(20, 0), IM_COL32(255, 255, 255, 255), (const char*)txt.c_str());
  }
  
  // All drawing finishes here
  draw_list->PopClipRect();
}

void NodeManager::DisplayNodeParams(std::shared_ptr<ImGuiNode> node) {
  if (node == nullptr)
    return;

  ImGui::Begin("Params");
  for (auto item : node->m_ParamLayout.items) {
    item.param->Display();
  }

  ImGui::End();
}

ImVec2 get_nodes_center(std::vector<std::shared_ptr<ImGuiNode>> nodes) {
  if (nodes.size() == 0)
    return ImVec2(0, 0);
  float minx = 999999999.f, miny = 999999999.f, maxx = -999999999.f,
        maxy = -999999999.f;

  for (auto node : nodes) {
    if (node->position.x < minx)
      minx = node->position.x;
    if (node->position.y < miny)
      miny = node->position.y;
    if (node->position.x > maxx)
      maxx = node->position.x;
    if (node->position.y > maxy)
      maxy = node->position.y;
  }
  float centerx, centery;
  centerx = (minx + maxx) / 2.0f + nodes[0]->size.x / 2.0f;
  centery = (miny + maxy) / 2.0f + nodes[0]->size.y / 2.0f;
  return ImVec2(centerx, centery);
}

void NodeManager::Evaluate() {
  if (m_OutputNode != nullptr) {
    m_OutputNode->Update();
  }
}

void NodeManager::SetOutputNode(std::shared_ptr<ImGuiNode> node) {
  m_OutputNode = node;
}

ImVec2 NodeManager::ToCanvasSpace(ImVec2 pos)
{
    return (pos - m_ViewProps.scrolling - m_ViewProps.canvasPos) * (1.0f/m_ViewProps.zoom);
}

ImVec2 NodeManager::ToScreenSpace(ImVec2 pos)
{
    return (pos + m_ViewProps.scrolling + m_ViewProps.canvasPos) * (m_ViewProps.zoom);
}

bool NodeManager::IsNodeHovered(std::shared_ptr<ImGuiNode> node)
{
    ImVec2 min = ToScreenSpace(node->position);
    ImVec2 max = min + node->size;
    double cursor_x, cursor_y;
    glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
    bool hovered = false;
    if (cursor_x > min.x && cursor_x < max.x && cursor_y > min.y &&
        cursor_y < max.y)
    {
        hovered = true;
    }

    return hovered;
}

bool NodeManager::IsInputConnectorHovered(std::shared_ptr<ImGuiNode> node, uint32_t index) {
  double cursor_x, cursor_y;
  glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
  bool hovered = false;

  auto ptr = static_cast<ImGuiNode *>(node.get());
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

void NodeManager::ApplyConnectionProcedure() {
  if (m_ConnectionProcedure.output_node == m_ConnectionProcedure.input_node)
  {
    return;
  }
  
  m_ConnectionProcedure.output_node->SetInput(
      m_ConnectionProcedure.output_index, m_ConnectionProcedure.input_node);
  ResetConnectionProcedure();
  NodeConnectionEvent event(m_ConnectionProcedure.input_node, 0,
                            m_ConnectionProcedure.output_node,
                            m_ConnectionProcedure.output_index);
  EventManager::GetInstance().Dispatch(event);
}

void NodeManager::ResetConnectionProcedure() {
  m_ConnectionProcedure.started = false;
  m_ConnectionProcedure.input_node = nullptr;
  m_ConnectionProcedure.output_node = nullptr;
  m_ConnectionProcedure.output_index = 0;
}

void NodeManager::OnMouseMove(const Event &event) {
  if (!m_ViewProps.canvasHovered)
    return;

  const MouseMoveEvent &moveEvent = static_cast<const MouseMoveEvent &>(event);
  static ImVec2 old_pos = ImVec2(0, 0);
  ImVec2 delta = ImVec2(moveEvent.x, moveEvent.y) - old_pos;
  std::shared_ptr<ImGuiNode> hovered_node = nullptr;

  
  if(ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
      m_ViewProps.scrolling += delta;
  }
  for (auto node : nodes) {
    node->highlighted = false;
    if (node->selected && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
      node->position += delta;
    }
    if (IsNodeHovered(node)) {
      hovered_node = node;
    }

    for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
      auto ptr = static_cast<ImGuiNode *>(node.get());
      InputConnector *connector = ptr->GetInputConnector(i);
      connector->hovered = IsInputConnectorHovered(node, i);
    }
  }
  old_pos = ImVec2(moveEvent.x, moveEvent.y);

  if (hovered_node != nullptr) {
    hovered_node->highlighted = true;
  }
}

void NodeManager::OnMouseClick(const Event &event) {
  if (!m_ViewProps.canvasHovered)
    return;

  const MouseClickEvent &clickEvent = static_cast<const MouseClickEvent &>(event);
  bool clicked_something = false;
  for (auto node : nodes) {
    bool node_hovered = IsNodeHovered(node);
    if( node_hovered){
      m_CurrentNode = node;
      clicked_something = true;
    }else{
      
    }

    if (node_hovered && node->selected == false) {
      node->selected = true;
    } else {
      // if (ImGui::GetIO().KeyCtrl == false){
      //   node->selected = false;
      // }
    }

    if (m_ConnectionProcedure.started && IsNodeHovered(node)) {
      clicked_something = true;
      m_ConnectionProcedure.input_node = node;

      ApplyConnectionProcedure();
      Evaluate();
    }
    for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
      auto ptr = static_cast<ImGuiNode *>(node.get());
      InputConnector *connector = ptr->GetInputConnector(i);
      if (connector->hovered) {
        clicked_something = true;
        connector->grabbed = true;

        m_ConnectionProcedure.started = true;
        m_ConnectionProcedure.output_node = node;
        m_ConnectionProcedure.output_index = i;
      }
    }
  }

  if (!clicked_something) {
    for(auto node : nodes) {
      node->selected = false;
    }
    if (m_ConnectionProcedure.started) {
      m_ConnectionProcedure.started = false;
      m_ConnectionProcedure.output_node->ResetInput(
          m_ConnectionProcedure.output_index);
      ResetConnectionProcedure();
    }
  }
}

void NodeManager::OnMouseDoubleClick(const Event &event)
{
  std::cout << "double click !!! from manager" << std::endl;
  
}

void NodeManager::OnMouseRelease(const Event &event) {
  
  const MouseReleaseEvent &clickEvent =
      static_cast<const MouseReleaseEvent &>(event);
  
  auto now = std::chrono::system_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastCLickReleaseTime).count() < 300) {
    if( m_ViewProps.canvasHovered){
      double x,y;
      glfwGetCursorPos(GetGLFWWindow(), &x, &y);
      MouseDoubleClickEvent doubleClickEvent(clickEvent.button, (float)x, (float)y);
      EventManager::GetInstance().Dispatch(doubleClickEvent);
    }
  }
  m_LastCLickReleaseTime = now;

  for (auto node : nodes) {
    if (m_ConnectionProcedure.started && IsNodeHovered(node)) {
      m_ConnectionProcedure.input_node = node;

      ApplyConnectionProcedure();
      Evaluate();

      
    }
  }

  if( m_OneParamChanged ) {
    Evaluate();
    m_OneParamChanged = false;
    ManagerUpdateEvent event;
    EventManager::GetInstance().Dispatch(event);
      
  }
}

void NodeManager::OnKeyPress(const Event &event) {
  if (!m_ViewProps.canvasHovered)
    return;
  const KeyPressEvent &clickEvent = static_cast<const KeyPressEvent &>(event);
  
  switch(clickEvent.key) {
    case GLFW_KEY_BACKSPACE:
      if (m_CurrentNode != nullptr) {
        auto it = std::find(nodes.begin(), nodes.end(), m_CurrentNode);
        nodes.erase(it);
        m_CurrentNode = nullptr;
      }
      break;
    case GLFW_KEY_TAB:
      m_ViewProps.nodes_menu_opened = true;
      break;
    case GLFW_KEY_ENTER:
      if(m_CurrentNode){
        SetOutputNode(m_CurrentNode);
        EventManager::GetInstance().Dispatch(ManagerUpdateEvent());
      }
      break;
    default:
      break;
  }
  if (clickEvent.key == GLFW_KEY_F) {
    ViewFrameAll();
  }
}

void NodeManager::ViewFrameAll() {
  ImVec2 center = get_nodes_center(nodes);
  m_ViewProps.scrolling = -center + m_ViewProps.canvasSize / 2.0f;
}

}; // namespace NodeEditor