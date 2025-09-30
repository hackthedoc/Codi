file(GLOB_RECURSE SANDBOX_SRC
    src/*.cpp
)

add_executable(Sandbox ${SANDBOX_SRC})

add_dependencies(Sandbox Codi)

target_link_libraries(Sandbox PRIVATE Codi)

if(UNIX AND NOT APPLE)
    target_link_libraries(Sandbox PRIVATE pthread dl)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(Sandbox PRIVATE ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Sandbox
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Sandbox
)
