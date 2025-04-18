cmake_minimum_required(VERSION 3.29)
project(VersatileCInterpreter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "/home/max/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_package(GTest REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(third_party/glfw)

include_directories(${CMAKE_SOURCE_DIR}/generated)

# --- Build ImGui as a static library ---
# List all the core ImGui source files. (If you don't need the demo, you can remove it.)
file(GLOB IMGUI_CORE_SOURCES
        ${CMAKE_SOURCE_DIR}/third_party/imgui/*.cpp
)

# List the backend source files for GLFW and OpenGL3.
set(IMGUI_BACKEND_SOURCES
        ${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp
)

# Combine the sources into one variable.
set(IMGUI_SOURCES ${IMGUI_CORE_SOURCES} ${IMGUI_BACKEND_SOURCES})

# Create a static library for ImGui.
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
        third_party/imgui
        third_party/imgui/backends
)


add_executable(VersatileCInterpreter
        src/main.cpp
        src/CInterpreterVisitor.cpp
        src/CInterpreterVisitor.h
        src/Interpreter.cpp
        src/Interpreter.h
        generated/CParser.cpp
        generated/CLexer.cpp
        generated/CBaseVisitor.cpp
        generated/CVisitor.cpp
        src/REPL.cpp
        src/REPL.h
        src/CustomErrorListener.cpp
        src/CustomErrorListener.h
        src/Environment.cpp
        src/Environment.h
        src/Variable.h
        src/Utils.h
        src/IReplUI.h
        src/ConsoleReplUI.h
        src/ImGuiReplUI.cpp
        src/ImGuiReplUI.h
        src/Utils.cpp
        src/Function.h
        src/ReturnException.h
        src/EnvScopeGuard.h
)

target_link_libraries(VersatileCInterpreter PRIVATE
        antlr4_static
        imgui
        glfw
        OpenGL::GL)

# Include tests
add_subdirectory(tests)
