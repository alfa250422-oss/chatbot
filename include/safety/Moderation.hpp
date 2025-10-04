#pragma once
#include <string>
struct ModVerdict{ bool flagged=false; std::string reason; };
class Moderation{
public:
  ModVerdict checkText(const std::string& text, const std::string& api_key, const std::string& model="omni-moderation-latest");
};