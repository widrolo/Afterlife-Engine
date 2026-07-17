set(GPU_OPENGL_VAL 1)
set(GPU_VULKAN_VAL 2)
set(GPU_METAL_VAL 3)
set(GPU_D3D12_VAL 4)
set(GPU_AGC_VAL 5)
set(GPU_NVN_VAL 6)

target_compile_definitions(Widrolo_Engine_3D PRIVATE
        GPU_OPENGL=${GPU_OPENGL_VAL}
        GPU_VULKAN=${GPU_VULKAN_VAL}
        GPU_METAL=${GPU_METAL_VAL}
        GPU_D3D12=${GPU_D3D12_VAL}
        GPU_AGC=${GPU_AGC_VAL}
        GPU_NVN=${GPU_NVN_VAL}
)

target_compile_definitions(Widrolo_Engine_3D PRIVATE GPU_BACKEND=${GPU_VULKAN_VAL})