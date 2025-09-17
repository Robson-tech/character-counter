# Servidor Números - Contador de Dígitos

## CMakeLists.txt para construir o servidor escravo de contagem de números
cmake_minimum_required(VERSION 3.16)
project(servidor_numeros)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar dependências
find_package(PkgConfig REQUIRED)

# HttpLib
find_path(HTTPLIB_INCLUDE_DIR httplib.h)
if(NOT HTTPLIB_INCLUDE_DIR)
    message(STATUS "httplib não encontrado, baixando...")
    file(DOWNLOAD 
        "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/httplib.h")
    set(HTTPLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# nlohmann/json
find_path(JSON_INCLUDE_DIR nlohmann/json.hpp)
if(NOT JSON_INCLUDE_DIR)
    message(STATUS "nlohmann/json não encontrado, baixando...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann/json.hpp")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann")
    set(JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Executable
add_executable(servidor_numeros main.cpp)

# Include directories
target_include_directories(servidor_numeros PRIVATE 
    ${HTTPLIB_INCLUDE_DIR}
    ${JSON_INCLUDE_DIR}
)

# Threads
find_package(Threads REQUIRED)
target_link_libraries(servidor_numeros Threads::Threads)

# Windows específico
if(WIN32)
    target_link_libraries(servidor_numeros ws2_32 wsock32)
endif()