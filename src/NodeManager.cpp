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

void NodeManager::OnMouseMove(const Event& event)
{
    const MouseMoveEvent& moveEvent = static_cast<const MouseMoveEvent&>(event);
    std::cout << moveEvent.x << ", " << moveEvent.y << std::endl;
    
}

void NodeManager::OnMouseClick(const Event &event)
{
    const MouseClickEvent& clickEvent = static_cast<const MouseClickEvent&>(event);
    std::cout << "Mouse clicked with button " << clickEvent.button << "\n";
}
