#include <windows.h>
#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "node_editor.h"
#include "StringGenerator.h"


int main()

{

    Application app;  

    if(!app.Init()) {
        std::cout << "App Init() Error ..." << std::endl;
        return -1;
    };

    auto& manager = app.GetNodeManager();

    auto node1 = std::make_shared<Node<HelloGenerator>>("Generator");
    node1->position = ImVec2(500, 300);
    auto node2 = std::make_shared<Node<StringConcatenator>>("Concatenator");
    node2->position = ImVec2(600, 400);
    node2->SetInput(0, node1);


    manager.AddNode(node1);
    manager.AddNode(node2);  

    app.SetLoopFunction([&app ](){
    
        
    });

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
    app.GetNodeManager().Evaluate();
    app.Run();

    std::cout << "__All Done__" << std::endl;

    return 0;
}