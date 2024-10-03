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

    manager.AddNode(hello_node);
    manager.AddNode(world_node);
    manager.AddNode(concat_node);  

    app.SetNodesMenu([&app ](){
        if (ImGui::MenuItem("Generator", NULL, false, true)) 
        { 
            app.GetNodeManager().AddNode(std::make_shared<Node<HelloGenerator>>("Generator"));            
        }
        if (ImGui::MenuItem("Concatenator", NULL, false, true)) 
        { 
            app.GetNodeManager().AddNode(std::make_shared<Node<StringConcatenator>>("Concatenator"));
        }
    });
    // app.GetNodeManager().Evaluate();
    concat_node->Update(); // call Update() on the 'last' node only should trigger Update() on all preceding nodes
    app.Run();

    std::cout << "__All Done__" << std::endl;

    return 0;
}