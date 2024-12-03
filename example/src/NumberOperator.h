#ifndef EXAMPLE_NUMBEROPERATOR_H
#define EXAMPLE_NUMBEROPERATOR_H
#pragma once
#include "ImGuiNode.h"
#include "NodeParam.h"
#include "params.h"

namespace NED {
class NumberOperator : public ImGuiNode<float> {
 public:
  NumberOperator() : ImGuiNode("default Number Op") {}
  virtual ~NumberOperator() = default;

  void ClearCache() override { m_DataCache = 0.0f; }
};

class NumberConstant : public NumberOperator {
 public:
  NumberConstant() : NumberOperator() {
    SetNumAvailableInputs(0);

    value = CREATE_PARAM(NED::Param<float>, "Value", this);
    value->Set(1.0f);
    m_ParamLayout.params = {value};
  }
  ~NumberConstant() {}
  void Generate() override { m_DataCache = value->Eval(); }

 public:
  std::shared_ptr<NED::Param<float>> value;
};

class Add : public NumberOperator {
 public:
  Add() : NumberOperator() { SetNumAvailableInputs(2); }
  ~Add() {}
  void Generate() override {
    if (GetInput(0) != nullptr && GetInput(1) != nullptr) {
      auto op0 = dynamic_cast<NumberOperator*>(GetInput(0));
      auto op1 = dynamic_cast<NumberOperator*>(GetInput(1));
      m_DataCache = op0->m_DataCache + op1->m_DataCache;
    } else {
      m_DataCache = 0.0f;
    }
  }
};
};  // namespace NED
#endif