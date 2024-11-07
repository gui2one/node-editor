#ifndef NODE_EDITOR_UTILS_H
#define NODE_EDITOR_UTILS_H
#pragma once

#include <locale>
#include <codecvt>
#include <string>

namespace NodeEditor {
    std::string generate_uuid();

    bool str_replace(std::string& str, const std::string& from, const std::string& to);
    bool str_replace_all(std::string& str, const std::string& from, const std::string& to);
    bool str_replace_last(std::string& str, const std::string& from, const std::string& to);
    bool str_remove(std::string& str, const std::string& target);
    bool str_remove_all(std::string& str, const std::string& target);
    bool str_remove_last(std::string& str, const std::string& target);


    std::string wide_to_utf8(const std::wstring& wide_utf16_source_string);
    std::wstring utf8_to_wide(const std::string& utf8_source_string);
    
};

#endif //NODE_EDITOR_UTILS_H