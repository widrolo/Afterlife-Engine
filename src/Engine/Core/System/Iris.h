#pragma once

// This file does not contain LLM generated documentation

#include "Engine/imgui/imgui.h"
#include "Engine/Types/Nullable.h"
#include <Engine/Math/Vector.h>

#include <Engine/Types/Rendering/Iris/Handles.h>
#include <Engine/Types/Rendering/Iris/FixedFunction.h>
#include <Engine/Types/Rendering/Iris/Descriptors.h>
#include <Engine/Types/Rendering/Iris/Resource.h>
#include <Engine/Types/Rendering/Iris/Pipeline.h>
#include <Engine/Types/Rendering/Iris/Init.h>
#include <Engine/Types/Rendering/Iris/Framebuffer.h>
#include <Engine/Types/Rendering/Iris/RenderCompute.h>

#include "Engine/Types/Rendering/Iris/Statistics.h"


namespace Iris
{
    bool Init(const InitDesc& desc);
    [[deprecated]]
    void Shutdown();
    uint32 GetCurrentFrameIndex();
    uint32 GetFramesInFlight();
    bool IsFirstFrame();

    // ------------------------------------ Frame Lifecycle ------------------------------------
    void BeginFrame();
    void EndFrame();

    // --------------------------------------- Resources ---------------------------------------
    // ------- Creation --------
    BufferHandle CreateBuffer(const BufferDesc& desc);
    BufferHandle CreateBuffer(const BufferDesc& desc, const byte* initialData, sizeT initialDataSize);

    TextureHandle CreateTexture(const TextureDesc& desc);

    SamplerHandle CreateSampler(const SamplerDesc& desc);

    ShaderHandle CreateShader(const ShaderStageDesc& desc);

    ResourceTableLayoutHandle CreateResourceTableLayout(const ResourceTableLayoutDesc& desc);
    ResourceTableHandle CreateResourceTable(ResourceTableLayoutHandle layout);

    GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc);
    ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& desc);

    FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc);

    // -------- Updates --------
    void UpdateBuffer(BufferHandle buffer, sizeT dstOffset, const byte* data, sizeT size);
    void UpdateResourceTable(ResourceTableHandle table, const ResourceTableUpdateDesc& update);

    // ------- Deletion --------
    void DestroyBuffer(BufferHandle buffer);

    // --------------------------------------- Swapchain ---------------------------------------
    TextureHandle AcquireSwapchainTexture();
    FramebufferHandle GetSwapchainFramebuffer();
    void Present();

    // ----------------------------------- Command Recording -----------------------------------
    CommandBufferHandle CreateCommandBuffer(QueueType queue = QueueType::Graphics);
    CopyBufferHandle CreateCopyBuffer();
    void BeginCommandBuffer(CommandBufferHandle cmd);
    void EndCommandBuffer(CommandBufferHandle cmd);
    void SubmitCommandBuffer(CommandBufferHandle cmd);

    // ----- Render Passes -----
    void BeginRenderPass(CommandBufferHandle cmd, const RenderPassBeginDesc& desc);
    void EndRenderPass(CommandBufferHandle cmd);

    void BeginComputePass(CommandBufferHandle cmd);
    void EndComputePass(CommandBufferHandle cmd);

    void BeginCopyPass(CopyBufferHandle cmd);
    void EndCopyPass(CopyBufferHandle cmd);
    bool IsCopyPassDone(CopyBufferHandle cmd);

    // ----------------------------------- Resource Binding ------------------------------------
    void BindGraphicsPipeline(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline);
    void BindComputePipeline(CommandBufferHandle cmd, ComputePipelineHandle  pipeline);
    void BindResourceTable(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, uint32 slot, ResourceTableHandle table);
    void BindFramebuffer(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, uint32 slot, FramebufferHandle fb,
        FramebufferBindKind bindKind);
    void SetPushConstants(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, const byte* data, sizeT size);
    void BindVertexBuffers(CommandBufferHandle cmd, uint32 firstBinding, const wtl::vector<BufferHandle>& buffers,
        const wtl::vector<sizeT>& offsets);
    void BindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, sizeT offset);


    // --------------------------------------- Commands ----------------------------------------
    // -------- Dynamic --------
    void SetViewport(CommandBufferHandle cmd, const Viewport& viewport);

    // ------- Draw call -------
    void Draw(CommandBufferHandle cmd, sizeT vertexCount, sizeT instanceCount, sizeT firstVertex, sizeT firstInstance);
    void DrawIndexed(CommandBufferHandle cmd, sizeT indexCount, sizeT instanceCount, sizeT firstIndex, int32 vertexOffset,
        sizeT firstInstance);
    void DrawIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride);
    void DrawIndexedIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset, sizeT drawCount, sizeT stride);

    // ------- Dispatch --------
    void Dispatch(CommandBufferHandle cmd, sizeT groupCountX, sizeT groupCountY, sizeT groupCountZ);
    void DispatchIndirect(CommandBufferHandle cmd, BufferHandle argBuffer, sizeT offset);

    // --------- Copy ----------
    void CopyBufferToBuffer(CommandBufferHandle cmd, BufferHandle dst, sizeT dstOffset, BufferHandle src,
            sizeT srcOffset, sizeT size);

    // generally, the only use for this is a staging buffer. Doesn't need much.
    void CopyBufferToTexture(CopyBufferHandle cmd, BufferHandle src, sizeT srcOffset, TextureHandle dst);

    // ---------------------------------------- ImGui ------------------------------------------
    void ConfigureImGui();
    void ImGuiNewFrame();
    void ImGuiRenderDrawData(CommandBufferHandle cmd);
    WEngine::Nullable<ImTextureID> TextureToImGui(TextureHandle texture);
    WEngine::Nullable<ImTextureID> GetFramebufferImGuiImage(FramebufferHandle fb);

    // ---------------------------------------- Stats ------------------------------------------
    GPUInfo GetGPUInfo();
    VramStats GetVRAMStats();
    DrawStats GetDrawCallStats();
    BindStats GetBindingStats();
}

