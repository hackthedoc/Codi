add_executable(Sandbox
    Application.cpp
)

target_link_libraries(Sandbox PRIVATE Codi)

set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Sandbox
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Sandbox
)
