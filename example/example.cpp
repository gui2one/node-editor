#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "node_editor.h"
#include "MeshGenerator.h"
int main()
{

    Application app;  

    if(!app.Init()) {
        std::cout << "App Init() Error ..." << std::endl;
        return -1;
    };

    auto& manager = app.GetNodeManager();

    auto node1 = std::make_shared<Node<GridGenerator>>("Generator");
    node1->position = ImVec2(500, 300);
    auto node2 = std::make_shared<ImGuiNode>("Node B");
    node2->position = ImVec2(620, 320);
    node2->SetInput(0, node1);


    manager.AddNode(node1);
    manager.AddNode(node2);  

    app.SetLoopFunction([&app ](){
    
        
    });
    node1->Update();
    app.Run();

    std::cout << "__All Done__" << std::endl;

    return 0;
}