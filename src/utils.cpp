#include "utils.h"
namespace NED {
std::string generate_unique_name(const std::string &baseName, const std::unordered_set<std::string> &existingNames) {
  // Strip trailing digits from the base name.
  std::regex stripDigitsRegex("^(.*?)(\\d*)$");
  std::smatch match;

  std::string strippedBaseName = baseName;
  if (std::regex_match(baseName, match, stripDigitsRegex)) {
    strippedBaseName = match[1].str();  // Capture the part without the trailing digits.
  }

  // Start with the stripped base name.
  if (existingNames.find(strippedBaseName) == existingNames.end()) {
    return strippedBaseName;
  }

  // Regex to extract numeric suffix from names matching the stripped base name.
  std::regex suffixRegex("^" + strippedBaseName + "(\\d+)$");
  int maxSuffix = 1;  // Start with 1 to handle "strippedBaseName2".

  // Iterate through existing names to find the maximum suffix for the stripped base name.
  for (const auto &name : existingNames) {
    if (std::regex_match(name, match, suffixRegex)) {
      int currentSuffix = std::stoi(match[1].str());
      maxSuffix = std::max(maxSuffix, currentSuffix);
    }
  }

  // Generate a new name with the next suffix.
  std::ostringstream ss;
  ss << strippedBaseName << (maxSuffix + 1);
  return ss.str();
}

std::string generate_uuid() {
  static std::random_device dev;
  static std::mt19937 rng(dev());

  std::uniform_int_distribution<int> dist(0, 15);

  const char *v = "0123456789abcdef";
  const bool dash[] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

  std::string res;
  for (int i = 0; i < 16; i++) {
    if (dash[i]) res += "-";
    res += v[dist(rng)];
    res += v[dist(rng)];
  }
  return res;
}


bool str_replace(std::string &str, const std::string &from, const std::string &to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos) return false;
  str.replace(start_pos, from.length(), to);
  return true;
}
bool str_replace_all(std::string &str, const std::string &from, const std::string &to) {
  bool replaced = false;
  while (str.find(from) != std::string::npos) {
    replaced = str_replace(str, from, to);
  }

  return replaced;
}

bool str_replace_last(std::string &str, const std::string &from, const std::string &to) {
  size_t pos = str.find_last_of(from);
  if (pos == std::string::npos) return false;
  str.replace(pos, from.length(), to);
  return true;
}
bool str_remove(std::string &str, const std::string &target) { return str_replace(str, target, ""); }
bool str_remove_all(std::string &str, const std::string &target) { return str_replace_all(str, target, ""); }
bool str_remove_last(std::string &str, const std::string &target) { return str_replace_last(str, target, ""); }

std::string wide_to_utf8(const std::wstring &wide_utf16_source_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::string narrow = converter.to_bytes(wide_utf16_source_string);
  return narrow;
}
std::wstring utf8_to_wide(const std::string &utf8_source_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(utf8_source_string);
  return wide;
}
};  // namespace NED