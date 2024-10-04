#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>


#include "StringGenerator.h"
#include "node_editor.h"


using namespace NodeEditor;

int main() {
  Application app;

  if (!app.Init()) {
    std::cout << "App Init() Error ..." << std::endl;
    return -1;
  };

  auto &manager = app.GetNodeManager();

  auto hello_node = std::make_shared<Node<StringGenerate>>("Hello");
  hello_node->position = ImVec2(500, 300);
  auto world_node = std::make_shared<Node<StringGenerate>>("World");
  world_node->position = ImVec2(650, 300);
  auto concat_node = std::make_shared<Node<StringConcatenator>>("Concatenator");
  concat_node->position = ImVec2(600, 400);
  concat_node->SetInput(0, hello_node);
  concat_node->SetInput(1, world_node);
  auto concat_node2 =
      std::make_shared<Node<StringConcatenator>>("Concatenator2");
  concat_node2->position = ImVec2(650, 500);
  concat_node2->SetInput(0, concat_node);
  concat_node2->SetInput(1, world_node);

  auto null_node = std::make_shared<Node<StringNull>>("Null");
  null_node->position = ImVec2(650, 600);
  null_node->SetInput(0, concat_node2);

  manager.AddNode(hello_node);
  manager.AddNode(world_node);
  manager.AddNode(concat_node);
  manager.AddNode(concat_node2);
  manager.AddNode(null_node);

  manager.SetNodesMenu([&manager]() {
    if (ImGui::MenuItem("Generator", NULL, false, true)) {
      manager.AddNode(std::make_shared<Node<StringGenerate>>("Generator"));
    }
    if (ImGui::MenuItem("Concatenator", NULL, false, true)) {
      manager.AddNode(
          std::make_shared<Node<StringConcatenator>>("Concatenator"));
    }
    if (ImGui::MenuItem("Repeater", NULL, false, true)) {
      manager.AddNode(std::make_shared<Node<StringRepeater>>("Repeater"));
    }
  });

  EventManager::GetInstance().Subscribe(
      EventType::NodeConnection, [&app, &null_node](const Event &event) {
        auto &manager = app.GetNodeManager();
        manager.Evaluate();
        auto op = static_cast<StringOperator *>(manager.GetOutputNode().get());
        std::cout << "Connection Update -> " << op->m_StringCache << std::endl;
      });

  app.GetNodeManager().SetOutputNode(null_node);
  app.GetNodeManager().Evaluate();

  app.Run();

  std::cout << "__All Done__" << std::endl;

  return 0;
}