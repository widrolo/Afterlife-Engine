FetchContent_Declare(
        glm
        GIT_REPOSITORY	"https://github.com/g-truc/glm.git"
        GIT_TAG "1.0.3"
)

FetchContent_MakeAvailable(glm)

target_link_libraries(Afterlife_Engine PRIVATE glm::glm)