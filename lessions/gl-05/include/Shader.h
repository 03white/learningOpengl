#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>   // 用于 glm::value_ptr
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>

class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    // 模板成员函数
    template<typename T>
    void setUniform(const std::string &name, const T& value) const {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            return;
        }
        // 根据 T 的实际类型选择对应的 OpenGL 函数
        if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            glUniform1i(location, static_cast<int>(value));
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, int>) {
            glUniform1i(location, value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, unsigned int>) {
            glUniform1ui(location, value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, float>) {
            glUniform1f(location, value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, glm::vec2>) {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, glm::vec3>) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, glm::vec4>) {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, glm::mat4>) {
            // 矩阵类型：传入 value_ptr(value)，矩阵按列主序存储，GL_FALSE 表示不需要转置
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
        // 可继续添加 mat3、mat2 等
        else {
            static_assert(sizeof(T) == 0, "Unsupported uniform type");
        }
    }
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

#endif