#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H


#include "ImguiNode.h"
#include "NodeParam.h"
#include "params.h"
#include <iostream>
#include <string>
#include <type_traits>

namespace NodeEditor {

class StringOperator : public ImGuiNode<std::string> {
public:
  StringOperator() : ImGuiNode("default") {};
  virtual ~StringOperator() = default;

};

class StringSubnetOperator : public SubnetNode {
public:
  StringSubnetOperator() : SubnetNode() {};
  virtual ~StringSubnetOperator() = default;

  void Generate() override {
    if (GetInput(0) != nullptr) {
      auto op0 = std::dynamic_pointer_cast<StringOperator>(GetInput(0));
      if(op0 != nullptr){
        // m_DataCache = op0->m_DataCache;
      }
    }else{

      // m_DataCache = "!!!";
    }

    if(node_network.outuput_node != nullptr){
      node_network.outuput_node->Update();
      auto op = std::dynamic_pointer_cast<StringOperator>(node_network.outuput_node);
      if(op != nullptr){
        
        // m_DataCache = op->m_DataCache;
      }
    }
  }

public : 
  
};


class StringGenerator : public StringOperator {
public:
  StringGenerator() : StringOperator() {
    color = NODE_COLOR::MAROON;
    SetNumAvailableInputs(0);
  };
  ~StringGenerator() {};
  virtual void Generate() = 0;

private:
};
class StringGenerate : public StringGenerator {
public:
  StringGenerate(): StringGenerator() {
    value = std::make_shared<Param<std::string>>("value", "Hello");

    m_ParamLayout.items = { {"Value!!", value}};
  };
  ~StringGenerate() {};

  void Generate() override {
    m_DataCache = value->Eval();
  }

  // YAML::Node YAMLSerialize() override {
  //   YAML::Node yaml_node;
  //   yaml_node["node"] = value->Eval();
  //   return yaml_node;
  // }

  std::shared_ptr<Param<std::string>> value;
public:


private:
};

class StringModifier : public StringOperator {
public:
  StringModifier() : StringOperator() { color = NODE_COLOR::DARK_GREEN; };
  ~StringModifier() {};
  virtual void Generate() = 0;

private:
};
class StringConcatenator : public StringModifier {
public:
  StringConcatenator() : StringModifier() { 
    SetNumAvailableInputs(2); 
    add_separator_param = std::make_shared<Param<bool>>("Add Separator", false);
    m_ParamLayout.items = { {"Separator", add_separator_param} };  
  }
  ~StringConcatenator() {};

  void Generate() override {
    if (GetInput(0) != nullptr && GetInput(1) != nullptr) {
      auto op0 = static_cast<StringOperator *>(GetInput(0).get());
      auto op1 = static_cast<StringOperator *>(GetInput(1).get());
      if(add_separator_param->Eval()){
        m_DataCache = op0->m_DataCache + " " + op1->m_DataCache;
      }else{

        m_DataCache = op0->m_DataCache + op1->m_DataCache;
      }
    }
  }

public:
  std::shared_ptr<Param<bool>> add_separator_param;
};

class StringConcatenatorMulti : public StringModifier {
public:
  StringConcatenatorMulti() : StringModifier() { 
    SetNumAvailableInputs(0);
    ActivateMultiInput(); 
    add_separator_param = std::make_shared<Param<bool>>("Add Separator", false);
    auto combo_test = std::make_shared<ParamComboBox>("Combo Test");
    combo_test->SetChoices({"A", "B", "C"});
    m_ParamLayout.items = { 
      {"Separator", add_separator_param}, 
      {"Combo", combo_test}  
    };
  }
  ~StringConcatenatorMulti() {};

  void Generate() override {
    m_DataCache = "";
    for(size_t i = 0; i < GetMultiInputCount(); i++){
      if (GetMultiInput(i) != nullptr) {
        auto op0 = static_cast<StringOperator *>(GetMultiInput(i).get());
        if(add_separator_param->Eval()){
          m_DataCache += " " + op0->m_DataCache;
        }else{

          m_DataCache += op0->m_DataCache;
        }
      }
    }
  }

public:
  std::shared_ptr<Param<bool>> add_separator_param;
};
class StringRepeater : public StringModifier {
public:
  StringRepeater():StringModifier() {
    SetNumAvailableInputs(1);
    count = std::make_shared<Param<uint32_t>>("Count", 10);
    m_ParamLayout.items = { 
      {"count", count},
      {"label", std::make_shared<ParamLabel>("Mega Label !")},
      {"sep", std::make_shared<ParamSeparator>("----------")},
      {"fake_position", std::make_shared<Param<glm::vec3>>("Fake", glm::vec3(0.0f))},
      {"fake_position2D", std::make_shared<Param<glm::vec2>>("Fake 2", glm::vec2(0.0f))}
      };
  };
  ~StringRepeater(){};

  void Generate() override {
    if (GetInput(0) != nullptr) {
      std::string val = "";
      auto op0 = static_cast<StringOperator *>(GetInput(0).get());

      for (uint32_t i = 0; i < count->Eval(); i++) {
        val += op0->m_DataCache;
      }
      m_DataCache = val;
    }
  }

public:
  std::shared_ptr<Param<uint32_t>> count;
};
class StringNull : public StringModifier {
public:
  StringNull() : StringModifier() {
    SetNumAvailableInputs(1);
    color = NODE_COLOR::ORANGE;
  };
  ~StringNull() {};

  void Generate() override {
    if (GetInput(0) != nullptr) {
      auto op0 = static_cast<StringOperator *>(GetInput(0).get());
      m_DataCache = op0->m_DataCache;
    }
  }
};

// class StringSubnet : public StringSubnetOperator {
// public:
//   StringSubnet() : StringSubnetOperator() {
//     // SetNumAvailableInputs(1);
//   };
//   ~StringSubnet() {};

//   void Update() override {
//     std::cout << "StringSubnet::Update" << std::endl;
//   }
//   void Generate() override {
//     std::cout << "Doing nothing .... " ;
//     std::cout << m_DataCache << std::endl;
//     // if (GetInput(0) != nullptr) {
//     //   // auto op0 = static_cast<StringOperator *>(GetInput(0).get());
//     //   // m_DataCache = "ok !!";

//     // }
//   }
// public:
//   
// }; 
};// namespace NodeEditor
#endif