#ifndef NODE_EDITOR_UTILS_H
#define NODE_EDITOR_UTILS_H
#pragma once
namespace NodeEditor {
    std::string generate_uuid();

    bool str_replace(std::string& str, const std::string& from, const std::string& to);
    bool str_replace_all(std::string& str, const std::string& from, const std::string& to);
    bool str_replace_last(std::string& str, const std::string& from, const std::string& to);
    bool str_remove(std::string& str, const std::string& target);
    bool str_remove_all(std::string& str, const std::string& target);
    bool str_remove_last(std::string& str, const std::string& target);
};

#endif //NODE_EDITOR_UTILS_H