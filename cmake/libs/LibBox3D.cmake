FetchContent_Declare(
        box3d
        GIT_REPOSITORY	"https://github.com/erincatto/box3d.git"
        GIT_TAG "v0.1.0"
)

FetchContent_MakeAvailable(box3d)

target_link_libraries(Afterlife_Engine PRIVATE box3d::box3d)