cmake_minimum_required(VERSION 3.16)
project(Codi)

# ----------------------------
# Sources
# ----------------------------
file(GLOB_RECURSE CODI_SOURCES src/*.cpp)
file(GLOB_RECURSE CODI_HEADERS src/*.h)

add_library(Codi STATIC ${CODI_SOURCES} ${CODI_HEADERS})

target_include_directories(Codi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ----------------------------
# External Libraries Macro
# ----------------------------
macro(add_vendor NAME TARGET)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/${NAME} EXCLUDE_FROM_ALL)
    target_link_libraries(Codi PUBLIC ${TARGET})
endmacro()

# ----------------------------
# Add Vendors
# ----------------------------
add_vendor(spdlog spdlog::spdlog)
add_vendor(GLFW glfw)
add_vendor(GLAD glad)
add_vendor(imgui ImGui)
add_vendor(glm glm)
add_vendor(stb_image stb)
add_vendor(entt entt)
add_vendor(yaml-cpp yaml-cpp::yaml-cpp)
add_vendor(ImGuizmo ImGuizmo)

# ----------------------------
# Platform-specific Definitions
# ----------------------------
if (WIN32)
    target_compile_definitions(Codi
        PRIVATE CODI_BUILD_DLL
        PUBLIC CODI_PLATFORM_WINDOWS
    )
    target_link_libraries(Codi PRIVATE opengl32)
elseif(APPLE)
    target_compile_definitions(Codi
        PRIVATE CODI_BUILD_DLL
        PUBLIC CODI_PLATFORM_APPLE
    )
elseif(UNIX)
    target_compile_definitions(Codi
        PRIVATE CODI_BUILD_DLL
        PUBLIC CODI_PLATFORM_LINUX
    )
    find_package(OpenGL REQUIRED)
    target_link_libraries(Codi PRIVATE OpenGL::GL dl pthread)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# ----------------------------
# Debug-specific Definitions
# ----------------------------
target_compile_definitions(Codi
    PRIVATE $<$<CONFIG:Debug>:CODI_DEBUG>
    PRIVATE $<$<CONFIG:Debug>:CODI_PROFILE>
)

# ----------------------------
# Output Directories
# ----------------------------
set_target_properties(Codi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Codi
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Codi
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Codi
)

# ----------------------------
# Precompiled Headers
# ----------------------------
target_precompile_headers(Codi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cdpch.h
)
