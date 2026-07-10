FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
        GIT_TAG "v3.4.0"
)

FetchContent_MakeAvailable(VulkanMemoryAllocator)

target_link_libraries(Widrolo_Engine_3D PRIVATE GPUOpen::VulkanMemoryAllocator)