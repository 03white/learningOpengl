cmake_minimum_required(VERSION 3.20)

project(learnopengl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# 生成 compile_commands.json，供 clangd / IDE 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# lessions: LearnOpenGL 教程跟练章节
add_subdirectory(lessions)

# projects: 自己的实践项目
add_subdirectory(projects)
