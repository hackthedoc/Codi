file(GLOB_RECURSE CODI_SOURCES
    Codi/*.cpp
    Codi/ImGui/*.cpp
    Platform/*.cpp
)
file(GLOB_RECURSE CODI_HEADERS
    *.h
)

add_library(Codi SHARED ${CODI_SOURCES} ${CODI_HEADERS})

target_include_directories(Codi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Codi
)

# ----- EXTERNAL LIBRARIES -----
macro(add_vendor NAME TARGET)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/${NAME} EXCLUDE_FROM_ALL)
    target_link_libraries(Codi PUBLIC ${TARGET})
endmacro()

add_vendor(spdlog spdlog::spdlog)
add_vendor(GLFW glfw)
add_vendor(GLAD glad)
add_vendor(imgui ImGui)
add_vendor(glm glm)

# ----- PLATFORM SPECIFIC DEFINES -----
if (WIN32)
    target_compile_definitions(Codi
        PRIVATE CODI_BUILD_DLL
        PUBLIC CODI_PLATFORM_WINDOWS
    )
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
else()
    message(FATAL_ERROR "unsupported platform")
endif()

# ----- DEBUG SPECIFIC DEFINITIONS -----
target_compile_definitions(Codi
    PRIVATE $<$<CONFIG:Debug>:CODI_DEBUG>
)

# ----- OUTPUT DIRS -----
set_target_properties(Codi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Codi
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Codi
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Codi
)

# ----- PCH HEADERS -----
target_precompile_headers(Codi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cdpch.h
)
