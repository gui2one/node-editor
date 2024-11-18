#ifndef NODE_EDITOR_PARAMFACTORY_H
#define NODE_EDITOR_PARAMFACTORY_H
#pragma once

#include <unordered_map>

#include "NodeParam.h"

namespace NED {
struct ParamFactoryRegistryItem {
  std::string type_name;
  std::function<std::shared_ptr<NodeParam>()> factory_func;
};
class ParamFactoryRegistry {
 public:
  static ParamFactoryRegistry& instance();
  inline void registerType(const std::string& typeName, ParamFactoryRegistryItem item) { factories[typeName] = item; }
  std::shared_ptr<NodeParam> create(const std::string& typeName, const char * label = "no label yet") const;

  template<typename T>
  std::shared_ptr<T> create2(std::string typeName, const char * label) { return std::dynamic_pointer_cast<T>(create(typeName, label)); }

  inline std::unordered_map<std::string, ParamFactoryRegistryItem> getFactories() const { return factories; }

 private:
  std::unordered_map<std::string, ParamFactoryRegistryItem> factories;
};
}  // namespace NED

#define REGISTER_PARAM_TYPE(Type)                                                                     \
  std::cout << "Registering " << #Type << std::endl;                                                  \
  NED::ParamFactoryRegistry::instance().registerType(#Type, {#Type, []() -> std::shared_ptr<Type> {   \
                                                               auto param = std::make_shared<Type>(); \
                                                               param->m_TypeName = #Type;             \
                                                               return param;                          \
                                                             }})

#define CREATE_PARAM(Type, Label) \
  NED::ParamFactoryRegistry::instance().create2<Type>(std::string(#Type), Label)
#endif