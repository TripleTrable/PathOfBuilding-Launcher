
#include "LoadLib.hpp"
#include <expected>
#include <memory>
#include <utility>

SimpleGraphics::~SimpleGraphics()
{
#ifdef __linux__
    if (lib_handle) {
        dlclose(lib_handle);
        lib_handle = nullptr;
    }
#else
    if (lib_handle) {
        FreeLibrary(lib_handle);
        lib_handle = nullptr;
    }

#endif // __linux__
    func_handle = nullptr;
}

SimpleGraphics::SimpleGraphics(SimpleGraphics && o) noexcept
: lib_handle(std::exchange(o.lib_handle, nullptr)),
    func_handle(std::exchange(o.func_handle, nullptr)) { }

SimpleGraphics& SimpleGraphics::operator=(SimpleGraphics && o) noexcept
{
    if(this != &o)
    {
        lib_handle = std::exchange(o.lib_handle, nullptr);
        func_handle = std::exchange(o.func_handle, nullptr);
    }
    return *this;
}

std::expected<SimpleGraphics, SimpleGraphics::Status>
SimpleGraphics::create(std::string &path)
{
    #ifdef __linux__

    lib_handle_t h = dlopen(path.c_str(), RTLD_LAZY | RTLD_DEEPBIND);

    if(!h) return std::unexpected(Status::LIB_NOT_LOAD);

    void* f = dlsym(h, "RunLuaFileAsWin");

    if (f) 
        return SimpleGraphics(h,reinterpret_cast<func_handle_t>(f));

    f = dlsym(h, "RunLuaFileAsConsole");

    if (f) 
        return SimpleGraphics(h,reinterpret_cast<func_handle_t>(f));

    dlclose(h);
    return std::unexpected(Status::FUNC_NOT_LOAD);

#else
	
    lib_handle_t h = LoadLibrary(path.c_str());

    if (!h) return std::unexpected(Status::LIB_NOT_LOAD);

    FARPROC f = GetProcAddress(h, "RunLuaFileAsWin");
    if (f)
	    return SimpleGraphics(h, reinterpret_cast<func_handle_t>(f));

    f = GetProcAddress(h, "RunLuaFileAsWin");
    if (f)
	    return SimpleGraphics(h, reinterpret_cast<func_handle_t>(f));

    FreeLibrary(h);
    return std::unexpected(Status::FUNC_NOT_LOAD);
    #endif // __linux__
}

