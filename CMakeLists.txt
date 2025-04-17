cmake_minimum_required(VERSION 3.5.0)
project(physics_simulation_software VERSION 0.1.0)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

# include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/lib/external/imgui)
set(IMGUIFILEDIALOG_PATH ${CMAKE_SOURCE_DIR}/lib/external/imguifiledialog)

#Add Glad
add_library(glad STATIC ${CMAKE_SOURCE_DIR}/src/glad.c)

# ImGui sources
set(IMGUI_SOURCES
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
    # ${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.cpp
)
set(IMGUIFILEDIALOG_SOURCES
    ${IMGUIFILEDIALOG_PATH}/ImGuiFileDialog.cpp
)

#ImGui headers
set(IMGUI_HEADER
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3_loader.h
)


# STB image
set(STB_IMAGE_SOURCE ${CMAKE_SOURCE_DIR}/lib/stb.cpp)

# Add source files
file(GLOB_RECURSE SOURCE_FILES
${CMAKE_SOURCE_DIR}/src/*.c
${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add header files
file(GLOB RECURSE IMGUI_HEADERS
${IMGUI_PATH}/*.h
${IMGUI_PATH}/backends/*.h
)

target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/include/glad)

# add executable
add_executable(physics_simulation_software ${SOURCE_FILES} ${IMGUI_SOURCES} ${IMGUI_HEADERS} ${STB_IMAGE_SOURCE} ${IMGUIFILEDIALOG_SOURCES})

# add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(physics_simulation_software PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends ${IMGUI_HEADER} ${IMGUIFILEDIALOG_PATH})
target_link_libraries(physics_simulation_software
    glad
    glfw
    OpenGL::GL
    assimp
)


# # Link libraries
# if(WIN32)
#     target_link_libraries(${PROJECT_NAME}
#         ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
#         opengl32
#     )
# else()
#     find_package(OpenGL REQUIRED)
#     find_library(GLFW_LIBRARY glfw)
#     target_link_libraries(${PROJECT_NAME}
#         ${OPENGL_LIBRARIES}
#         ${GLFW_LIBRARY}
#         dl
#     )
# endif()

# # Copy DLL files to build directory on Windows
# if(WIN32)
#     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${CMAKE_SOURCE_DIR}/lib/glfw3.dll"
#         $<TARGET_FILE_DIR:${PROJECT_NAME}>
#     )
# endif()
