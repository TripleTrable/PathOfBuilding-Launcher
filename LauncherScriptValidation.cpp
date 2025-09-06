#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

constexpr std::string_view UTF8_BOM = "\xEF\xBB\xBF";

using namespace std::literals;

std::optional<std::string> IsValidLuaFile(const std::filesystem::path &path)
{


    if (!std::filesystem::exists(path))
        return std::nullopt;

    std::ifstream script(path,std::ios::in);

    if (!script.is_open())
        return std::nullopt;
    
    
    std::string line;
    if(!std::getline(script,line))
        return std::nullopt;

    // On windows getline should strip it but opening a CRLF file on POSIX still
    // needs it
    if(!line.empty() && line.back() == '\r')
        line.pop_back();

    // Remove the UTF8-BOM if neccessary
    if (line.starts_with(UTF8_BOM))
        line.erase(0,UTF8_BOM.size());

    // Test for magic header 
    if(!line.starts_with("#@"sv))
        return std::nullopt;

	return line;
}
