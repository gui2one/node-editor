#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "node_editor.h"

int main()
{

    Application app;  

    if(!app.Init()) {
        std::cout << "Big Problem !!!" << std::endl;
        return -1;
    };

    auto& manager = app.GetNodeManager();

    auto node1 = std::make_shared<ImGuiNode>();
    node1->position = ImVec2(500, 300);
    node1->title = "Node 1";
    auto node2 = std::make_shared<ImGuiNode>();
    node2->title = "Node 2";
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