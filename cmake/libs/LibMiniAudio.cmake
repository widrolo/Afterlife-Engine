FetchContent_Declare(
        miniaudio
        GIT_REPOSITORY	"https://github.com/mackron/miniaudio.git"
        GIT_TAG "0.11.25"
)

FetchContent_MakeAvailable(miniaudio)

if(TARGET miniaudio)
    target_compile_definitions(miniaudio PRIVATE
            MA_NO_DECODING
            MA_NO_ENCODING
            MA_NO_GENERATION
            MA_NO_NODE_GRAPH
            MA_NO_ENGINE
    )
endif()

if(NOT TARGET miniaudio::miniaudio)
    add_library(miniaudio::miniaudio ALIAS miniaudio)
endif()
target_link_libraries(Afterlife_Engine PRIVATE miniaudio::miniaudio)