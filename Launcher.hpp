#pragma once


#include <filesystem>
#include <optional>
#include <string>

std::optional<std::string> IsValidLuaFile(const std::filesystem::path &path);


std::optional<std::filesystem::path> FindInstalledScript();



