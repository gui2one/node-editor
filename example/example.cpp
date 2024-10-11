#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"

#include <yaml-cpp/yaml.h>

using namespace NodeEditor;

YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

void serialize_nodes(std::vector<std::shared_ptr<ImGuiNode>> nodes) {
  YAML::Emitter out;
  for(auto node : nodes) {
    auto op = static_cast<StringOperator *>(node.get());
    out << op->title;
    out << glm::vec3(1.f, 2.f, 3.f);
  }

  std::cout << "Yaml output : \n"<< out.c_str() << std::endl;
  
}

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


  serialize_nodes(manager.GetNodes());

  manager.SetNodesMenu([&manager]() {
    node_menu_item<Node<StringGenerate>>(manager, "Generator");
    node_menu_item<Node<StringConcatenator>>(manager, "Concatenate");
    node_menu_item<Node<StringRepeater>>(manager, "Repeater");
    node_menu_item<Node<StringNull>>(manager, "Null");
  });

  EventManager::GetInstance().Subscribe(
      EventType::NodeConnection, [&app, &null_node](const Event &event) {
        auto &manager = app.GetNodeManager();
        manager.Evaluate();
        auto op = static_cast<StringOperator *>(manager.GetOutputNode().get());
        std::cout << "Connection Update -> " << op->m_StringCache << std::endl;
      });
  EventManager::GetInstance().Subscribe(
      EventType::ParamChanged, [&app, &null_node](const Event &event) {
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

  app.GetNodeManager().SetOutputNode(null_node);
  app.GetNodeManager().Evaluate();

  app.Run();

  std::cout << "__All Done__" << std::endl;

  return 0;
}