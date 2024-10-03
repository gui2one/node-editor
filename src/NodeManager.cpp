#include "NodeManager.h"

NodeManager::NodeManager()
{
    // event system test 
    // EventManager::GetInstance().Subscribe(EventType::MouseClick, [](const Event& event) {
    //     const MouseClickEvent& clickEvent = static_cast<const MouseClickEvent&>(event);
    //     std::cout << "Mouse clicked with button " << clickEvent.button << "\n";
    // });    

    
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
   ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 offset = m_Origin;

    //draw connections first
    for(auto node : GetNodes()) {
        auto ptr = static_cast<ImGuiNode*>(node.get());
        for(uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
            
            if(ptr->GetInput(i) != nullptr) {
                auto other = ptr->GetInput(i);
                auto input_conn = ptr->GetInputConnector(i);
                ImVec2 p0 = node->position + input_conn->relative_pos + m_Origin;
                ImVec2 other_pos = other->position + offset + ImVec2(other->size.x/2.0f, other->size.y);
                
                float y_sep = other_pos.y - p0.y;
                ImVec2 ctrl1 = p0 + ImVec2(0, y_sep);
                ImVec2 ctrl2 = other_pos - ImVec2(0, y_sep);

                draw_list->AddBezierCubic(p0, ctrl1, ctrl2, other_pos, (ImU32)NODE_COLOR::GREY, 2.0f); // ImDrawList API uses screen coordinates()
            }
        }
    }
    
    for(auto node : GetNodes()) {
        auto ptr = static_cast<ImGuiNode*>(node.get());
        
        // input 'connectors'
        for(uint32_t i = 0; i < node->GetNumAvailableInputs(); i++) {
            auto input_conn = ptr->GetInputConnector(i);
            ImVec2 cp = node->position + offset + input_conn->relative_pos;
            draw_list->AddCircleFilled(cp, 5.0f, (ImU32)NODE_COLOR::WHITE);
        }

        //output 'connector'
        ImVec2 cp = node->position + offset + ImVec2(node->size.x/2.0f, node->size.y);
        draw_list->AddCircleFilled(cp, 5.0f, (ImU32)NODE_COLOR::WHITE); 

        ImVec2 min = node->position + offset;
        ImVec2 max = min + node->size;
        draw_list->AddRectFilled(min, max, node->color, 3.0f);

        draw_list->AddText(min + ImVec2(10.0f, 10.0f), IM_COL32(255, 255, 255, 255), node->title);   
        if(!node->highlighted){
            draw_list->AddRect(min, max, IM_COL32(50, 50, 50, 255), 3.0f);
        } else {
            draw_list->AddRect(min, max, IM_COL32(100, 100, 100, 255), 3.0f);
        }

        if(node->selected){
            draw_list->AddRect(min, max, IM_COL32(200, 200, 60, 100), 3.0f, 0, 3.0f);
        }
    }    
}

void NodeManager::DrawCanvas(){

    static ImVector<ImVec2> points;
    static ImVec2 scrolling(0.0f, 0.0f);
    static float scale = 1.0f;
    static bool opt_enable_grid = true;
    static bool opt_enable_context_menu = true;
    static bool adding_line = false;

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    

    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    m_Origin = ImVec2((canvas_p0.x + scrolling.x), (canvas_p0.y + scrolling.y)); // Lock scrolled origin
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - m_Origin.x, io.MousePos.y - m_Origin.y);


    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
    {
        scrolling.x += io.MouseDelta.x;
        scrolling.y += io.MouseDelta.y;
    }
    
    if(ImGui::IsItemClicked() || ImGui::IsItemHovered()) {

        scale += io.MouseWheel * 0.2f;
    
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

    // Draw grid + all lines in the canvas
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    if (opt_enable_grid)
    {
        const float GRID_STEP = 64.0f;
        for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
        for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
            draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    }
    for (int n = 0; n < points.Size; n += 2){

        draw_list->AddLine(ImVec2(m_Origin.x + points[n].x, m_Origin.y + points[n].y), ImVec2(m_Origin.x + points[n + 1].x, m_Origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
    }

    // m_NodeManager.m_Origin = m_Origin;
    DrawNodes();



    draw_list->PopClipRect();

    

}

void NodeManager::Evaluate()
{
    for(auto node : nodes) {

        node->Update();
    }
}

bool NodeManager::IsNodeHovered(std::shared_ptr<ImGuiNode> node)
{
    ImVec2 min = node->position + m_Origin;
    ImVec2 max = min + node->size;
    double cursor_x, cursor_y;
    glfwGetCursorPos(m_GLFWWindow, &cursor_x, &cursor_y);
    bool hovered = false;
    if(cursor_x > min.x && cursor_x < max.x && cursor_y > min.y && cursor_y < max.y) {
        hovered = true;
    }
    // bool hovered = ImGui::IsMouseHoveringRect(min, max);
    return hovered;    
}

void NodeManager::OnMouseMove(const Event& event)
{
    const MouseMoveEvent& moveEvent = static_cast<const MouseMoveEvent&>(event);
    static ImVec2 old_pos = ImVec2(0,0);
    ImVec2 delta = ImVec2(moveEvent.x - old_pos.x, moveEvent.y - old_pos.y);
    for(auto node : nodes) {
        if(node->selected && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            node->position.x += delta.x;
            node->position.y += delta.y;
        }
    }
    old_pos = ImVec2(moveEvent.x, moveEvent.y);
}

void NodeManager::OnMouseClick(const Event &event)
{
    const MouseClickEvent& clickEvent = static_cast<const MouseClickEvent&>(event);
    for(auto node : nodes) {
        if(IsNodeHovered(node) && node->selected == false) {
            node->selected = true;
        }else{
            if(ImGui::GetIO().KeyCtrl == false)
            node->selected = false;
        }
    }
}

void NodeManager::OnMouseRelease(const Event &event)
{
    const MouseReleaseEvent& clickEvent = static_cast<const MouseReleaseEvent&>(event);
}
