
# target 名 = 当前目录名（复制本目录即可作为新子项目，无需改名）
get_filename_component(PROJ_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# ---- glad：预编译静态库，用 IMPORTED target 包装 ----
add_library(glad_${PROJ_NAME} STATIC IMPORTED GLOBAL)
set_target_properties(glad_${PROJ_NAME} PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/libglad.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ---- glfw：预编译静态库，用 IMPORTED target 包装 ----
add_library(glfw_${PROJ_NAME} STATIC IMPORTED GLOBAL)
set_target_properties(glfw_${PROJ_NAME} PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/libglfw3.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ----其他头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# ---- 可执行文件 ----
set(SOURCES 
    main.cpp
    src/Application.cpp
    src/Shader.cpp
)
add_executable(${PROJ_NAME} ${SOURCES})
target_link_libraries(${PROJ_NAME} PRIVATE glad_${PROJ_NAME} glfw_${PROJ_NAME})

# GLFW 静态库在各平台需要链接的系统依赖
if(WIN32)
    target_link_libraries(${PROJ_NAME} PRIVATE gdi32 opengl32)
elseif(APPLE)
    target_link_libraries(${PROJ_NAME} PRIVATE
        "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJ_NAME} PRIVATE OpenGL::GL X11 dl pthread)
endif()