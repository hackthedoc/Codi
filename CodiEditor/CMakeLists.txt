project(CodiEditor)

file(GLOB_RECURSE EDITOR_SRC
    src/*.cpp
)

add_executable(CodiEditor ${EDITOR_SRC})

add_dependencies(CodiEditor Codi)

target_link_libraries(CodiEditor PRIVATE Codi)

if(UNIX AND NOT APPLE)
    target_link_libraries(CodiEditor PRIVATE pthread dl)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(CodiEditor PRIVATE ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

set_target_properties(CodiEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CodiEditor
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/CodiEditor
)
