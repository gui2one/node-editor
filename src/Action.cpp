#include "Action.h"

#include "NodeManager.h"
namespace NED {
MoveNodeAction::MoveNodeAction(AbstractNode* node, ImVec2 from_pos, ImVec2 to_pos)
    : from_pos(from_pos), to_pos(to_pos), m_Node(node) {
  message = std::format("Moved Node from {},{} to {},{}", from_pos.x, from_pos.y, to_pos.x, to_pos.y);
}

void MoveNodeAction::Do() { m_Node->position = to_pos; }
void MoveNodeAction::Undo() { m_Node->position = from_pos; }

NodeConnectAction::NodeConnectAction(AbstractNode* new_parent_node, AbstractNode* old_parent_node,
                                     AbstractNode* child_node, uint32_t child_input_index)
    : m_NewParentNode(new_parent_node),
      m_OldParentNode(old_parent_node),
      m_ChildNode(child_node),
      m_ChildInputIndex(child_input_index) {}

void NodeConnectAction::Do() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, m_NewParentNode);
  } else {
    m_ChildNode->AppendInput(m_NewParentNode);
  }
}

void NodeConnectAction::Undo() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, m_OldParentNode);
    if (m_OldParentNode != nullptr) {
      m_ChildNode->ClearCache();
    }
  } else {
    m_ChildNode->RemoveLastInput();
  }
}

NodeDisconnectAction::NodeDisconnectAction(AbstractNode* parent_node, AbstractNode* child_node,
                                           uint32_t child_input_index)
    : m_ParentNode(parent_node), m_ChildNode(child_node), m_ChildInputIndex(child_input_index) {}

void NodeDisconnectAction::Do() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, nullptr);
    m_ChildNode->ClearCache();
  } else {
    m_ChildNode->RemoveLastInput();
  }
}

void NodeDisconnectAction::Undo() {
  if (m_ChildNode->IsMultiInput() == false) {
    m_ChildNode->SetInput(m_ChildInputIndex, m_ParentNode);
  } else {
    m_ChildNode->AppendInput(m_ParentNode);
  }
}

NodeCreateAction::NodeCreateAction(NodeManager* node_manager, NodeNetwork* network, std::string type_name,
                                   ImVec2 position)
    : m_NodeManager(node_manager), m_NodeNetwork(network), m_TypeName(type_name), m_Position(position) {
  std::cout << "Creating Node of type: " << m_TypeName << std::endl;
  std::cout << "Num nodes in network : " << m_NodeNetwork->nodes.size() << std::endl;
  m_Node = NodeFactoryRegistry::GetInstance().Create(m_TypeName);
  m_Node->parent_node = network->owner.get();
  m_Node->position = m_Position;
  m_NodeNetwork->AddNode(m_Node);
}

void NodeCreateAction::Do() { m_NodeNetwork->AddNode(m_Node); }

void NodeCreateAction::Undo() {
  m_NodeNetwork->RemoveNode(m_Node.get());
  if (m_NodeManager->m_CurrentNode == m_Node) {
    m_NodeManager->m_CurrentNode = nullptr;
  }
}

NodeDeleteAction::NodeDeleteAction(NodeManager* node_manager, NodeNetwork* network, std::shared_ptr<AbstractNode> node)
    : m_NodeManager(node_manager), m_NodeNetwork(network), m_Node(node) {}

void NodeDeleteAction::Do() { m_NodeNetwork->RemoveNode(m_Node.get()); }

void NodeDeleteAction::Undo() { m_NodeNetwork->AddNode(m_Node); }

MoveMultipleNodesAction::MoveMultipleNodesAction(std::vector<AbstractNode*> nodes, std::vector<ImVec2> from_positions,
                                                 std::vector<ImVec2> to_positions)
    : nodes(nodes), from_positions(from_positions), to_positions(to_positions) {}

void MoveMultipleNodesAction::Do() {
  for (size_t i = 0; i < nodes.size(); i++) {
    nodes[i]->position = to_positions[i];
  }
}

void MoveMultipleNodesAction::Undo() {
  for (size_t i = 0; i < nodes.size(); i++) {
    nodes[i]->position = from_positions[i];
  }
}

SelectionChangedAction::SelectionChangedAction(NodeManager* node_manager, std::vector<AbstractNode*> old_selection,
                                               std::vector<AbstractNode*> new_selection)
    : node_manager(node_manager), old_selection(old_selection), new_selection(new_selection) {}

void SelectionChangedAction::Do() {
  Utils::deselect_all(node_manager->GetNodes());
  node_manager->m_ViewProps.selected_nodes = new_selection;
  for (auto node : new_selection) {
    node->selected = true;
  }
}

void SelectionChangedAction::Undo() {
  Utils::deselect_all(node_manager->GetNodes());
  node_manager->m_ViewProps.selected_nodes = old_selection;
  for (auto node : old_selection) {
    node->selected = true;
  }
}

};  // namespace NED