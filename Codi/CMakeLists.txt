file(GLOB_RECURSE CODI_SOURCES
    Codi/*.cpp
)
file(GLOB_RECURSE CODI_HEADERS
    *.h
)

add_library(Codi SHARED ${CODI_SOURCES} ${CODI_HEADERS})

target_include_directories(Codi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Codi
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog EXCLUDE_FROM_ALL)
target_link_libraries(Codi PUBLIC spdlog::spdlog)

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


set_target_properties(Codi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Codi
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Codi
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Codi
)

target_precompile_headers(Codi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cdpch.h
)
