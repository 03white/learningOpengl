// 注意：glad 必须在 glfw 之前 include，因为 glad 负责提供 OpenGL 头声明
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include"Application.h"

// 顶点着色器
const char *vertexShaderSource="#version 330 core\n"
    "layout (location=0) in vec3 aPos;\n"
    "out vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position=vec4(aPos,1.0);\n"
    "   vertexColor=vec4(0.5,0.0,0.0,1.0);\n"
    "}\0";
unsigned int vertexShader;

//片段着色器
const char* fragmentShaderSource="#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor=vertexColor;\n"
    "}\0";
unsigned int fragmentShader;

//着色器程序
unsigned int shaderProgram;

float vertices[]={
    -0.5f,-0.5f,0.0f,
     0.5f,-0.5f,0.0f,
     0.0f, 0.5f,0.0f
    };
unsigned int VAO;//顶点数组
unsigned int VBO;//顶点缓冲


void initShader(){
    vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,nullptr);
    glCompileShader(vertexShader);

    fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,nullptr);
    glCompileShader(fragmentShader);

    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    {
    //1.绑定VBO，上传数据
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    //设置读取规则
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void delResource(){
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteProgram(shaderProgram);
}

void render(){
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,3);
}
int main() {
    auto& app=Application::instance();
    app.init();
    initShader();
    app.setRenderCallback(render);
    app.run();
    return 0;
}

