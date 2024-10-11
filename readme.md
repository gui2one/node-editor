# another imGui Node Editor ... 

Frankly, this is born only from my inability to build [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)
If you are looking for a robust node editor, you should look at [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)

This project is completely dependent on ImGui and GLFW.

Trying to be agnostic regarding the type of data on the Editor side, make the user implementation relatively "involved". You basically have to implement an Application class.( see [example dir](/example))

![screenshot](github_resources/node_editor_capture.jpg)