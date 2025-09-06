cmake_minimum_required(VERSION 3.30)
project(pob2 VERSION 0.0.1 LANGUAGES CXX)


include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)




add_executable(pob2 
    ./Launcher.cpp
    ./LauncherScriptValidation.cpp
    ./LoadLib.cpp
    ./FindInstalledScript.cpp
    )


install(TARGETS pob2 DESTINATION CMAKE_INSTALL_BINDIR)


