#ifndef PARAMS_H
#define PARAMS_H
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include "NodeParam.h"

namespace NodeEditor {
template<>
class Param<glm::vec3> : public NodeParam {
public:
  Param(const char * _name, glm::vec3 _value) : NodeParam(_name), value(_value) {}

  ~Param() {}

  glm::vec3 Eval() {
    return value;
  }

  void Display() {
    ImGui::ColorEdit3(name, glm::value_ptr(value));
  }

public:
  glm::vec3 value;
};

};

#endif