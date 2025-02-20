#include "ImGuiNode.h"
namespace NED {

void NodeNetwork::AddNode(std::shared_ptr<AbstractNode> _node) {
  // collect nodes names
  std::unordered_set<std::string> names;
  for (auto node : nodes) {
    names.insert(node->title);
  }
  auto unique_name = generate_unique_name(_node->title, names);
  _node->title = unique_name;
  nodes.push_back(_node);
}
void NodeNetwork::RemoveNode(AbstractNode* _node) {
  if (_node == nullptr) return;
  auto it = std::find(nodes.begin(), nodes.end(), _node->get_shared_ptr());
  if (it == nodes.end()) return;
  nodes.erase(it);
}
std::string AbstractNode::GenerateParamsHash() {
  YAML::Node yaml_node = YAMLSerialize();
  /* reset position. I don't want it to affect hash*/
  yaml_node["position"] = ImVec2(0, 0);

  std::hash<std::string> hasher;
  size_t hash = hasher(YAML::Dump(yaml_node));

  std::stringstream stream;
  stream << std::hex << hash;

  std::string result(stream.str());
  return result;
}
};  // namespace NED
