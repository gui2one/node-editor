#include "Application.h"
#include "StringGenerator.h"
#include "NumberOperator.h"
#include "node_editor.h"
#include "params.h"


using namespace NodeEditor;

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

  REGISTER_NODE_TYPE(NodeEditor::Add, "add", "operator");
  REGISTER_NODE_TYPE(NodeEditor::NumberConstant, "constant", "operator");
  Application app;

  if (!app.Init()) {
    std::cout << "App Init() Error ..." << std::endl;
    return -1;
  };

  auto &manager = app.GetNodeManager();

  manager.AddIcon("to_upper", "example_resources/icons/to_upper.png");


  static EventDispatcher &dispatcher = EventManager::GetInstance();

  dispatcher.Subscribe(EventType::NodeConnection, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.Evaluate();
    if (manager.GetOutputNode() != nullptr) {
      auto string_op = std::dynamic_pointer_cast<StringOperator>(manager.GetOutputNode());
      auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager.GetOutputNode());

      if(string_op != nullptr){
        std::cout << "String Connection Update -> " << string_op->m_DataCache << std::endl;
      }else if(number_op != nullptr){
        std::cout << "Number Connection Update -> " << number_op->m_DataCache << std::endl;
      }
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

      auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager.GetOutputNode());
      auto op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(manager.GetOutputNode());
      if (subnet_op != nullptr) {
        if (subnet_op->node_network.outuput_node != nullptr) {
          auto output_op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(subnet_op->node_network.outuput_node);
          std::cout << "m_DataCache -> " << output_op->m_DataCache << std::endl;
        }
      } else if (op != nullptr) {
        std::cout << "ManagerUpdate Event -> " << op->m_DataCache << std::endl;
      } else if (subnet_input_op != nullptr) {
        auto op2 = static_cast<ImGuiNode<std::string> *>(subnet_input_op->parent_node->GetInput(0).get());
        std::cout << "Subnet input Operator -> " << op2->m_DataCache << std::endl;

      } else if(number_op != nullptr){
        // auto op2 = static_cast<NumberOperator *>(subnet_input_op->parent_node->GetInput(0).get());
        std::cout << "Subnet input Operator -> " << number_op->m_DataCache << std::endl;
        
      

      }else {
        std::cout << "can't convert to Operator" << std::endl;
      }
    }
  });

  app.GetNodeManager().CreateAllNodes();
  app.GetNodeManager().ViewFrameAll();
  app.GetNodeManager().Evaluate();

  app.GetNodeManager().LoadFromFile(file_to_load);
  app.Run();

  std::cout << "__All Done__" << std::endl;

  return 0;
}
