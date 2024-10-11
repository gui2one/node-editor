#include "Serialize.h"

namespace NE = NodeEditor;
YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}
YAML::Emitter& operator << (YAML::Emitter& out, const ImVec2& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
}
YAML::Emitter& operator << (YAML::Emitter& out, const std::shared_ptr<NE::ImGuiNode>& node) {
	// auto op = static_cast<StringOperator *>(node.get());
  out << node->title;

  out << YAML::BeginMap;
    out << YAML::Key << "uuid";
    out << YAML::Value << node->uuid;
    
    out << YAML::Key << "position";
    out << node->position;
    out << YAML::Key << "params";
      out << YAML::BeginMap;
        for(auto param : node->m_Params) {
          out << YAML::Key << param->name;
          auto p_string = std::dynamic_pointer_cast<NE::Param<std::string>>(param);
          auto p_uint32_t = std::dynamic_pointer_cast<NE::Param<uint32_t>>(param);

          if( p_string) out << p_string->Eval();
          else if( p_uint32_t) out << p_uint32_t->Eval();
        }
    
      out << YAML::EndMap;
  out << YAML::EndMap;
	return out;
}


void serialize_nodes(std::vector<std::shared_ptr<NE::ImGuiNode>> nodes) {
  YAML::Emitter out;
  for(auto node : nodes) {
    // auto op = static_cast<StringOperator *>(node.get());
    out << node;
  }

  std::cout << "Yaml output : \n"<< out.c_str() << std::endl;
  
}


