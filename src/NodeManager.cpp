#include "NodeManager.h"

NodeManager::NodeManager()
{
}

NodeManager::~NodeManager()
{
}

void NodeManager::SetNodesMenu(std::function<void()> func)
{
    m_NodesMenu = func;
}

void NodeManager::DrawNodes()
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 offset = m_Origin;

    // draw connections first
    for (auto node : GetNodes())
    {
        auto ptr = static_cast<ImGuiNode *>(node.get());
        for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++)
        {

            if (ptr->GetInput(i) != nullptr)
            {
                auto other = ptr->GetInput(i);
                auto input_conn = ptr->GetInputConnector(i);
                ImVec2 p0 = node->position + input_conn->relative_pos + m_Origin;
                ImVec2 other_pos = other->position + offset + ImVec2(other->size.x / 2.0f, other->size.y);

                float y_sep = other_pos.y - p0.y;
                ImVec2 ctrl1 = p0 + ImVec2(0, y_sep);
                ImVec2 ctrl2 = other_pos - ImVec2(0, y_sep);

                draw_list->AddBezierCubic(p0, ctrl1, ctrl2, other_pos, NODE_COLOR::GREY, 2.0f); // ImDrawList API uses screen coordinates()
            }
        }
    }

    if (m_ConnectionProcedure.started)
    {
        ImVec2 connector_pos = m_ConnectionProcedure.output_node->GetInputConnector(m_ConnectionProcedure.output_index)->relative_pos + m_ConnectionProcedure.output_node->position + offset;
        ImVec2 p0 = connector_pos;
        double x, y;
        glfwGetCursorPos(m_GLFWWindow, &x, &y);
        // std::cout << x << " " << y  << std::endl;
        ImVec2 cursor_pos = ImVec2((float)x, (float)y);
        ImVec2 p1 = cursor_pos;
        draw_list->AddBezierCubic(p0, p0 - ImVec2(0, 100), p1 + ImVec2(0, 100), p1, NODE_COLOR::ORANGE, 2.0f);
    }

    for (auto node : GetNodes())
    {
        auto ptr = static_cast<ImGuiNode *>(node.get());

        // input 'connectors'
        for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++)
        {
            auto input_conn = ptr->GetInputConnector(i);
            ImVec2 cp = node->position + offset + input_conn->relative_pos;
            draw_list->AddCircleFilled(cp, 5.0f, NODE_COLOR::WHITE);
            if (input_conn->hovered)
            {
                draw_list->AddCircle(cp, 5.0f, NODE_COLOR::ORANGE, 0, 3.0f);
            }
        }

        // output 'connector'
        ImVec2 cp = node->position + offset + ImVec2(node->size.x / 2.0f, node->size.y);
        draw_list->AddCircleFilled(cp, 5.0f, NODE_COLOR::WHITE);

        ImVec2 min = node->position + offset;
        ImVec2 max = min + node->size;
        draw_list->AddRectFilled(min, max, node->color, 3.0f);

        draw_list->AddText(min + ImVec2(10.0f, 10.0f), IM_COL32(255, 255, 255, 255), node->title);

        if (!node->highlighted)
        {
            draw_list->AddRect(min, max, NODE_COLOR::DARK_GREY, 3.0f);
        }
        else
        {
            draw_list->AddRect(min, max, NODE_COLOR::LIGHT_GREY, 3.0f);
        }

        if (node->selected)
        {
            draw_list->AddRect(min, max, IM_COL32(200, 200, 60, 100), 3.0f, 0, 3.0f);
        }
    }
}

void NodeManager::DrawCanvas()
{
    m_CanvasHovered = ImGui::IsWindowHovered();
    m_CanvasSize = ImGui::GetWindowSize();
    static ImVector<ImVec2> points;
    // static ImVec2 scrolling(0.0f, 0.0f);
    static bool opt_enable_grid = true;
    static bool opt_enable_context_menu = true;
    static bool adding_line = false;

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();    // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
    if (canvas_sz.x < 50.0f)
        canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f)
        canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO &io = ImGui::GetIO();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

    const bool is_hovered = ImGui::IsItemHovered();                                  // Hovered
    const bool is_active = ImGui::IsItemActive();                                    // Held
    m_Origin = ImVec2((canvas_p0.x + m_Scrolling.x), (canvas_p0.y + m_Scrolling.y)); // Lock scrolled origin
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - m_Origin.x, io.MousePos.y - m_Origin.y);

    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
    {
        m_Scrolling.x += io.MouseDelta.x;
        m_Scrolling.y += io.MouseDelta.y;
    }

    // Context menu (under default mouse threshold)
    ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
    if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
    if (ImGui::BeginPopup("context"))
    {
        m_NodesMenu();
        ImGui::EndPopup();
    }

    // Draw grid
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);

    if (opt_enable_grid)
    {
        const float GRID_STEP = 64.0f;
        for (float x = fmodf(m_Scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
        for (float y = fmodf(m_Scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    }
    for (int n = 0; n < points.Size; n += 2)
    {
        draw_list->AddLine(ImVec2(m_Origin.x + points[n].x, m_Origin.y + points[n].y), ImVec2(m_Origin.x + points[n + 1].x, m_Origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
    }

    DrawNodes();

    draw_list->PopClipRect();
}

ImVec2 get_nodes_center(std::vector<std::shared_ptr<ImGuiNode>> nodes)
{
    if (nodes.size() == 0)
        return ImVec2(0, 0);
    float minx = 999999999.f, miny = 999999999.f, maxx = -999999999.f, maxy = -999999999.f;

    for (auto node : nodes)
    {
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

void NodeManager::Evaluate()
{
    m_OutputNode->Update();
}

void NodeManager::SetOutputNode(std::shared_ptr<ImGuiNode> node)
{
    m_OutputNode = node;
}

bool NodeManager::IsNodeHovered(std::shared_ptr<ImGuiNode> node)
{
    ImVec2 min = node->position + m_Origin;
    ImVec2 max = min + node->size;
    double cursor_x, cursor_y;
    glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
    bool hovered = false;
    if (cursor_x > min.x && cursor_x < max.x && cursor_y > min.y && cursor_y < max.y)
    {
        hovered = true;
    }

    return hovered;
}

bool NodeManager::IsInputConnectorHovered(std::shared_ptr<ImGuiNode> node, uint32_t index)
{
    double cursor_x, cursor_y;
    glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
    bool hovered = false;

    auto ptr = static_cast<ImGuiNode *>(node.get());
    InputConnector *connector = ptr->GetInputConnector(index);
    ImVec2 connector_pos = node->position + connector->relative_pos + m_Origin;

    float padding = 1.5f;
    if (cursor_x > connector_pos.x - connector->width * padding &&
        cursor_x < connector_pos.x + connector->width * padding &&
        cursor_y > connector_pos.y - connector->width * padding &&
        cursor_y < connector_pos.y + connector->width * padding)
    {
        hovered = true;
    }

    return hovered;
}

void NodeManager::ApplyConnectionProcedure()
{
    m_ConnectionProcedure.output_node->SetInput(m_ConnectionProcedure.output_index, m_ConnectionProcedure.input_node);
    ResetConnectionProcedure();
    NodeConnectionEvent event(m_ConnectionProcedure.input_node, 0, m_ConnectionProcedure.output_node, m_ConnectionProcedure.output_index);
    EventManager::GetInstance().Dispatch(event);
}

void NodeManager::ResetConnectionProcedure()
{
    m_ConnectionProcedure.started = false;
    m_ConnectionProcedure.input_node = nullptr;
    m_ConnectionProcedure.output_node = nullptr;
    m_ConnectionProcedure.output_index = 0;
}

void NodeManager::OnMouseMove(const Event &event)
{
    const MouseMoveEvent &moveEvent = static_cast<const MouseMoveEvent &>(event);
    static ImVec2 old_pos = ImVec2(0, 0);
    ImVec2 delta = ImVec2(moveEvent.x - old_pos.x, moveEvent.y - old_pos.y);
    std::shared_ptr<ImGuiNode> hovered_node = nullptr;
    for (auto node : nodes)
    {
        node->highlighted = false;
        if (node->selected && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            node->position.x += delta.x;
            node->position.y += delta.y;
        }
        if (IsNodeHovered(node))
        {
            hovered_node = node;
        }

        for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++)
        {
            auto ptr = static_cast<ImGuiNode *>(node.get());
            InputConnector *connector = ptr->GetInputConnector(i);
            connector->hovered = IsInputConnectorHovered(node, i);
        }
        // node->GetInputConnector(0)->hovered = IsInputConnectorHovered(node, 0);
    }
    old_pos = ImVec2(moveEvent.x, moveEvent.y);

    if (hovered_node != nullptr)
    {
        hovered_node->highlighted = true;
    }
}

void NodeManager::OnMouseClick(const Event &event)
{
    if (!m_CanvasHovered)
        return;

    const MouseClickEvent &clickEvent = static_cast<const MouseClickEvent &>(event);
    bool clicked_something = false;
    for (auto node : nodes)
    {
        if (IsNodeHovered(node) && node->selected == false)
        {
            node->selected = true;
            clicked_something = true;
        }
        else
        {
            if (ImGui::GetIO().KeyCtrl == false)
                node->selected = false;
        }

        if (m_ConnectionProcedure.started && IsNodeHovered(node))
        {
            clicked_something = true;
            m_ConnectionProcedure.input_node = node;

            ApplyConnectionProcedure();
            Evaluate();
        }
        for (uint32_t i = 0; i < node->GetNumAvailableInputs(); i++)
        {
            auto ptr = static_cast<ImGuiNode *>(node.get());
            InputConnector *connector = ptr->GetInputConnector(i);
            if (connector->hovered)
            {
                clicked_something = true;
                connector->grabbed = true;

                m_ConnectionProcedure.started = true;
                m_ConnectionProcedure.output_node = node;
                m_ConnectionProcedure.output_index = i;
            }
        }
    }

    if (!clicked_something)
    {
        if (m_ConnectionProcedure.started)
        {
            m_ConnectionProcedure.started = false;
            m_ConnectionProcedure.output_node->ResetInput(m_ConnectionProcedure.output_index);
            ResetConnectionProcedure();
        }
    }
}

void NodeManager::OnMouseRelease(const Event &event)
{
    const MouseReleaseEvent &clickEvent = static_cast<const MouseReleaseEvent &>(event);
    for (auto node : nodes)
    {
        if (m_ConnectionProcedure.started && IsNodeHovered(node))
        {
            m_ConnectionProcedure.input_node = node;

            ApplyConnectionProcedure();
            Evaluate();
            // m_ConnectionProcedure.output_node->SetInput(m_ConnectionProcedure.output_index, m_ConnectionProcedure.input_node);
            // ResetConnectionProcedure();
        }
    }
}

void NodeManager::OnKeyPress(const Event &event)
{
    if (!m_CanvasHovered)
        return;
    const KeyPressEvent &clickEvent = static_cast<const KeyPressEvent &>(event);
    if (clickEvent.key == GLFW_KEY_F)
    {
        ViewFrameAll();
    }
}

void NodeManager::ViewFrameAll()
{
    ImVec2 center = get_nodes_center(nodes);
    m_Scrolling = -center + m_CanvasSize / 2.0f;
}
