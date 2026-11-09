// 注意：glad 必须在 glfw 之前 include，因为 glad 负责提供 OpenGL 头声明
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <cmath>
#include "Application.h"
#include "Shader.h"
#include "stb_image.h"
const unsigned int SCR_WIDTH=800;
const unsigned int SCR_HEIGHT=600;
Shader* shader = nullptr;
float vertices[]={
    //位置(x,y,z)    纹理坐标(s,t)
    -0.5f,-0.5f,0.0f,0.0f,0.0f, //左下
     0.5f,-0.5f,0.0f,1.0f,0.0f, //右下
     0.0f, 0.5f,0.0f,0.5f,1.0f  //顶部
    };
unsigned int VAO;//顶点数组ID
unsigned int VBO;//顶点缓冲ID
unsigned int texture;//纹理ID
void setDataBind(){
    shader = new Shader(ASSET_DIR "/shader/shader.vs", ASSET_DIR "/shader/shader.fs");
    glGenBuffers(1,&VBO);
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    {
    //1.绑定VBO，上传数据
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    //位置属性 location=0
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    //纹理坐标属性 location=1
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void setTextureBind(){
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    //设置环绕和过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载图片
    stbi_set_flip_vertically_on_load(true); // 翻转Y轴
    int width, height, nrChannels;
    unsigned char *data = stbi_load(ASSET_DIR "/res/container.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // 根据图片通道数决定格式
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // 自动生成多级渐远纹理
    }
    else
    {
    std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // 释放图片内存
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
    shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("ourTexture", 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,3);
}
int main() {
    auto& app=Application::instance();
    app.init();
    setDataBind();
    setTextureBind();
    app.setRenderCallback(render);
    app.run();
    return 0;
}

