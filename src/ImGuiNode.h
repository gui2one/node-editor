#ifndef NODE_EDITOR_IMGUINODE_H
#define NODE_EDITOR_IMGUINODE_H
#pragma once

#include <imgui.h>
#include <yaml-cpp/yaml.h>

#include <array>
#include <iostream>
#include <memory>
#include <vector>

#include "Node.h"
#include "utils.h"
#include "yaml_serialize.h"

namespace NED {

class AbstractNode;

enum NODE_COLOR {
  BLACK = (ImU32)IM_COL32(0, 0, 0, 255),
  BLUE = (ImU32)IM_COL32(0, 0, 255, 255),
  DARK_BLUE = (ImU32)IM_COL32(0, 0, 100, 255),
  BROWN = (ImU32)IM_COL32(165, 42, 42, 255),
  CYAN = (ImU32)IM_COL32(0, 255, 255, 255),
  GREEN = (ImU32)IM_COL32(0, 255, 0, 255),
  DARK_GREEN = (ImU32)IM_COL32(0, 100, 0, 255),
  LIGHT_GREY = (ImU32)IM_COL32(160, 160, 160, 255),
  GREY = (ImU32)IM_COL32(128, 128, 128, 255),
  DARK_GREY = (ImU32)IM_COL32(32, 32, 32, 255),
  INDIGO = (ImU32)IM_COL32(75, 0, 130, 255),
  LIME = (ImU32)IM_COL32(0, 255, 0, 255),
  LIGHT_BLUE = (ImU32)IM_COL32(173, 216, 230, 255),
  MAGENTA = (ImU32)IM_COL32(255, 0, 255, 255),
  MAROON = (ImU32)IM_COL32(128, 0, 0, 255),
  ORANGE = (ImU32)IM_COL32(255, 128, 0, 255),
  PINK = (ImU32)IM_COL32(255, 192, 203, 255),
  PURPLE = (ImU32)IM_COL32(128, 0, 128, 255),
  RED = (ImU32)IM_COL32(255, 0, 0, 255),
  TURQUOISE = (ImU32)IM_COL32(64, 224, 208, 255),
  WHITE = (ImU32)IM_COL32(255, 255, 255, 255),
  YELLOW = (ImU32)IM_COL32(230, 175, 0, 255)
};

struct NodeNetwork {
  std::shared_ptr<AbstractNode> outuput_node = nullptr;
  std::shared_ptr<AbstractNode> owner = nullptr;
  std::vector<std::shared_ptr<AbstractNode>> nodes;

  void AddNode(std::shared_ptr<AbstractNode> _node);
  void RemoveNode(AbstractNode* _node);
};

struct ParamLayout {
  std::vector<std::shared_ptr<NodeParam>> params;
  std::shared_ptr<NodeParam> Get(size_t idx) { return params[idx]; }
  inline void Append(std::shared_ptr<NodeParam> new_param) { params.push_back(new_param); }
  inline void Append(std::vector<std::shared_ptr<NodeParam>> new_params) {
    this->params.insert(this->params.end(), new_params.begin(), new_params.end());
  }
};
struct InputConnector {
  ImVec2 relative_pos;
  uint32_t index = 0;
  float width = 5.0f;
  bool hovered = false;
  bool grabbed = false;
};

struct InputInfo {
  AbstractNode* node = nullptr;
  uint32_t connector_index = 0;
};
class AbstractNode : public std::enable_shared_from_this<AbstractNode> {
 public:
  AbstractNode(std::string _title) : title(_title), position(50, 0), size(100, 30), color(NODE_COLOR::DARK_GREY) {
    uuid = generate_uuid();
  };
  virtual ~AbstractNode() = default;

  std::shared_ptr<AbstractNode> get_shared_ptr() { return shared_from_this(); }
  virtual void Update() = 0;    // implemented in Node.h, in the Node<T> class
  virtual void Generate() = 0;  // user defined method. i.e the work the node is doint for the user app
  virtual void ClearCache() = 0;

  YAML::Node YAMLSerialize() {
    YAML::Node yaml_node;
    yaml_node["title"] = title;
    std::string type_str = std::string(m_TypeName);
    yaml_node["type"] = type_str;
    yaml_node["uuid"] = uuid;
    yaml_node["position"] = position;
    yaml_node["size"] = size;
    yaml_node["color"] = (ImU32)color;

    for (auto item : m_ParamLayout.params) {
      yaml_node["params"].push_back(item->YAMLSerialize());
    }

    for (size_t i = 0; i < MAX_N_INPUTS; i++) {
      YAML::Node input_info;
      if (inputs[i].node != nullptr) {
        input_info["node"] = inputs[i].node->uuid;
        input_info["connector"] = inputs[i].connector_index;
      } else {
        input_info["node"] = "null";
        input_info["connector"] = 0;
      }
      yaml_node["inputs"].push_back(input_info);
    }

    // TODO : fix this. It doesn't use the new InputInfo struct
    yaml_node["is_multi_input"] = IsMultiInput();
    if (IsMultiInput()) {
      for (size_t i = 0; i < GetMultiInputCount(); i++) {
        if (GetMultiInput(i).node != nullptr) {
          InputInfo info;
          info = GetMultiInput(i);
          YAML::Node input_info;
          input_info["node"] = info.node->uuid;
          input_info["connector"] = info.connector_index;
          yaml_node["multi_input"].push_back(input_info);
        }
      }
    }

    yaml_node["is_subnet"] = IsSubnet();
    if (IsSubnet()) {
      yaml_node["is_subnet"] = true;
      yaml_node["node_network"] = serialize_network(node_network);
    }
    return yaml_node;
  }

  void SetInput(uint32_t index, AbstractNode* node, uint32_t connector_index = 0) {
    if (index < 0 || index > 3) return;
    inputs[index].node = node;
    inputs[index].connector_index = connector_index;
  }
  void ResetInput(uint32_t index) {
    if (index < 0 || index > 3) return;
    inputs[index].node = nullptr;
    inputs[index].connector_index = 0;
  }
  AbstractNode* GetInput(uint32_t index) {
    if (index < 0 || index > 3) return nullptr;
    return inputs[index].node;
  }

  InputConnector* GetInputConnector(uint32_t index) {
    if (index < 0 || index >= GetNumAvailableInputs()) {
      std::cout << "Problem with GetInputConnector" << std::endl;

      return nullptr;
    }
    return &m_InputConnectors[index];
  }
  InputInfo RemoveLastInput() {
    auto save = m_MultiInput[m_MultiInput.size() - 1];
    if (m_MultiInput.size() > 0) {
      m_MultiInput.pop_back();
    }
    return save;
  }

  inline uint32_t GetNumAvailableInputs() const { return m_NumAvailableInputs; }

  inline size_t GetMultiInputCount() { return m_MultiInput.size(); }
  inline InputInfo& GetMultiInput(size_t index) { return m_MultiInput[index]; }
  inline void AppendInput(AbstractNode* node, uint32_t connector = 0) {
    InputInfo info;
    info.node = node;
    info.connector_index = connector;
    m_MultiInput.push_back(info);
  }

  inline void ActivateMultiInput() { m_IsMultiInput = true; }
  inline bool IsMultiInput() const { return m_IsMultiInput; }

  inline void ActivateSubnet() { m_IsSubnet = true; }
  inline bool IsSubnet() const { return m_IsSubnet; }

  inline void ActivateSubnetInputNode() { m_IsSubnetInputNode = true; }
  inline bool IsSubnetInputNode() { return m_IsSubnetInputNode; }

  inline void SetNumAvailableInputs(uint32_t num) {
    if (num > MAX_N_INPUTS) {
      std::cout << "Too many inputs" << std::endl;
      num = MAX_N_INPUTS;
    }
    m_NumAvailableInputs = num;
    InitInputConnectors();
  }

  inline void InitInputConnectors() {
    m_InputConnectors.clear();
    uint32_t num_spots = GetNumAvailableInputs();

    if (num_spots == 0) return;

    float spot_width = size.x / num_spots;
    for (uint32_t i = 0; i < num_spots; i++) {
      float x = spot_width * i + spot_width / 2.0f;
      float y = -4.0f;
      InputConnector connector;
      connector.index = i;
      connector.relative_pos = ImVec2(x, y);
      m_InputConnectors.push_back(connector);
    }
  }

  std::string GenerateParamsHash();

 private:
  uint32_t m_NumAvailableInputs = 1;
  bool m_IsMultiInput = false;
  bool m_IsSubnet = false;
  bool m_IsSubnetInputNode = false;
  // bool m_IsNullNode = false;

 public:
  bool m_IsCooking = false;
  const char* icon_name = "";
  const char* m_TypeName = "no_type";
  NodeNetwork node_network;
  AbstractNode* parent_node = nullptr;
  std::string uuid;
  std::string title;
  NODE_COLOR color;
  ImVec2 position;
  ImVec2 size;
  ParamLayout m_ParamLayout;
  bool m_Serializable = true;

  bool selected = false;
  bool preselected = false;
  bool grabbed = false;
  bool highlighted = false;

  std::array<InputInfo, MAX_N_INPUTS> inputs = {nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0};
  std::vector<InputInfo> m_MultiInput;
  std::vector<InputConnector> m_InputConnectors;

  std::string m_ParamsHash = "";
};

template <typename T>
class ImGuiNode : public AbstractNode {
 public:
  ImGuiNode(std::string _title) : AbstractNode(_title) {}
  virtual ~ImGuiNode() = default;

 public:
  T m_DataCache;
};

template <typename T>
class SubnetNode : public ImGuiNode<T> {
 public:
  SubnetNode() : ImGuiNode<T>("no title") {
    this->SetNumAvailableInputs(MAX_N_INPUTS);
    this->ActivateSubnet();

    this->icon_name = "three_dots";
  }
  void Generate() override {
    if (this->node_network.outuput_node != nullptr) {
      auto op = std::dynamic_pointer_cast<ImGuiNode<T>>(this->node_network.outuput_node);
      if (op != nullptr) {
        this->m_DataCache = op->m_DataCache;
      }
    }
  }
  void ClearCache() override { this->m_DataCache = T(); }

  // public:
  //  T m_DataCache;
};

template <typename T>
class SubnetInputNode : public ImGuiNode<T> {
 public:
  SubnetInputNode() : ImGuiNode<T>("subnet input") {
    this->ActivateSubnetInputNode();
    this->SetNumAvailableInputs(0);

    this->size.x = 50.0f;
    this->size.y = 50.0f;

    this->input_id = CREATE_PARAM(NED::ParamInt, "input id", this);
    this->input_id->Set(0);
    this->m_ParamLayout.params.push_back(input_id);

    this->icon_name = "arrow";
  }

  ~SubnetInputNode() {};

  void Generate() override {
    auto _parent_node = static_cast<SubnetNode<T>*>(this->parent_node);
    if (_parent_node != nullptr) {
      this->parent_node->Generate();

      uint32_t id = this->input_id->Eval();
      if (_parent_node->GetInput(id) != nullptr) {
        auto op = static_cast<ImGuiNode<T>*>(_parent_node->GetInput(id));
        this->m_DataCache = op->m_DataCache;
      }
    } else {
      std::cout << "NO Parent node defined" << std::endl;
    }
  }

  void ClearCache() override { this->m_DataCache = T(); }

 public:
  // T m_DataCache;

  std::shared_ptr<Param<int>> input_id;
};
template <typename T>
class NullNode : public ImGuiNode<T> {
 public:
  NullNode() : ImGuiNode<T>("no title") { this->SetNumAvailableInputs(1); }

  void Generate() override {
    if (this->GetInput(0) != nullptr) {
      auto op0 = dynamic_cast<ImGuiNode<T>*>(this->GetInput(0));
      auto subnetinput_op = dynamic_cast<SubnetInputNode<T>*>(this->GetInput(0));
      auto subnet_op = dynamic_cast<SubnetNode<T>*>(this->GetInput(0));

      if (op0 != nullptr) {
        this->m_DataCache = op0->m_DataCache;
      } else if (subnetinput_op != nullptr) {
        this->m_DataCache = subnetinput_op->m_DataCache;
      } else if (subnet_op != nullptr) {
        this->m_DataCache = subnet_op->m_DataCache;
      }
    }
  }

  void ClearCache() override { this->m_DataCache = T(); }
};
};  // namespace NED

#endif