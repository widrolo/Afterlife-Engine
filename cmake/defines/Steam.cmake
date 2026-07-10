set(STEAMWORKS_PATH "${CMAKE_SOURCE_DIR}/libs/Steamworks-SDK")

set(STEAM_ENABLED ${OPTION_ENABLED})

target_compile_definitions(Widrolo_Engine_3D PRIVATE STEAM=${STEAM_ENABLED})