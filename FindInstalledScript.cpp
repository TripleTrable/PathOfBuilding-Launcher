
#include <filesystem>
#include <optional>
#include <string>
#include <limits.h>
#include "Launcher.hpp"

#ifdef __linux__
#include <unistd.h>
#else
#include <ShlObj_core.h>
#include <Windows.h>
#endif
std::optional<std::filesystem::path> executalbe_base_path()
{
#ifdef __linux__
	char buf[PATH_MAX];
	ssize_t length = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
	if (length == -1) {
		return std::nullopt;
	}
	buf[length] = '\0';
	return std::filesystem::path(buf).parent_path();

#else
	wchar_t buf[MAX_PATH]{};
	if (GetModuleFileNameW(nullptr, buf, MAX_PATH))
		return std::filesystem::path(buf).parent_path();
	return std::nullopt;

#endif //  __linux__
}

std::optional<std::filesystem::path>
search_launch_script(std::filesystem::path &base_path)
{
	if (IsValidLuaFile(base_path / "Launch.lua").has_value())
		return base_path / "Launch.lua";

	if (IsValidLuaFile(base_path / "src/Launch.lua").has_value())
		return base_path / "src/Launch.lua";

	if (base_path.string().ends_with("runtime"))
		if (IsValidLuaFile(base_path.parent_path() / "src/Launch.lua")
			    .has_value())
			return base_path.parent_path() / "src/Launch.lua";

	return std::nullopt;
}

std::optional<std::filesystem::path> FindInstalledScript()
{
	// Search in up to 4 locations:
	// 1. relative to the launcher binary
	// 2. search Windows registry
	// 3. search %AppData%
	// 4. search %Programfiles%


	// Search locally relative to the launcher binary
	auto base_path = executalbe_base_path();
	if (base_path.has_value())
		return search_launch_script(base_path.value());

#ifdef __linux__
	return std::nullopt;
#else

#ifdef GAMEVERSION_2
	constexpr const wchar_t *sub_key =
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Path of Building Community (PoE2)";
	const std::filesystem::path install_dir("Path of Building Community (PoE2)");
#else
	constexpr const wchar_t *sub_key =
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Path of Building Community";
	const std::filesystem::path install_dir("Path of Building Community");
#endif
	// search registry
	{


		DWORD type = 0;
		DWORD size_bytes = 0;

		LSTATUS status = RegGetValueW(
			HKEY_CURRENT_USER, sub_key, L"InstallLocation",
			RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, &type, nullptr,
			&size_bytes);

		if (status != ERROR_SUCCESS)
			return std::nullopt;

		std::wstring buf(size_bytes / sizeof(wchar_t), L'\0');
		status = RegGetValueW(HKEY_CURRENT_USER, sub_key,
				      L"InstallLocation",
				      RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
				      &type, buf.data(), &size_bytes);

		if (status != ERROR_SUCCESS)
			return std::nullopt;

		if (buf.back() == L'\0')
			buf.resize(size_bytes / sizeof(wchar_t) - 1);

		if (buf.front() == L'"' && buf.back() == L'"')
			buf = buf.substr(1, buf.size() - 2);

		std::filesystem::path regex_path(buf);

		auto script = search_launch_script(regex_path);
		if (script.has_value())
			return script;
	}

	//Search AppData
	{
		wchar_t *raw = nullptr;
		const HRESULT result =
			SHGetKnownFolderPath(FOLDERID_RoamingAppData,
					     KF_FLAG_DEFAULT, nullptr, &raw);

			if (FAILED(result) || !raw)
				return std::nullopt;

		std::filesystem::path base_path(raw);
		CoTaskMemFree(raw);
		auto script = search_launch_script(base_path);
		if (script.has_value())
			return script;

	}

	#ifndef GAMEVERSION_2

	//Search ProgramData
	{
		wchar_t *raw = nullptr;
		const HRESULT result =
			SHGetKnownFolderPath(FOLDERID_ProgramData,
					     KF_FLAG_DEFAULT, nullptr, &raw);

		if (FAILED(result) || !raw)
			return std::nullopt;

		std::filesystem::path base_path(raw);
		CoTaskMemFree(raw);
		auto script = search_launch_script(base_path);
		if (script.has_value())
			return script;
	}
	#endif

#endif
}
