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
};  // namespace NED
