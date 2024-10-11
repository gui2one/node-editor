# another imGui Node Editor ... 

Frankly, this is born only from my inability to build [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)
If you are looking for a robust node editor, you should look at [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)

This project is completely dependent on ImGui and GLFW.

Trying to be agnostic regarding the type of data on the Editor side, make the user implementation relatively "involved". You basically have to implement an Application class, some Serialization code too ... .( see [example dir](/example)) I will try to make it easier

For now , there is no clever 'network evaluation'. Just a recursive discovery of input nodes, starting from **NodesManager::m_OutputNode** ( the on signaled with a red circle)

![screenshot](github_resources/node_editor_capture.jpg)




An example implementation would look like that :
```cpp
class StringOperator : public ImGuiNode {
public:
  StringOperator() : ImGuiNode("default") {};
  virtual ~StringOperator() = default;
  virtual void Generate() = 0;

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
    m_StringCache = get_param_value<std::string>(value.get());
  }
