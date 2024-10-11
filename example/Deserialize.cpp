#include "Deserialize.h"


std::shared_ptr<NodeEditor::ImGuiNode>
find_node_by_uuid(std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> nodes, std::string uuid){

    for(auto node : nodes) {
        if(node->uuid == uuid) {
            return node;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<NodeEditor::ImGuiNode>> 
deserialize_yaml_save(const char *filename)
{
    using namespace NodeEditor;
    std::cout << "Implementing load Save Right now !!!" << std::endl;
    YAML::Node yaml_data = YAML::LoadFile(filename);
    std::cout << "Yaml loaded" << std::endl;

    std::vector<std::shared_ptr<ImGuiNode>> nodes;
    for(auto item : yaml_data) {
        auto type_str = item["type"].as<std::string>();
        str_replace(type_str, "class NodeEditor::Node<class NodeEditor::", "");
        str_replace(type_str, ">", "");

        
        if(type_str == "StringGenerate"){  
            auto node = basic_node_convert<StringGenerate>(item);
            // node->value->value = item["params"]["value"].as<std::string>();
            nodes.push_back(node);
        }else if(type_str == "StringConcatenator"){
            auto node = basic_node_convert<StringConcatenator>(item);
            nodes.push_back(node);            
        }else if(type_str == "StringRepeater"){
            auto node = basic_node_convert<StringRepeater>(item);
            nodes.push_back(node);            
        }else if(type_str == "StringNull"){
            auto node = basic_node_convert<StringNull>(item);
            nodes.push_back(node);            
        }else{
            std::cout << item["type"] << " not IMPLEMENTED ...." << std::endl;
        }
    }

    // second pass to connect inputs
    for(auto item : yaml_data) {
        auto type_str = item["type"].as<std::string>();

        auto my_uuid = item["uuid"].as<std::string>();
        for(size_t i = 0; i < item["inputs"].size(); i++) {
            auto input_uuid = item["inputs"][i].as<std::string>();
            auto input_node = find_node_by_uuid(nodes, input_uuid);
            auto me = find_node_by_uuid(nodes, my_uuid);
            if(input_node != nullptr) {
                me->SetInput((uint32_t)i, find_node_by_uuid(nodes, input_uuid));
            }
        }

    }
    
    return nodes;
}
