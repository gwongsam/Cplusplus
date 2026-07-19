# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.12...3.30)

# Set c++ standard for project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Feeds compile_commands.json to the VS Code C/C++ extension.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  ModernCMakeExample
  VERSION 1.0
  LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Third-party dependencies
# ---------------------------------------------------------------------------
# On macOS the deps normally come from Homebrew, which installs keg-only
# packages outside the default search prefixes.
if(APPLE)
  find_program(BREW_EXECUTABLE brew)
  if(BREW_EXECUTABLE)
    foreach(_keg openssl@3 lua asio)
      execute_process(
        COMMAND ${BREW_EXECUTABLE} --prefix ${_keg}
        OUTPUT_VARIABLE _keg_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
      if(_keg_prefix AND EXISTS ${_keg_prefix})
        list(APPEND CMAKE_PREFIX_PATH ${_keg_prefix})
      endif()
    endforeach()
  endif()
endif()

# OpenSSL: base64 encode/decode in public/coding.hpp
find_package(OpenSSL REQUIRED)

# Lua: embedded interpreter in public/coding.hpp. Homebrew ships no CMake
# config for Lua, and CMake's bundled FindLua does not know about 5.5 yet, so
# locate the headers and library by hand. The headers live in a "lua"
# subdirectory, which coding.hpp does not spell out, so add it directly.
find_path(
  LUA_INCLUDE_DIR lua.h
  PATH_SUFFIXES lua lua5.5 lua5.4 lua54 lua5.3
  REQUIRED)
find_library(
  LUA_LIBRARY
  NAMES lua lua5.5 lua5.4 lua54 lua5.3
  REQUIRED)
message(STATUS "Using Lua: ${LUA_LIBRARY} (headers: ${LUA_INCLUDE_DIR})")

# Asio (standalone, header-only): server/asio_server.hpp. Optional — nothing in
# the default build needs it, so a missing Asio only disables the asio_server
# target instead of breaking the whole configure step.
find_path(ASIO_INCLUDE_DIR asio.hpp)

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
# file(GLOB SOURCES "*.cpp" "public/*.cpp")
set(SOURCES main.cpp)

add_executable(exe ${SOURCES})
target_include_directories(exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                       ${LUA_INCLUDE_DIR})
target_link_libraries(exe PRIVATE OpenSSL::Crypto ${LUA_LIBRARY})

find_package(Threads REQUIRED)
target_link_libraries(exe PRIVATE Threads::Threads)

if(ASIO_INCLUDE_DIR)
  message(STATUS "Using Asio: ${ASIO_INCLUDE_DIR}")
  # asio_server.hpp has no translation unit of its own, so an INTERFACE library
  # would never actually compile it. Generate a one-line TU in the build tree so
  # a normal build catches breakage in the header.
  set(_asio_tu ${CMAKE_CURRENT_BINARY_DIR}/asio_server_tu.cpp)
  file(WRITE ${_asio_tu} "#include \"server/asio_server.hpp\"\n")
  add_library(asio_server STATIC ${_asio_tu})
  target_include_directories(asio_server PUBLIC ${ASIO_INCLUDE_DIR}
                                                ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(asio_server PUBLIC Threads::Threads)
  target_compile_definitions(asio_server PUBLIC ASIO_STANDALONE)
else()
  message(STATUS "Asio not found — skipping asio_server (brew install asio)")
endif()
