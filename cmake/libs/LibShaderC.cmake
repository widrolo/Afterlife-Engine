find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Packaging")
    target_link_libraries(Afterlife_Engine PRIVATE Vulkan::shaderc_combined)
endif()