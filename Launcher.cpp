#include <cstring>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
#include <expected>

#include "Launcher.hpp"
#include "LoadLib.hpp"

constexpr auto WHITE_SPACE = " \t\n\r\f\v";

constexpr std::string trim(std::string_view view)
{
	auto start = view.find_first_not_of(WHITE_SPACE);

	if (start == std::string_view::npos)
		return {};

	auto end = view.find_last_not_of(WHITE_SPACE);
	return std::string(view.substr(start, end - start + 1));
}

int main(int argc, char *argv[])
{
	std::vector<std::string> command_line(argv + 1, argv + argc);

    //TODO: do i need to handle Window 8.3 file names as cmd input?
    //
	std::optional<std::string> first_script_line =
		(command_line.size() > 1 ? IsValidLuaFile(command_line[0]) :
					   std::nullopt);


    std::optional<std::filesystem::path> script_path;
	if (!first_script_line.has_value()) {
		script_path =
			FindInstalledScript();

		if (!script_path.has_value()) {
			std::cout
				<< "ERROR: Could not find a valid launcher lua file."
				<< std::endl;
			return 1;
		}

		first_script_line =
			IsValidLuaFile(script_path.value());
	}

	if (!first_script_line.has_value()) {
		std::cout << "ERROR: Could not find a valid launcher lua file."
			  << std::endl;
		return 1;
	}

    command_line.insert(command_line.begin(),script_path.value().string());

	std::string lib_name(first_script_line.value().begin() + 2,
			     first_script_line.value().end());

	lib_name = trim(lib_name);
	if (!lib_name.ends_with(".dll"))
		lib_name.append(".dll");






    auto simple_graphics = SimpleGraphics::create(lib_name);

	if (!simple_graphics) {
        std::cout << "ERROR: DLL "<< lib_name <<" does not appear to be a Path of Building dll." << std::endl;;
		return 1;
	}


    // TODO: Handle UTF-8 ?

    size_t buffer_size = 0;

    for (auto const& s : command_line)
	    buffer_size += s.size() + 1;

    std::cout << "buffer_size: " << buffer_size << std::endl;

    std::vector<char> storage(buffer_size);
    std::vector<char*> lua_argv(command_line.size() + 1);

    std::cout << "command_line.size(): " << command_line.size()<< std::endl;
    char* p = storage.data();
    for (size_t i = 0; i < command_line.size(); i++) {
	    std::cout << "LOOP i: " << i << std::endl;
	    lua_argv[i] = p;
	    std::cout << "p: " << p << "lua_argv[i]:" << lua_argv[i] << std::endl;
        std::memcpy(p,command_line[i].data(),command_line[i].size());
    std::cout << "DEBUG 2" << std::endl;
        p += command_line[i].size();

    std::cout << "DEBUG 3" << std::endl;
        *p++ = '\0';
    }


    lua_argv.back() = nullptr;

	// Call into the DLL
	int dwStatus = simple_graphics.value()(command_line.size(), (char**)lua_argv.data());

	return dwStatus;
}
