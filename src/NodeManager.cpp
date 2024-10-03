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


void NodeManager::Evaluate()
{
    for(auto node : nodes) {

        node->Update();
    }
}

bool NodeManager::IsNodeHovered(std::shared_ptr<ImGuiNode> node) {
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
    std::cout << "Mouse clicked with button " << clickEvent.button << "\n";
    for(auto node : nodes) {
        // std::cout << (IsNodeHovered(node) ? ">" : "") << node->title << std::endl;
        
        if(IsNodeHovered(node) && node->selected == false) {
            node->selected = true;
        }else{
            node->selected = false;
        }
    }
}

void NodeManager::OnMouseRelease(const Event &event)
{
    const MouseReleaseEvent& clickEvent = static_cast<const MouseReleaseEvent&>(event);
    // std::cout << "Mouse Released with button " << clickEvent.button << "\n";
}
