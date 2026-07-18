FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG "main"
)

unset(SDL_SHARED)
FetchContent_MakeAvailable(SDL)

target_link_libraries(Afterlife_Engine PRIVATE SDL3::SDL3-static)