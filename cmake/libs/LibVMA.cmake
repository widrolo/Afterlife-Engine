FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
        GIT_TAG "v3.4.0"
)

FetchContent_MakeAvailable(VulkanMemoryAllocator)

target_link_libraries(Afterlife_Engine PRIVATE GPUOpen::VulkanMemoryAllocator)