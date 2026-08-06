# CMakeLists.txt 知识点解析

## 本项目学到的关键技术

### 1. **IMPORTED 库：使用预编译的静态库**

大多数教程只教 `add_library(mylib src1.cpp src2.cpp)` 从源码编译库，但实际项目中经常需要链接**已编译好的第三方库**。

```cmake
# 声明一个预编译的静态库
add_library(glad_${PROJ_NAME} STATIC IMPORTED GLOBAL)
set_target_properties(glad_${PROJ_NAME} PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/libglad.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

**关键点：**
- `IMPORTED`：告诉 CMake 这个库已经编译好了，不需要构建
- `STATIC`：静态库类型（`.a` / `.lib`）
- `GLOBAL`：让这个 target 在整个项目中可见
- `IMPORTED_LOCATION`：指定库文件的实际路径
- `INTERFACE_INCLUDE_DIRECTORIES`：指定头文件路径，链接这个库的目标会自动继承

**实际意义：**
- 不重复编译第三方库（节省时间）
- 便于管理多个版本的库
- 适合闭源库或预编译包

---

### 2. **动态获取项目名：复制即用的模板化设计**

```cmake
get_filename_component(PROJ_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
```

**作用：** 自动用当前目录名作为可执行文件名。

**好处：**
- 复制整个 `gl-01` 目录为 `gl-02`，不需要修改 CMakeLists.txt
- 避免硬编码项目名
- 减少人为错误

**配合 IMPORTED 库的命名：**
```cmake
add_library(glad_${PROJ_NAME} ...)  # 生成 glad_gl-01、glad_gl-02 等独立 target
```
这样多个子项目可以各自使用自己的库版本，互不干扰。

---

### 3. **跨平台系统库链接**

静态库（如 GLFW）通常依赖操作系统的底层 API，需要额外链接系统库：

```cmake
if(WIN32)
    target_link_libraries(${PROJ_NAME} PRIVATE gdi32 opengl32)
elseif(APPLE)
    target_link_libraries(${PROJ_NAME} PRIVATE
        "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
else()  # Linux
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJ_NAME} PRIVATE OpenGL::GL X11 dl pthread)
endif()
```

**平台依赖说明：**

| 平台    | 需要的系统库                          | 原因                     |
|---------|---------------------------------------|--------------------------|
| Windows | `gdi32`, `opengl32`                   | 窗口绘制、OpenGL 驱动    |
| macOS   | Cocoa, OpenGL, IOKit frameworks       | 窗口系统、图形、输入设备 |
| Linux   | `OpenGL::GL`, `X11`, `dl`, `pthread`  | OpenGL、X11窗口、动态加载、线程 |

**学习要点：**
- 静态库不是"完全独立"的，可能隐含依赖
- 不同平台的窗口系统完全不同
- `find_package(OpenGL REQUIRED)` 是找系统 OpenGL 的标准方法

---

### 4. **PRIVATE 链接：避免依赖传播**

```cmake
target_link_libraries(${PROJ_NAME} PRIVATE glad_${PROJ_NAME} glfw_${PROJ_NAME})
```

**PRIVATE vs PUBLIC vs INTERFACE：**

- `PRIVATE`：只有当前 target 能用，不传播给依赖者
- `PUBLIC`：当前 target 和依赖者都能用
- `INTERFACE`：当前 target 不用，但依赖者能用

**本项目为什么用 PRIVATE？**
- `gl-01` 是可执行文件（叶子节点），没有其他库会依赖它
- glad 和 glfw 只在 main.cpp 内部使用
- 避免无意义的依赖传播

**如果改成库会怎样：**
```cmake
add_library(myengine Application.cpp)
target_link_libraries(myengine PUBLIC glad_${PROJ_NAME})  # Application.h 暴露了 glad 类型
target_link_libraries(myengine PRIVATE glfw_${PROJ_NAME}) # 只在 .cpp 内部用
```

---

## 完整依赖链解读

```
gl-01 (可执行文件)
├─ PRIVATE → glad_gl-01 (IMPORTED)
│   └─ INTERFACE_INCLUDE_DIRECTORIES: include/
├─ PRIVATE → glfw_gl-01 (IMPORTED)
│   └─ INTERFACE_INCLUDE_DIRECTORIES: include/
└─ PRIVATE → gdi32 + opengl32 (Windows系统库)
```

**编译时发生什么：**
1. 编译 `main.cpp` / `Application.cpp` 时，自动添加 `-I include`
2. 链接时，按顺序链接：
   ```
   lib/glad/libglad.a
   lib/glfw/libglfw3.a
   gdi32.lib
   opengl32.lib
   ```

---

## 对比：如果从源码编译库

```cmake
# 传统做法（不推荐用于第三方库）
add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)

add_executable(gl-01 main.cpp)
target_link_libraries(gl-01 PRIVATE glad)
```

**问题：**
- 每次 `cmake --build` 都要重新编译 glad
- 无法使用官方预编译包
- 难以管理多版本库

---

## 方案对比：IMPORTED vs 传统 link_directories

### 传统做法（link_directories + include_directories）

```cmake
project(umathlib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

# 1. 全局添加头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)  # 对应 g++ -I

add_executable(${PROJECT_NAME} main.cpp)

# 2. 全局添加库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)         # 对应 g++ -L路径

# 3. 链接库（只写库名，不写路径）
target_link_libraries(${PROJECT_NAME} PRIVATE       # 对应 g++ -lcmathlib
    cmathlib
)
```

**等价的 g++ 命令：**
```bash
g++ -I./include main.cpp -L./lib -lcmathlib -o umathlib
```

---

### 本项目做法（IMPORTED target）

```cmake
# 1. 创建 IMPORTED target，明确指定库文件路径和头文件路径
add_library(glad_gl-01 STATIC IMPORTED GLOBAL)
set_target_properties(glad_gl-01 PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/libglad.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(gl-01 main.cpp)

# 2. 链接 target（CMake 自动处理头文件和库文件路径）
target_link_libraries(gl-01 PRIVATE glad_gl-01)
```

**CMake 自动生成的编译命令：**
```bash
g++ -I./include main.cpp ./lib/glad/libglad.a -o gl-01
```

---

### 核心区别对比表

| 特性                     | `link_directories` 传统方案           | `IMPORTED target` 现代方案          |
|--------------------------|---------------------------------------|-------------------------------------|
| **头文件路径**           | `include_directories()` 全局生效      | `INTERFACE_INCLUDE_DIRECTORIES` 自动继承 |
| **库文件路径**           | `link_directories()` + 库名           | `IMPORTED_LOCATION` 精确指定完整路径 |
| **作用域**               | 全局污染，影响所有 target             | 局部化，只影响链接该 target 的目标   |
| **多版本库管理**         | ❌ 困难（同名库冲突）                 | ✅ 简单（不同 target 名区分）        |
| **依赖传播**             | ❌ 无法控制                           | ✅ PUBLIC/PRIVATE/INTERFACE 精确控制 |
| **库文件查找**           | ❌ 运行时搜索，可能找错                | ✅ 配置时验证，立即报错              |
| **跨平台可移植性**       | ⚠️ 需手动处理 `.a` / `.lib` 差异      | ✅ 通过变量或生成器表达式自动适配    |
| **CMake 官方推荐**       | ⚠️ 已废弃，仅为兼容旧项目              | ✅ 现代 CMake 3.x+ 标准做法          |

---

### 为什么传统方案有问题？

#### 问题 1：全局污染
```cmake
include_directories(/usr/local/include)  # 所有 target 都会搜索这个路径
link_directories(/usr/local/lib)         # 所有 target 都会搜索这个路径

add_executable(app1 main1.cpp)
add_executable(app2 main2.cpp)  # 即使不需要，也被强制添加了上面的路径
```

#### 问题 2：库名冲突
```cmake
link_directories(lib/v1)  # 有 libmath.a
link_directories(lib/v2)  # 也有 libmath.a

target_link_libraries(app1 PRIVATE math)  # 链接哪个版本？不确定！
target_link_libraries(app2 PRIVATE math)  # 可能链接错版本
```

#### 问题 3：找不到库时不报错
```cmake
link_directories(/some/path)
target_link_libraries(app PRIVATE nonexist)  # 配置阶段不报错，编译时才失败
```

---

### 为什么 IMPORTED 方案更好？

#### 优势 1：精确控制依赖
```cmake
add_library(mathv1 STATIC IMPORTED)
set_target_properties(mathv1 PROPERTIES IMPORTED_LOCATION lib/v1/libmath.a)

add_library(mathv2 STATIC IMPORTED)
set_target_properties(mathv2 PROPERTIES IMPORTED_LOCATION lib/v2/libmath.a)

target_link_libraries(app1 PRIVATE mathv1)  # 明确用 v1
target_link_libraries(app2 PRIVATE mathv2)  # 明确用 v2
```

#### 优势 2：自动传播头文件
```cmake
add_library(mylib STATIC IMPORTED)
set_target_properties(mylib PROPERTIES
    IMPORTED_LOCATION lib/libmylib.a
    INTERFACE_INCLUDE_DIRECTORIES include  # 链接 mylib 的 target 自动获得 include 路径
)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE mylib)  # 不需要再写 include_directories()
```

#### 优势 3：配置时验证
```cmake
set_target_properties(mylib PROPERTIES
    IMPORTED_LOCATION lib/nonexist.a  # CMake 立即报错：文件不存在
)
```

---

### 什么时候还能用传统方案？

**仅在以下场景勉强可接受：**
1. 移植非常老的项目（CMake 2.x 时代）
2. 库文件路径由用户在命令行指定（`-DLIB_DIR=/custom/path`）
3. 系统库（但现在更推荐用 `find_package`）

**示例：系统库的正确做法**
```cmake
# ❌ 不推荐
link_directories(/usr/lib)
target_link_libraries(app PRIVATE pthread)

# ✅ 推荐
find_package(Threads REQUIRED)
target_link_libraries(app PRIVATE Threads::Threads)
```

---

### 迁移指南：从传统方案改为 IMPORTED

**原代码（传统）：**
```cmake
include_directories(vendor/include)
link_directories(vendor/lib)
target_link_libraries(myapp PRIVATE foo bar)
```

**改进后（IMPORTED）：**
```cmake
# 为每个库创建 IMPORTED target
add_library(foo STATIC IMPORTED)
set_target_properties(foo PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/lib/libfoo.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/vendor/include
)

add_library(bar STATIC IMPORTED)
set_target_properties(bar PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/lib/libbar.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/vendor/include
)

# 链接时使用 target 名
target_link_libraries(myapp PRIVATE foo bar)
```

**如果有多个可执行文件共享相同的库，可以写成函数：**
```cmake
function(import_prebuilt_lib name libfile incdir)
    add_library(${name} STATIC IMPORTED GLOBAL)
    set_target_properties(${name} PROPERTIES
        IMPORTED_LOCATION ${libfile}
        INTERFACE_INCLUDE_DIRECTORIES ${incdir}
    )
endfunction()

import_prebuilt_lib(foo vendor/lib/libfoo.a vendor/include)
import_prebuilt_lib(bar vendor/lib/libbar.a vendor/include)
```

---

## 现代 CMake 链接方式的完整层级

IMPORTED 并不是唯一的现代做法，它处于以下层级中：

```
find_package()          ← 最优先（库自带 CMake 配置时用）
    ↓ 库不支持
FetchContent            ← 从网上拉源码、自动编译
    ↓ 不想联网/有现成 .a 文件
IMPORTED target         ← 手里有预编译库，本项目就是这种
    ↓ 祖传老项目
link_directories        ← 避免使用
```

### find_package（最推荐）

适用场景：库本身提供了 CMake 配置文件（`XXXConfig.cmake`），现代库通常都支持。

```cmake
find_package(glfw3 REQUIRED)
target_link_libraries(gl-01 PRIVATE glfw3)  # 路径、头文件、系统依赖全自动处理
```

优点：完全跨平台，不需要手动管理任何路径。
缺点：库必须已安装在系统中，或通过包管理器安装（vcpkg、conan 等）。

### FetchContent（适合需要联网的项目）

适用场景：不想手动管理库文件，让 CMake 自动下载并编译。

```cmake
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

target_link_libraries(gl-01 PRIVATE glfw)
```

优点：不需要手动准备库文件，版本锁定精确。
缺点：首次构建需要联网，编译时间较长。

### IMPORTED target（本项目做法）

适用场景：手里已有预编译的 `.a` / `.lib` 文件，库没有提供 CMake 配置。

```cmake
add_library(glfw STATIC IMPORTED GLOBAL)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/libglfw3.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

优点：不依赖网络，构建速度快，精确控制使用哪个版本的库文件。
缺点：需要手动准备各平台的库文件（`.a` 是 GCC/MinGW，MSVC 用 `.lib`）。

### 选择依据

| 情况                              | 推荐方案           |
|-----------------------------------|--------------------|
| 库支持 `find_package`（现代库）   | `find_package`     |
| 想自动管理版本、可以联网          | `FetchContent`     |
| 手里有 `.a` 文件、入门练习        | `IMPORTED target`  |
| 维护祖传老项目                    | `link_directories`（尽量迁移） |

本项目属于第三种情况：手里有编译好的 glad 和 GLFW，用 IMPORTED 是最合适的现代做法。

---

## 实战建议

1. **第三方预编译库优先用 IMPORTED**（本项目做法）
2. **自己的库用源码编译**（如果要做成库，把 Application.cpp 分离）
3. **系统库依赖要记得测试跨平台**
4. **目录名即项目名**的设计适合练习项目和原型
5. **新项目完全避免 `include_directories()` 和 `link_directories()`**
6. **看不懂时先 `cmake --build . --verbose`**，看实际执行的编译命令
