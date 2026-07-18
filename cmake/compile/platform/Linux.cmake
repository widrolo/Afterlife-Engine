message(STATUS Detected Linux)

target_link_libraries(Afterlife_Engine PRIVATE
        dl
        pthread
)

if (STEAM_ENABLED)
    target_link_libraries(Afterlife_Engine PRIVATE
            ${STEAMWORKS_PATH}/lib/libsteam_api.so
    )
endif ()

target_compile_definitions(Afterlife_Engine PRIVATE WE_Linux)
set(CMAKE_CXX_FLAGS_PACKAGING "-O3 -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_PACKAGING "-O3 -DNDEBUG" CACHE STRING "" FORCE)