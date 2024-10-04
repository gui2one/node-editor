#include <windows.h>
#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "node_editor.h"
#include "StringGenerator.h"


int main(){
    Application app;  

    if(!app.Init()) {
        std::cout << "App Init() Error ..." << std::endl;
        return -1;
    };

    auto& manager = app.GetNodeManager();

    auto hello_node = std::make_shared<Node<HelloGenerator>>("Hello");
    hello_node->position = ImVec2(500, 300);
    auto world_node = std::make_shared<Node<WorldGenerator>>("World");
    world_node->position = ImVec2(650, 300);
    auto concat_node = std::make_shared<Node<StringConcatenator>>("Concatenator");
    concat_node->position = ImVec2(600, 400);
    concat_node->SetInput(0, hello_node);
    concat_node->SetInput(1, world_node);
    auto concat_node2 = std::make_shared<Node<StringConcatenator>>("Concatenator2");
    concat_node2->position = ImVec2(600, 600);
    concat_node2->SetInput(0, concat_node);
    concat_node2->SetInput(1, world_node);


    manager.AddNode(hello_node);
    manager.AddNode(world_node);
    manager.AddNode(concat_node);  
    manager.AddNode(concat_node2);  
  

    manager.SetNodesMenu([&manager ](){
        if (ImGui::MenuItem("Generator", NULL, false, true)) 
        { 
            manager.AddNode(std::make_shared<Node<HelloGenerator>>("Generator"));            
        }
        if (ImGui::MenuItem("Concatenator", NULL, false, true)) 
        { 
            manager.AddNode(std::make_shared<Node<StringConcatenator>>("Concatenator"));
        }
    });

    EventManager::GetInstance().Subscribe(EventType::NodeConnection,[&app, &concat_node2](const Event& event){
        auto& manager = app.GetNodeManager();
        manager.Evaluate();
        std::cout << "Connection Update -> "<< concat_node2->m_StringCache << std::endl;
        
    });

    app.GetNodeManager().SetOutputNode(concat_node2);
    app.GetNodeManager().Evaluate();
    
    app.Run();

    std::cout << "__All Done__" << std::endl;

    return 0;
}