#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H
#pragma once

#include "ImguiNode.h"
#include "NodeParam.h"
#include "params.h"
#include <iostream>
#include <string>
#include <type_traits>

namespace NodeEditor {

class StringOperator : public ImGuiNode {
public:
  StringOperator() : ImGuiNode("default") {};
  virtual ~StringOperator() = default;
  // virtual void Generate() = 0;

public:
  std::string m_StringCache = "";
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
    m_Params.push_back(value);
  };
  ~StringGenerate() {};

  void Generate() override {
    // auto param_0 = static_cast<Param<std::string> *>(m_Params[0].get());
    m_StringCache = get_param_value<std::string>(m_Params[0]);

  }

public:
  std::shared_ptr<Param<std::string>> value;

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
  StringConcatenator() : StringModifier() { SetNumAvailableInputs(2); }
  ~StringConcatenator() {};

  void Generate() override {
    if (GetInput(0) != nullptr && GetInput(1) != nullptr) {
      auto op0 = static_cast<StringOperator *>(GetInput(0).get());
      auto op1 = static_cast<StringOperator *>(GetInput(1).get());
      m_StringCache = op0->m_StringCache + op1->m_StringCache;
    }
  }

public:
};
class StringRepeater : public StringModifier {
public:
  StringRepeater():StringModifier() {
    SetNumAvailableInputs(1);
    auto param = std::make_shared<Param<uint32_t>>("Count", 10);
    m_Params.push_back(param);
  };
  ~StringRepeater(){};

  void Generate() override {
    if (GetInput(0) != nullptr) {
      std::string val = "";
      auto op0 = static_cast<StringOperator *>(GetInput(0).get());
      // auto param_0 = static_cast<Param<uint32_t> *>(m_Params[0].get());
      auto val_0 = get_param_value<uint32_t>(m_Params[0]);
      for (uint32_t i = 0; i < m_Count; i++) {
        val += op0->m_StringCache;
      }
      m_StringCache = val;
    }
  }

public:
  uint32_t m_Count = 2;
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
      m_StringCache = op0->m_StringCache;
    }
  }
};

}; // namespace NodeEditor

#endif