FetchContent_Declare(
        Yaml
        GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp"
        GIT_TAG "yaml-cpp-0.9.0"
)

FetchContent_MakeAvailable(Yaml)

target_compile_definitions(Widrolo_Engine_3D PRIVATE YAML_CPP_STATIC_DEFINE)

target_link_libraries(Widrolo_Engine_3D PRIVATE yaml-cpp::yaml-cpp)