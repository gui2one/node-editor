#include "utils.h"
namespace NodeEditor {
std::string generate_uuid() {
    static std::random_device dev;
    static std::mt19937 rng(dev());

    std::uniform_int_distribution<int> dist(0, 15);

    const char *v = "0123456789abcdef";
    const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

    std::string res;
    for (int i = 0; i < 16; i++) {
        if (dash[i]) res += "-";
        res += v[dist(rng)];
        res += v[dist(rng)];
    }
    return res;
}

std::string clean_node_type_name(const char * dirty_type) { 
    std::string clean = std::string(dirty_type); 
    str_replace_all(clean, "class ", "");
    str_replace_all(clean, " >", ">");
    str_replace(clean, "NodeEditor::Node<", "");
    str_replace_last(clean, ">", "");
    str_replace(clean, "std::basic_string<char,struct std::char_traits<char>,std::allocator<char>>", "std::string");    
    return clean;     
}

bool str_replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
bool str_replace_all(std::string &str, const std::string &from, const std::string &to)
{
    bool replaced = false;
    while(str.find(from) != std::string::npos){
        replaced = str_replace(str, from, to);
    }

    return replaced;

}

bool str_replace_last(std::string &str, const std::string &from, const std::string &to){
    size_t pos = str.find_last_of(from);
    if(pos == std::string::npos)
        return false;
    str.replace(pos, from.length(), to);
    return true;
}
bool str_remove(std::string &str, const std::string &target)
{
    return str_replace(str, target, "");
}
bool str_remove_all(std::string &str, const std::string &target)
{
    return str_replace_all(str, target, "");
}
bool str_remove_last(std::string &str, const std::string &target)
{
    return str_replace_last(str, target, "");
}

std::string wide_to_utf8(const std::wstring &wide_utf16_source_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string narrow = converter.to_bytes(wide_utf16_source_string);
    // std::wstring wide = converter.from_bytes(narrow_utf8_source_string);
    return narrow;
}
std::wstring utf8_to_wide(const std::string &utf8_source_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(utf8_source_string);
    return wide;
}
};