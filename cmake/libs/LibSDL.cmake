FetchContent_Declare(
        SDL
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG "main"
)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL)

target_link_libraries(Afterlife_Engine PRIVATE SDL3::SDL3-static)