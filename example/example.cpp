#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"

using namespace NodeEditor;

void add_example_nodes(NodeManager &manager);
int main() {

  Application app;

  if (!app.Init()) {
    std::cout << "App Init() Error ..." << std::endl;
    return -1;
  };

  auto &manager = app.GetNodeManager();

  

  add_example_nodes(manager);

  serialize_nodes(manager.GetNodes());

  manager.SetNodesMenu([&manager]() {
    node_menu_item<Node<StringGenerate>>(manager, "Generator");
    node_menu_item<Node<StringConcatenator>>(manager, "Concatenate");
    node_menu_item<Node<StringRepeater>>(manager, "Repeater");
    node_menu_item<Node<StringNull>>(manager, "Null");
  });

  EventManager::GetInstance().Subscribe(
      EventType::NodeConnection, [&app](const Event &event) {
        auto &manager = app.GetNodeManager();
        manager.Evaluate();
        auto op = static_cast<StringOperator *>(manager.GetOutputNode().get());
        std::cout << "Connection Update -> " << op->m_StringCache << std::endl;
      });
  EventManager::GetInstance().Subscribe(
      EventType::ParamChanged, [&app](const Event &event) {
        auto &manager = app.GetNodeManager();
        manager.m_OneParamChanged = true;
      });
  EventManager::GetInstance().Subscribe(
      EventType::ManagerUpdate, [&app](const Event &event) {
        auto &manager = app.GetNodeManager();
        manager.Evaluate();
        auto op = static_cast<StringOperator *>(manager.GetOutputNode().get());
        std::cout << "ManagerUpdate Event -> " << op->m_StringCache << std::endl;

      });


  app.GetNodeManager().Evaluate();

  app.Run();

  std::cout << "__All Done__" << std::endl;

  return 0;
}

void add_example_nodes(NodeManager &manager) {

  auto hello_node = std::make_shared<Node<StringGenerate>>("Hello");
  hello_node->position = ImVec2(500, 300);
  manager.AddNode(hello_node);

  auto world_node = std::make_shared<Node<StringGenerate>>("World");
  world_node->value->value = "World";
  world_node->position = ImVec2(700, 300);
  manager.AddNode(world_node);

  auto excla_node = std::make_shared<Node<StringGenerate>>("!");
  excla_node->value->value = "!";
  excla_node->position = ImVec2(900, 300);
  manager.AddNode(excla_node);

  auto repeater_node = std::make_shared<Node<StringRepeater>>("Repeater");
  repeater_node->position = excla_node->position + ImVec2(0, 100);
  repeater_node->SetInput(0, excla_node);
  manager.AddNode(repeater_node);

  auto concat_node = std::make_shared<Node<StringConcatenator>>("Concatenator");
  concat_node->position = ImVec2(600, 400);
  concat_node->SetInput(0, hello_node);
  concat_node->SetInput(1, world_node);
  manager.AddNode(concat_node);

  auto concat_node2 =
      std::make_shared<Node<StringConcatenator>>("Concatenator2");
  concat_node2->position = ImVec2(650, 500);
  concat_node2->SetInput(0, concat_node);
  concat_node2->SetInput(1, repeater_node);
  manager.AddNode(concat_node2);
  
  auto output_node = std::make_shared<Node<StringNull>>("Output");
  output_node->position = ImVec2(650, 600);
  output_node->SetInput(0, concat_node2);
  manager.AddNode(output_node);

  manager.SetOutputNode(output_node);
}