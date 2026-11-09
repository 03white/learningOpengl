// 注意：glad 必须在 glfw 之前 include，因为 glad 负责提供 OpenGL 头声明
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"Application.h"
#include "Shader.h"

float vertices[]={
    -0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f, //左下
     0.5f,-0.5f,0.0f,0.0f,1.0f,0.0f, //右下
     0.0f, 0.5f,0.0f,0.0f,0.0f,1.0f  //顶部
    };
unsigned int VAO;//顶点数组
unsigned int VBO;//顶点缓冲
Shader* shader = nullptr;
void initShader(){
    shader = new Shader(ASSET_DIR "/shader/shader.vs", ASSET_DIR "/shader/shader.fs");
    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    {
    //1.绑定VBO，上传数据
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    //设置读取规则
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void delResource(){
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
}

void render(){
    // //更新uniform颜色
    // float timeValue=glfwGetTime();
    // float greenValue=sin(timeValue)/2.0f+0.5;
    // int vertexColorLocation=glGetUniformLocation(shaderProgram,"ourColor");
    // glUniform4f(vertexColorLocation,0.0f,greenValue,0.0f,1.0f);
    //绘制三角形
    glm::mat4 trans(glm::mat4(1.0));
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    shader->use();
    shader->setUniform("transform",trans);
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

