#include "Application.h"
#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"


using namespace NodeEditor;

void add_example_nodes(NodeManager &manager);
int main(int argc, char *argv[]) {
  std::filesystem::path file_to_load = "";
  std::filesystem::path exe_path = argv[0];

#ifdef _WIN32
  SetCurrentDirectoryA(exe_path.parent_path().string().c_str());
#endif
  if (argc > 1) {
    file_to_load = argv[1];
  }



  REGISTER_NODE_TYPE(NodeEditor::StringGenerate, "generate", "generator");
  REGISTER_NODE_TYPE(NodeEditor::TextFileLoader, "Load Text File", "generator");
  REGISTER_NODE_TYPE(NodeEditor::StringConcatenator, "concatenator", "modifier");
  REGISTER_NODE_TYPE(NodeEditor::StringConcatenatorMulti, "concatmulti", "modifier");
  REGISTER_NODE_TYPE(NodeEditor::StringRepeater, "repeater", "modifier");
  REGISTER_NODE_TYPE(NodeEditor::StringToUpperCase, "To Upper Case", "modifier");
  REGISTER_NODE_TYPE(NodeEditor::StringNull, "null node", "utility");
  REGISTER_NODE_TYPE(NodeEditor::StringSubnetOperator, "Subnetwork", "utility");
  CREATE_SUBNET_INPUT_NODE_CLASS(std::string, "Subnet input", "utility");
  Application app;

  if (!app.Init()) {
    std::cout << "App Init() Error ..." << std::endl;
    return -1;
  };

  auto &manager = app.GetNodeManager();

  manager.AddIcon("to_upper", "example_resources/icons/to_upper.png");
  // add_example_nodes(manager);

  static EventDispatcher &dispatcher = EventManager::GetInstance();

  dispatcher.Subscribe(EventType::NodeConnection, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.Evaluate();
    if (manager.GetOutputNode() != nullptr) {
      auto op = static_cast<StringOperator *>(manager.GetOutputNode().get());
      std::cout << "Connection Update -> " << op->m_DataCache << std::endl;
    }
  });
  dispatcher.Subscribe(EventType::ParamChanged, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.m_OneParamChanged = true;
  });
  dispatcher.Subscribe(EventType::ManagerUpdate, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.Evaluate();
    if (manager.GetOutputNode() != nullptr) {
      auto subnet_op = std::dynamic_pointer_cast<StringSubnetOperator>(manager.GetOutputNode());
      auto subnet_input_op = std::dynamic_pointer_cast<SubnetInputNode<std::string>>(manager.GetOutputNode());
      auto op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(manager.GetOutputNode());
      if (subnet_op != nullptr) {
        if (subnet_op->node_network.outuput_node != nullptr) {
          auto output_op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(subnet_op->node_network.outuput_node);
          // std::cout << "Want Subnet Data Cache !!!!!!!!" << std::endl;
          std::cout << "m_DataCache -> " << output_op->m_DataCache << std::endl;
        }
      } else if (op != nullptr) {
        std::cout << "ManagerUpdate Event -> " << op->m_DataCache << std::endl;
      } else if (subnet_input_op != nullptr) {
        auto op2 = static_cast<ImGuiNode<std::string> *>(subnet_input_op->parent_node->GetInput(0).get());
        std::cout << "Subnet input Operator -> " << op2->m_DataCache << std::endl;
      } else {
        std::cout << "can't convert to Operator" << std::endl;
      }
    }
  });

  app.GetNodeManager().Evaluate();

  app.GetNodeManager().LoadFromFile(file_to_load);
  app.Run();

  std::cout << "__All Done__" << std::endl;

  return 0;
}

void add_example_nodes(NodeManager &manager) {
  auto hello_node = std::make_shared<Node<StringGenerate>>("Hello");
  hello_node->position = ImVec2(550, 300);
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

  auto concat_node2 = std::make_shared<Node<StringConcatenator>>("Concatenator2");
  concat_node2->position = ImVec2(650, 500);
  concat_node2->SetInput(0, concat_node);
  concat_node2->SetInput(1, repeater_node);
  manager.AddNode(concat_node2);

  auto output_node = std::make_shared<Node<StringNull>>("Output");
  output_node->position = ImVec2(650, 600);
  output_node->SetInput(0, concat_node2);
  manager.AddNode(output_node);

  auto concat_multi_node = std::make_shared<Node<StringConcatenatorMulti>>("ConcatenatorMulti");
  concat_multi_node->position = ImVec2(650, 700);
  concat_multi_node->AppendInput(hello_node);
  concat_multi_node->AppendInput(world_node);
  concat_multi_node->AppendInput(excla_node);
  manager.AddNode(concat_multi_node);

  auto subnet = std::make_shared<Node<StringSubnetOperator>>("Subnet");
  subnet->position = ImVec2(0, 0);
  // subnet->SetInput(0, concat_multi_node);
  manager.AddNode(subnet);

  manager.SetOutputNode(output_node);
}