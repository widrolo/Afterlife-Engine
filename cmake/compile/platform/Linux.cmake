message(STATUS Detected Linux)

target_link_libraries(Widrolo_Engine_3D PRIVATE
        dl
        pthread
)

if (STEAM_ENABLED)
    target_link_libraries(Widrolo_Engine_3D PRIVATE
            ${STEAMWORKS_PATH}/lib/libsteam_api.so
    )
endif ()

target_compile_definitions(Widrolo_Engine_3D PRIVATE WE_Linux)
set(CMAKE_CXX_FLAGS_PACKAGING "-O3 -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_PACKAGING "-O3 -DNDEBUG" CACHE STRING "" FORCE)