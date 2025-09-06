
#include <filesystem>

#include <expected>

#ifdef __linux__
#include <dlfcn.h>
using lib_handle_t = void*;
#else
#include <windows.h>
using lib_handle_t = HMODULE;
#endif // __linux__

typedef int (*func_handle_t)(int, char **);

class SimpleGraphics {
public:
    enum class Status { OK, LIB_NOT_LOAD, FUNC_NOT_LOAD };

    // delete copy constructors
    SimpleGraphics(const SimpleGraphics&)            = delete;
    SimpleGraphics& operator=(const SimpleGraphics&) = delete;


    // delete copy constructors
    SimpleGraphics(SimpleGraphics&&) noexcept;
    SimpleGraphics& operator=(SimpleGraphics&&) noexcept;

    ~SimpleGraphics();

    static std::expected<SimpleGraphics, Status> create(std::string&);

    int operator()(int argc, char** argv) const {
        return func_handle ? func_handle(argc,argv) : -1;
    }

private:

    SimpleGraphics(lib_handle_t h, func_handle_t f) : lib_handle(h), func_handle(f) {}

    lib_handle_t lib_handle{nullptr};
    func_handle_t func_handle{nullptr};
    
};

