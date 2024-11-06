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

class StringSubnetOperator : public SubnetNode<std::string> {
public:
  StringSubnetOperator() : SubnetNode() {};
  virtual ~StringSubnetOperator() = default;

  void Generate() override {
    if(node_network.outuput_node != nullptr){
      auto op = std::dynamic_pointer_cast<StringOperator>(node_network.outuput_node);
      if(op != nullptr){
        m_DataCache = op->m_DataCache;
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
    auto sep = std::make_shared<ParamSeparator>("");
    auto coords = std::make_shared<Param<glm::vec2>>("coords", glm::vec2(0.0f));
    auto position = std::make_shared<Param<glm::vec3>>("position", glm::vec3(0.0f));
    m_ParamLayout.items = {
      {"", value}, 
      {"", sep},
      {"", coords},
      {"", position}
    };
  };
  ~StringGenerate() {};

  void Generate() override {
    m_DataCache = value->Eval();
  }

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
    count = std::make_shared<Param<int>>("Count", 10);
    count->min_val = 0;
    count->max_val = 100;
    m_ParamLayout.items = { 
      {"count", count}};
  };
  ~StringRepeater(){};

  void Generate() override {
    if (GetInput(0) != nullptr) {
      std::string val = "";
      auto op0 = static_cast<StringOperator *>(GetInput(0).get());

      int num_copies = count->Eval();
      if(num_copies < 0) return;
      
      for (int i = 0; i < num_copies; i++) {
        val += op0->m_DataCache;
      }
      m_DataCache = val;
    }
  }

public:
  std::shared_ptr<Param<int>> count;
};

class StringToUpperCase : public StringModifier {
public:
  StringToUpperCase():StringModifier(){
    SetNumAvailableInputs(1);
    icon_name = "to_upper";
  }

  void Generate() override{
    auto op0 = static_cast<StringOperator *>(GetInput(0).get());
    if(op0 == nullptr) return;
    std::string src = op0->m_DataCache;
    std::transform(src.begin(), src.end(), src.begin(), ::toupper);
    m_DataCache = src;
  }
};

class StringNull : public StringModifier {
public:
  StringNull() : StringModifier() {
    SetNumAvailableInputs(1);
    color = NODE_COLOR::ORANGE;
    icon_name = "empty_set";
  };
  ~StringNull() {};

  void Generate() override {
    if (GetInput(0) != nullptr) {
      auto op0 = static_cast<StringOperator *>(GetInput(0).get());
      m_DataCache = op0->m_DataCache;
    }
  }
};

};// namespace NodeEditor
#endif