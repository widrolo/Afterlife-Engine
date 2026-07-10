FetchContent_Declare(
        Tinyxml
        GIT_REPOSITORY "https://github.com/leethomason/tinyxml2"
        GIT_TAG "11.0.0"
)

FetchContent_MakeAvailable(Tinyxml)

target_compile_definitions(Widrolo_Engine_3D PRIVATE TINYGLTF3_ENABLE_FS)

target_link_libraries(Widrolo_Engine_3D PRIVATE tinyxml2::tinyxml2)