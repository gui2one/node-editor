#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "node_editor.h"

int main()
{

    Application app;  

    if(!app.Init()) {
        std::cout << "App Init() Error ..." << std::endl;
        return -1;
    };

    auto& manager = app.GetNodeManager();

    auto node1 = std::make_shared<ImGuiNode>("Node A");
    node1->position = ImVec2(500, 300);
    auto node2 = std::make_shared<ImGuiNode>("Node B");
    node2->position = ImVec2(620, 320);


    manager.AddNode(node1);
    manager.AddNode(node2);  

    app.SetLoopFunction([&app ](){
    
        
    });

    app.Run();

    std::cout << "Goodbye ..." << std::endl;
    printf("hey !!!!\n");

    return 0;
}