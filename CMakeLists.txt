cmake_minimum_required(VERSION 3.20)

project(learnopengl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# 生成 compile_commands.json，供 clangd / IDE 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 各章节子项目
add_subdirectory(gl-01)
add_subdirectory(gl-02)
add_subdirectory(gl-03)
add_subdirectory(gl-sample)