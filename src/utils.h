#ifndef NODE_EDITOR_UTILS_H
#define NODE_EDITOR_UTILS_H

namespace NodeEditor {
    std::string generate_uuid();
    bool str_replace(std::string& str, const std::string& from, const std::string& to);
    bool str_replace_all(std::string& str, const std::string& from, const std::string& to);
};

#endif //NODE_EDITOR_UTILS_H