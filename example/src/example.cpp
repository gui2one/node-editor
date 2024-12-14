#include "Application.h"
#include "NumberOperator.h"
#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"

using namespace NED;

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

  // CREATE_UTILITY_CLASSES(float, "number/utility");
  // REGISTER_NODE_TYPE(NED::Add, "add", "number/math");
  // REGISTER_NODE_TYPE(NED::NumberConstant, "constant", "number");

  // "User" Param<T> type
  REGISTER_PARAM_TYPE(NED::ParamDouble);

  Application app;

  // needed for undo/redo to work on User Param<T> type
  app.GetNodeManager().ParamChangeSubscribe<double>();

  app.SetLoopFunction([&app]() {
    ImGui::Begin("user window");
    for (auto node : app.GetNodeManager().GetNodes()) {
      ImGui::Text("%s", node->title.c_str());
    }
    ImGui::End();
  });

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
      auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager.GetOutputNode());
      auto string_op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(manager.GetOutputNode());

      if (string_op != nullptr) {
        std::cout << "Render string : " << string_op->m_DataCache << std::endl;
      } else if (number_op != nullptr) {
        std::cout << "Number Op Update -> " << number_op->m_DataCache << std::endl;

      } else {
        std::cout << "Can't convert to any known Operator Type" << std::endl;
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
