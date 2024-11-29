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

  dispatcher.Subscribe(EventType::NodeConnection, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.Evaluate();
    if (manager.GetOutputNode() != nullptr) {
      auto string_op = std::dynamic_pointer_cast<StringOperator>(manager.GetOutputNode());
      auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager.GetOutputNode());

      if (string_op != nullptr) {
        std::cout << "String Connection Update -> " << string_op->m_DataCache << std::endl;
      } else if (number_op != nullptr) {
        std::cout << "Number Connection Update -> " << number_op->m_DataCache << std::endl;
      }
    }
  });
  dispatcher.Subscribe(EventType::ParamChanged, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    auto ev_string = dynamic_cast<const ParamChangedEvent<std::string> *>(&event);
    auto ev_wstring = dynamic_cast<const ParamChangedEvent<std::wstring> *>(&event);
    auto ev_bool = dynamic_cast<const ParamChangedEvent<bool> *>(&event);
    auto ev_int = dynamic_cast<const ParamChangedEvent<int> *>(&event);
    auto ev_vec2 = dynamic_cast<const ParamChangedEvent<glm::vec2> *>(&event);
    auto ev_vec3 = dynamic_cast<const ParamChangedEvent<glm::vec3> *>(&event);
    if (ev_string != nullptr) {
      std::cout << "Param Changed : " << ev_string->param_name << "\nNew Value : " << ev_string->new_value << std::endl;
    } else if (ev_wstring != nullptr) {
      std::string utf8_str = wide_to_utf8(ev_wstring->new_value);
      std::cout << "Param Changed : " << ev_wstring->param_name << "\nNew Value : " << utf8_str << std::endl;
    } else if (ev_bool != nullptr) {
      std::cout << "Param Changed : " << ev_bool->param_name
                << "\nNew Value : " << (ev_bool->new_value ? "true" : "false") << std::endl;
    } else if (ev_int != nullptr) {
      std::cout << "Param Changed : " << ev_int->param_name << "\nNew Value : " << ev_int->new_value << std::endl;
    } else if (ev_vec2 != nullptr) {
      std::cout << "Param Changed : " << ev_vec2->param_name << "\nNew Value : " << ev_vec2->new_value.x << ", "
                << ev_vec2->new_value.y << std::endl;
    } else if (ev_vec3 != nullptr) {
      std::cout << "Param Changed : " << ev_vec3->param_name << "\nNew Value : " << ev_vec3->new_value.x << ", "
                << ev_vec3->new_value.y << ", " << ev_vec3->new_value.z << std::endl;
    }
    // auto ev = dynamic_cast<const ParamChangedEvent *>(&event);
    manager.m_OneParamChanged = true;
  });
  dispatcher.Subscribe(EventType::ManagerUpdate, [&app](const Event &event) {
    auto &manager = app.GetNodeManager();
    manager.Evaluate();
    if (manager.GetOutputNode() != nullptr) {
      auto subnet_op = std::dynamic_pointer_cast<NED::SubnetNode<std::string>>(manager.GetOutputNode());
      auto subnet_input_op = std::dynamic_pointer_cast<SubnetInputNode<std::string>>(manager.GetOutputNode());

      auto number_op = std::dynamic_pointer_cast<NumberOperator>(manager.GetOutputNode());
      auto op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(manager.GetOutputNode());
      if (subnet_op != nullptr) {
        if (subnet_op->node_network.outuput_node != nullptr) {
          auto output_op = std::dynamic_pointer_cast<ImGuiNode<std::string>>(subnet_op->node_network.outuput_node);
          std::cout << "m_DataCache -> " << output_op->m_DataCache << std::endl;
        }
      } else if (op != nullptr) {
        std::cout << "ManagerUpdate Event !!-> " << op->m_DataCache << std::endl;
      } else if (subnet_input_op != nullptr) {
        auto op2 = static_cast<ImGuiNode<std::string> *>(subnet_input_op->parent_node->GetInput(0).get());
        std::cout << "Subnet input Operator -> " << op2->m_DataCache << std::endl;

      } else if (number_op != nullptr) {
        // auto op2 = static_cast<NumberOperator *>(subnet_input_op->parent_node->GetInput(0).get());
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
