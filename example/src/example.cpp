#include "Application.h"
#include "NumberOperator.h"
#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"

using namespace NED;

union {
  ImGuiNode<std::string> *node;
  SubnetNode<std::string> *subnet_node;
  SubnetInputNode<std::string> *subnet_input_node;
} NodeUnion;

int main(int argc, char *argv[]) {
  std::filesystem::path file_to_load = "";
  std::filesystem::path exe_path = argv[0];

#ifdef _WIN32
  SetCurrentDirectoryA(exe_path.parent_path().string().c_str());
#endif
  if (argc > 1) {
    file_to_load = argv[1];
  }

  CREATE_UTILITY_CLASSES(std::string, "string/utility");
  REGISTER_NODE_TYPE(NED::StringGenerate, "generate", "string/generator");
  REGISTER_NODE_TYPE(NED::StringConcatenator, "concatenator", "string/modifier");
  REGISTER_NODE_TYPE(NED::TextFileLoader, "load_text_file", "string/generator");
  REGISTER_NODE_TYPE(NED::StringConcatenatorMulti, "concatmulti", "string/modifier");
  REGISTER_NODE_TYPE(NED::StringRepeater, "repeater", "string/modifier");
  REGISTER_NODE_TYPE(NED::StringToUpperCase, "to_upper_case", "string/modifier");

  REGISTER_NODE_TYPE(NED::Add, "add", "number/math");
  REGISTER_NODE_TYPE(NED::NumberConstant, "constant", "number");
  Application app;

  if (!app.Init()) {
    std::cout << "App Init() Error ..." << std::endl;
    return -1;
  };

  auto &manager = app.GetNodeManager();

  manager.AddIcon("to_upper", "example_resources/icons/to_upper.png");

  static EventDispatcher &dispatcher = EventManager::GetInstance();

  dispatcher.Subscribe(EventType::ParamChanged, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.m_OneParamChanged = true;
  });
  dispatcher.Subscribe(EventType::ManagerUpdate, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.Evaluate();
    if (manager.GetOutputNode() != nullptr) {
      auto subnet_input_op = std::dynamic_pointer_cast<SubnetInputNode<std::string>>(manager.GetOutputNode());

      auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager.GetOutputNode());
      auto op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(manager.GetOutputNode());

      if (op != nullptr) {
        std::cout << "ManagerUpdate Event !!-> " << op->m_DataCache << std::endl;
      } else if (subnet_input_op != nullptr) {
        auto op2 = static_cast<ImGuiNode<std::string> *>(subnet_input_op->parent_node->GetInput(0));
        std::cout << "Subnet input Operator -> " << op2->m_DataCache << std::endl;

      } else if (number_op != nullptr) {
        // auto op2 = static_cast<NumberOperator *>(subnet_input_op->parent_node->GetInput(0));
        std::cout << "Subnet input Operator -> " << number_op->m_DataCache << std::endl;

      } else {
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
