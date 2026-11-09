// 注意：glad 必须在 glfw 之前 include，因为 glad 负责提供 OpenGL 头声明
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include"Application.h"

const char *vertexShaderSource="#version 330 core\n"
    "layout (location=0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position=vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
    "}\0";
const char* fragmentShaderSource="#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()"
    "{\n"
    "   FragColor=vec4(0.0f,0.5f,0.2f,1.0f);\n"
    "}\0";
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;
float vertices[]={
     0.5f, 0.5f,0.0f, //右上角
     0.5f,-0.5f,0.0f, //右下角
     -0.5f, -0.5f,0.0f, //左下角
     -0.5f,0.5f,0.0f   //左上角
};
unsigned int indices[]={
    //注意索引从0开始
    //此例的索引(0,1,2,3)就是顶点数组vertices的下标
    //这样可以由下表代表顶点组合成矩形
    0,1,3,//第一个三角形
    1,2,3 //第二个三角形
};
unsigned int VAO;
unsigned int VBO;
unsigned int EBO;

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
    glGenBuffers(1,&EBO);
    glGenVertexArrays(1,&VAO);

    glBindVertexArray(VAO);
    {
        //1.绑定VBO，上传数据
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
        //2.绑定EBO，上传数据
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
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
    glDeleteBuffers(1,&EBO);
    glDeleteProgram(shaderProgram);
}

void render(){
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
}
int main() {
    auto& app=Application::instance();
    app.init();
    initShader();
    app.setRenderCallback(render);
    app.run();
    delResource();
    return 0;
}
