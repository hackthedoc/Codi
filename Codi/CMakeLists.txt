file(GLOB_RECURSE CODI_SOURCES
    Codi/*.cpp
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
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW EXCLUDE_FROM_ALL)

target_link_libraries(Codi 
    PUBLIC spdlog::spdlog
    PUBLIC glfw
)

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
    PRIVATE $<$<CONFIG:Debug>:CODI_ENABLE_ASSERTS>
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
