#include <thread>

#include "Application.h"
#include "NumberOperator.h"
#include "StringGenerator.h"
#include "node_editor.h"
#include "params.h"
#include "ui_utils.h"
using namespace NED;

static void worker_thread(NodeManager *manager) {
  manager->Evaluate();
  if (manager->GetOutputNode() != nullptr) {
    auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager->GetOutputNode());
    auto string_op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(manager->GetOutputNode());

    if (string_op != nullptr) {
      // STRING_RESULT = string_op->m_DataCache;
      // std::cout << "Render string : " << STRING_RESULT << std::endl;
    } else if (number_op != nullptr) {
      std::cout << "Number Op Update -> " << number_op->m_DataCache << std::endl;

    } else {
      std::cout << "Can't convert to any known Operator Type" << std::endl;
    }
  }
  std::cout << "working ...." << std::endl;
}

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

  std::string STRING_RESULT = "";
  Application app;
  if (!app.Init()) {
    std::cout << "App Init() Error ..." << std::endl;
    return -1;
  };

  ImGuiIO &io = ImGui::GetIO();

  auto big_font = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Regular.ttf", 32);
  io.Fonts->Build();

  // needed for undo/redo to work on User Param<T> type
  app.GetNodeManager().ParamChangeSubscribe<double>();

  app.UserFunction([&]() {
    UI::Begin("Render");
    ImGui::PushFont(big_font);
    ImGui::Text("%s", STRING_RESULT.c_str());
    ImGui::PopFont();
    UI::End();
  });

  auto &manager = app.GetNodeManager();

  manager.AddIcon("to_upper", "example_resources/icons/to_upper.png");

  static EventDispatcher &dispatcher = EventManager::GetInstance();
  dispatcher.Subscribe(EventType::SaveFile, [&app](const Event &event) {
    (void)event;  // Suppress unused parameter warning
    std::cout << "Save File event" << std::endl;
  });
  dispatcher.Subscribe(EventType::LoadFile, [&app](const Event &event) {
    (void)event;  // Suppress unused parameter warning
    std::cout << "Load File event" << std::endl;
  });
  dispatcher.Subscribe(EventType::ParamChanged, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.m_OneParamChanged = true;
    manager.m_ViewProps.fileChanged = true;
    glfwSetWindowTitle(app.GetNativeWindow(),
                       std::format("{}-- modified", app.GetNodeManager().m_SavePath.string()).c_str());
  });
  dispatcher.Subscribe(EventType::ManagerUpdate, [&app, &STRING_RESULT](const Event &event) {
    auto &manager = app.GetNodeManager();

    std::thread t(worker_thread, &manager);
    t.detach();
    // manager.Evaluate();
    // if (manager.GetOutputNode() != nullptr) {
    //   auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager.GetOutputNode());
    //   auto string_op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(manager.GetOutputNode());

    //  if (string_op != nullptr) {
    //    STRING_RESULT = string_op->m_DataCache;
    //    std::cout << "Render string : " << STRING_RESULT << std::endl;
    //  } else if (number_op != nullptr) {
    //    std::cout << "Number Op Update -> " << number_op->m_DataCache << std::endl;

    //  } else {
    //    std::cout << "Can't convert to any known Operator Type" << std::endl;
    //  }
    //}
  });

  app.GetNodeManager().CreateAllNodes();
  app.GetNodeManager().ViewFrameAll();
  app.GetNodeManager().Evaluate();

  app.GetNodeManager().LoadFromFile(file_to_load);

  app.Run();

  std::cout << "__All Done__" << std::endl;

  return 0;
}
