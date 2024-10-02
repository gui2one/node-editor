#include "NodeManager.h"

NodeManager::NodeManager()
{
    // event system test 
    EventManager::GetInstance().Subscribe(EventType::MouseClick, [](const Event& event) {
        const MouseClickEvent& clickEvent = static_cast<const MouseClickEvent&>(event);
        std::cout << "Mouse clicked with button " << clickEvent.button << "\n";
    });    
    
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
