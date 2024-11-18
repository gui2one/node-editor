#ifndef NODE_EDITOR_UTILS_H
#define NODE_EDITOR_UTILS_H
#pragma once

#include <codecvt>
#include <locale>
#include <string>
#include <unordered_set>
#include <sstream>

namespace NED {
std::string generate_unique_name(const std::string& baseName, const std::unordered_set<std::string>& existingNames);
std::string generate_uuid();

bool str_replace(std::string& str, const std::string& from, const std::string& to);
bool str_replace_all(std::string& str, const std::string& from, const std::string& to);
bool str_replace_last(std::string& str, const std::string& from, const std::string& to);
bool str_remove(std::string& str, const std::string& target);
bool str_remove_all(std::string& str, const std::string& target);
bool str_remove_last(std::string& str, const std::string& target);

std::string wide_to_utf8(const std::wstring& wide_utf16_source_string);
std::wstring utf8_to_wide(const std::string& utf8_source_string);

};  // namespace NED

#endif  // NODE_EDITOR_UTILS_H