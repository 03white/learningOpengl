# OpenGL 纹理使用说明

本项目演示了 OpenGL 3.3 Core Profile 下纹理的基本使用方法，包括纹理加载、配置和渲染。

## 核心流程

### 1. 纹理对象创建与绑定

```cpp
unsigned int texture;
glGenTextures(1, &texture);           // 生成纹理对象
glBindTexture(GL_TEXTURE_2D, texture); // 绑定为 2D 纹理
```

**关键 API**:
- `glGenTextures(GLsizei n, GLuint* textures)` - 生成 n 个纹理对象 ID
- `glBindTexture(GLenum target, GLuint texture)` - 绑定纹理到目标类型（GL_TEXTURE_2D）

### 2. 纹理参数设置

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // S轴环绕方式
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // T轴环绕方式
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 缩小过滤
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大过滤
```

**关键 API**:
- `glTexParameteri(GLenum target, GLenum pname, GLint param)` - 设置纹理参数

**常用参数**:
- 环绕方式：`GL_REPEAT`（重复）、`GL_CLAMP_TO_EDGE`（边缘拉伸）、`GL_MIRRORED_REPEAT`（镜像重复）
- 过滤方式：`GL_LINEAR`（线性插值）、`GL_NEAREST`（最近邻）、`GL_LINEAR_MIPMAP_LINEAR`（三线性过滤）

### 3. 图像加载（stb_image）

```cpp
stbi_set_flip_vertically_on_load(true); // OpenGL 纹理坐标原点在左下角，需翻转图片
int width, height, nrChannels;
unsigned char* data = stbi_load("path/to/image.png", &width, &height, &nrChannels, 0);
```

**关键函数**:
- `stbi_set_flip_vertically_on_load(int flag)` - 设置是否垂直翻转图片
- `stbi_load(const char* filename, int* x, int* y, int* channels, int desired_channels)` - 加载图片
- `stbi_image_free(void* data)` - 释放图片内存

### 4. 纹理数据上传

```cpp
GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D); // 生成多级渐远纹理
```

**关键 API**:
- `glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data)` - 上传 2D 纹理数据
  - `level`: Mipmap 级别（0 为基础级别）
  - `internalformat`: GPU 内部存储格式（GL_RGB/GL_RGBA）
  - `format`: 源数据格式（GL_RGB/GL_RGBA）
  - `type`: 数据类型（GL_UNSIGNED_BYTE）
- `glGenerateMipmap(GLenum target)` - 自动生成多级渐远纹理（Mipmap）

### 5. 顶点数据配置（纹理坐标）

```cpp
float vertices[] = {
    // 位置(x,y,z)    纹理坐标(s,t)
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // 左下
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // 右下
     0.0f,  0.5f, 0.0f,  0.5f, 1.0f  // 顶部
};

// 位置属性 location=0
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// 纹理坐标属性 location=1
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
glEnableVertexAttribArray(1);
```

**纹理坐标范围**: (s, t) 坐标范围为 [0.0, 1.0]，原点在左下角

### 6. 着色器配置

**顶点着色器** (shader.vs):
```glsl
#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;  // 传递纹理坐标到片段着色器
}
```

**片段着色器** (shader.fs):
```glsl
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D ourTexture; // 纹理采样器

void main() {
    FragColor = texture(ourTexture, TexCoord); // 采样纹理
}
```

**关键 GLSL 函数**:
- `texture(sampler2D sampler, vec2 coord)` - 从纹理采样颜色

### 7. 渲染时绑定纹理

```cpp
shader->use();
glActiveTexture(GL_TEXTURE0);           // 激活纹理单元 0
glBindTexture(GL_TEXTURE_2D, texture);  // 绑定纹理到当前单元
shader->setInt("ourTexture", 0);        // 告诉着色器采样器使用纹理单元 0
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, 3);
```

**关键 API**:
- `glActiveTexture(GLenum texture)` - 激活指定纹理单元（GL_TEXTURE0 ~ GL_TEXTURE31）
- `glUniform1i(GLint location, GLint value)` - 设置采样器 uniform（通过 Shader::setInt 封装）

## 完整工作流程

```
创建纹理对象 → 设置参数 → 加载图片 → 上传纹理数据 → 生成 Mipmap
                ↓
配置顶点属性（位置 + 纹理坐标） → 编译着色器
                ↓
渲染循环：激活纹理单元 → 绑定纹理 → 设置采样器 uniform → 绘制
```

## 注意事项

1. **Y 轴翻转**: OpenGL 纹理坐标原点在左下角，而图片格式（如 PNG/JPG）原点在左上角，需调用 `stbi_set_flip_vertically_on_load(true)`
2. **通道数匹配**: 根据图片通道数（RGB/RGBA）选择正确的格式参数
3. **纹理单元**: 支持多纹理时需注意单元编号（GL_TEXTURE0 + i）与采样器 uniform 值的对应
4. **资源释放**: 使用完毕后调用 `glDeleteTextures()` 释放纹理对象

## 依赖库

- **stb_image.h** - 单头文件图片加载库，支持 JPG/PNG/BMP/TGA 等格式
- **GLAD** - OpenGL 函数加载器
- **GLFW** - 跨平台窗口和输入管理
