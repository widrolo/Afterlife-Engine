message(STATUS Detected Windows)

set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)

if (STEAM_ENABLED)
    target_link_libraries(Afterlife_Engine PRIVATE
        ${STEAMWORKS_PATH}/lib/steam_api64.lib
    )
endif ()

target_compile_definitions(Afterlife_Engine PRIVATE
        WE_Windows
        _CRT_SECURE_NO_WARNINGS
)
target_compile_options(Afterlife_Engine PRIVATE /MP /wd4005)
set(CMAKE_CXX_FLAGS_PACKAGING "/O2 /DNDEBUG" CACHE STRING "" FORCE)

set_property(DIRECTORY windows PROPERTY VS_STARTUP_PROJECT Afterlife_Engine)
set_target_properties(Afterlife_Engine PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Packaging>:TRUE>)
