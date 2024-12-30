#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#include <iostream>
#include <string>
#include <type_traits>

#include "ImGuiNode.h"
#include "NodeParam.h"
#include "ParamFactory.h"
#include "params.h"

namespace NED {

class StringOperator : public ImGuiNode<std::string> {
 public:
  StringOperator() : ImGuiNode("default") {};
  virtual ~StringOperator() = default;

  void ClearCache() override { m_DataCache = ""; }
};

class StringGenerator : public StringOperator {
 public:
  StringGenerator() : StringOperator() {
    color = NODE_COLOR::YELLOW;
    SetNumAvailableInputs(0);
  };
  ~StringGenerator() {};
  virtual void Generate() = 0;

 private:
};
class StringGenerate : public StringGenerator {
 public:
  StringGenerate() : StringGenerator() {
    value = CREATE_PARAM(NED::ParamString, "String Value", this);
    value->Set("Hello ???");

    auto color_p = CREATE_PARAM(NED::ParamColor, "Color", this);
    color_p->Set(glm::vec3(1.0f));

    auto label = CREATE_PARAM(NED::ParamLabel, "label1", this);
    label->Set("Is this the label ?");

    auto combo_p = CREATE_PARAM(NED::ParamComboBox, "Combo", this);
    combo_p->SetChoices({"one", "two", "three", "four", "five"});
    combo_p->Set(0);

    auto float_p = CREATE_PARAM(NED::ParamFloat, "Float", this);
    float_p->Set(3.14f, 0.0f, 999.9f);

    auto int_p = CREATE_PARAM(NED::ParamInt, "Int", this);
    int_p->Set(42, 0, 100);

    auto coords = CREATE_PARAM(NED::ParamVec2, "Vec2", this);
    coords->Set(glm::vec2(0.0f));
    coords->default_val = glm::vec2(0.0f);

    auto position_p = CREATE_PARAM(NED::ParamVec3, "Vec3", this);
    position_p->Set(glm::vec3(1.0f));
    position_p->default_val = glm::vec3(1.0f);

    auto double_p = CREATE_PARAM(NED::ParamDouble, "Double", this);
    double_p->Set(3.14, -5.0, 5.0);

    auto bool_p = CREATE_PARAM(NED::ParamBool, "Bool", this);
    bool_p->Set(true);

    auto grp = CREATE_PARAM(NED::ParamGroup, "GroupTest", this);
    grp->params = {label, color_p, combo_p, int_p, coords, position_p, double_p, bool_p};

    m_ParamLayout.Append({value, float_p, grp});
  };
  ~StringGenerate() {};

  void Generate() override { m_DataCache = value->Eval(); }

  std::shared_ptr<ParamString> value;

 public:
 private:
};

class TextFileLoader : public StringGenerator {
 public:
  TextFileLoader() : StringGenerator() {
    file_path_param = CREATE_PARAM(NED::ParamFile, "File Path", this);
    m_ParamLayout.params = {file_path_param};
  }
  ~TextFileLoader() {};

  void Generate() override {
    std::ifstream file;
    file.open(std::filesystem::path(file_path_param->Eval()), std::ios::in);
    if (file.is_open()) {
      std::stringstream buffer;
      buffer << file.rdbuf();
      std::string content = buffer.str();
      file.close();
      m_DataCache = content;
    } else {
      std::cerr << "Unable to open file" << std::endl;
      m_DataCache = "NO DATA";
    }
  }

 public:
  std::shared_ptr<ParamFile> file_path_param;
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
    // add_separator_param = std::make_shared<ParamBool>("Add Separator", false);
    add_separator_param = CREATE_PARAM(NED::ParamBool, "Add Separator", this);
    add_separator_param->Set(false);
    m_ParamLayout.params = {add_separator_param};
  }
  ~StringConcatenator() {};

  void Generate() override {
    if (GetInput(0) != nullptr && GetInput(1) != nullptr) {
      auto op0 = static_cast<StringOperator*>(GetInput(0));
      auto op1 = static_cast<StringOperator*>(GetInput(1));
      if (add_separator_param->Eval()) {
        m_DataCache = op0->m_DataCache + " " + op1->m_DataCache;
      } else {
        m_DataCache = op0->m_DataCache + op1->m_DataCache;
      }
    }
  }

 public:
  std::shared_ptr<ParamBool> add_separator_param;
};

class StringConcatenatorMulti : public StringModifier {
 public:
  StringConcatenatorMulti() : StringModifier() {
    SetNumAvailableInputs(0);
    ActivateMultiInput();
    // add_separator_param = std::make_shared<ParamBool>("Add Separator", false);
    add_separator_param = CREATE_PARAM(NED::ParamBool, "Add Separator", this);
    add_separator_param->Set(false);
    // auto combo_test = std::make_shared<ParamComboBox>("Combo Test");
    auto combo_test = CREATE_PARAM(NED::ParamComboBox, "Combo Test", this);
    combo_test->SetChoices({"A", "B", "C"});
    m_ParamLayout.params = {add_separator_param, combo_test};
  }
  ~StringConcatenatorMulti() {};

  void Generate() override {
    m_DataCache = "";
    for (size_t i = 0; i < GetMultiInputCount(); i++) {
      if (GetMultiInput(i) != nullptr) {
        auto op0 = static_cast<StringOperator*>(GetMultiInput(i));
        if (add_separator_param->Eval()) {
          m_DataCache += " " + op0->m_DataCache;
        } else {
          m_DataCache += op0->m_DataCache;
        }
      }
    }
  }

 public:
  std::shared_ptr<ParamBool> add_separator_param;
};
class StringRepeater : public StringModifier {
 public:
  StringRepeater() : StringModifier() {
    SetNumAvailableInputs(1);
    // count = std::make_shared<ParamInt>("Count", 10);
    count = CREATE_PARAM(NED::ParamInt, "Count", this);
    count->Set(10);
    count->min_val = 0;
    count->max_val = 100;
    m_ParamLayout.params = {count};
  };
  ~StringRepeater() {};

  void Generate() override {
    if (GetInput(0) != nullptr) {
      std::string val = "";
      auto op0 = static_cast<StringOperator*>(GetInput(0));

      int num_copies = count->Eval();
      if (num_copies < 0) return;

      for (int i = 0; i < num_copies; i++) {
        val += op0->m_DataCache;
      }
      m_DataCache = val;
    }
  }

 public:
  std::shared_ptr<ParamInt> count;
};

class StringToUpperCase : public StringModifier {
 public:
  StringToUpperCase() : StringModifier() {
    SetNumAvailableInputs(1);
    icon_name = "to_upper";
  }

  void Generate() override {
    auto op0 = static_cast<StringOperator*>(GetInput(0));
    if (op0 == nullptr) return;
    std::string src = op0->m_DataCache;
    std::transform(src.begin(), src.end(), src.begin(), ::toupper);
    m_DataCache = src;
  }
};

};  // namespace NED
#endif