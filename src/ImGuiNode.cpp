#include "ImGuiNode.h"
namespace NED {

void NodeNetwork::AddNode(std::shared_ptr<AbstractNode> _node) {
    // collect nodes names 
    std::unordered_set<std::string> names;
    for(auto node : nodes) {
        names.insert(node->title);
    }
    auto unique_name = generate_unique_name(_node->title, names);
    _node->title = unique_name;
    nodes.push_back(_node);
}
};
