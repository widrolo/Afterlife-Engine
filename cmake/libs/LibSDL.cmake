FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG "main"
)

FetchContent_MakeAvailable(SDL)

target_link_libraries(Widrolo_Engine_3D PRIVATE SDL3::SDL3-static)