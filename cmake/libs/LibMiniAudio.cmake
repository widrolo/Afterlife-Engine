FetchContent_Declare(
        miniaudio
        GIT_REPOSITORY	"https://github.com/mackron/miniaudio.git"
        GIT_TAG "0.11.25"
)

FetchContent_MakeAvailable(miniaudio)

if(NOT TARGET miniaudio::miniaudio)
    add_library(miniaudio::miniaudio ALIAS miniaudio)
endif()
target_link_libraries(Afterlife_Engine PRIVATE miniaudio::miniaudio)