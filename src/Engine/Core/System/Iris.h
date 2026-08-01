#pragma once

// This file does not contain LLM generated documentation

#include <Engine/Util/Handles.h>
#include <Engine/Util/BitwiseMacros.h>

#include "Engine/imgui/imgui.h"
#include "Engine/Types/Nullable.h"

namespace Iris
{
    DEFINE_OPAQUE_HANDLE(BufferHandle);
    DEFINE_OPAQUE_HANDLE(TextureHandle);
    DEFINE_OPAQUE_HANDLE(SamplerHandle);
    DEFINE_OPAQUE_HANDLE(ShaderHandle);
    DEFINE_OPAQUE_HANDLE(ResourceTableLayoutHandle)
    DEFINE_OPAQUE_HANDLE(ResourceTableHandle)
    DEFINE_OPAQUE_HANDLE(GraphicsPipelineHandle);
    DEFINE_OPAQUE_HANDLE(ComputePipelineHandle);
    DEFINE_OPAQUE_HANDLE(FramebufferHandle);
    DEFINE_OPAQUE_HANDLE(CommandBufferHandle);

    enum class ImgFormat : uint8
    {
        Unknown,

        // -------- Render Targets Only --------
        RGBA8_UNorm,
        BGRA8_UNorm,
        RGBA8_SRGB,
        BGRA8_SRGB,

        // ------------ Depth Only -------------
        D24_UNorm_S8_UInt,

        // ----------- Texture Only ------------
        BC1,    // Albedo
        BC4,    // Heightmaps and single channel stuff
        BC5,    // Dual channel normal maps

        ImgFormat_Count
    };

    // Its obvious i dislike the idea that Khronos had about merging image formats and vertex formats
    enum class VertFormat : uint8
    {
        Float1,
        Float2,
        Float3,
        Float4,
        VertFormat_Count
    };

    enum class BufferUsage : uint8
    {
        None            = 0,
        Vertex          = BIT(0),
        Index           = BIT(1),
        Uniform         = BIT(2),
        Storage         = BIT(3),
        Indirect        = BIT(4),
        TransferSrc     = BIT(5),
        TransferDst     = BIT(6),
    };
    DEFINE_ENUM_BITWISE(BufferUsage)

    enum class TextureUsage : uint8
    {
        None           = 0,
        Sampled        = BIT(0),
        Storage        = BIT(1),
        ColorTarget    = BIT(2),
        DepthTarget    = BIT(3),
        TransferSrc    = BIT(4),
        TransferDst    = BIT(5),
    };
    DEFINE_ENUM_BITWISE(TextureUsage)

    // In theory, there are also 3D textures. However, im not keep on experimenting with it just yet.
    // But I am keeping this an Enum instead of a using for the future.
    enum class TextureType : uint8
    {
        Texture2D,
    };

    enum class SampleCount : uint8
    {
        Samples1        = 1,
        Samples2        = 2,
        Samples4        = 4,
        Samples8        = 8,
    };

    // Yes, mesh shaders exist; no im never ever using them ever.
    enum class ShaderStage : uint8
    {
        Vertex          = BIT(0),
        Fragment        = BIT(1),
        Compute         = BIT(2),
        Geometry        = BIT(3),
        TessControl     = BIT(4),
        TessEval        = BIT(5),
    };
    DEFINE_ENUM_BITWISE(ShaderStage)

    enum class TopologyType : uint8
    {
        Point_List,
        Line_List,
        Triangle_List,

        Line_Strip,
        Triangle_Strip,

        // Really only useful for 2D, but Ui exists so we might as well.
        Triangle_Fan,
    };

    using IndexFormat = uint32;

    // AI copy-paste begin; no way im writing this myself.
    enum class CompareOp : uint8
    {
        Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always
    };
    enum class CullMode : uint8 { None, Front, Back };
    enum class FrontFace : uint8 { CounterClockwise, Clockwise };
    enum class FillMode : uint8 { Solid, Wireframe };
    enum class BlendFactor : uint8
    {
        Zero, One, SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor, SrcAlpha, OneMinusSrcAlpha, DstAlpha,
        OneMinusDstAlpha, ConstantColor, OneMinusConstantColor, ConstantAlpha, OneMinusConstantAlpha, SrcAlphaSaturate,
        Src1Color, OneMinusSrc1Color, Src1Alpha, OneMinusSrc1Alpha
    };
    enum class BlendOp : uint8 { Add, Subtract, ReverseSubtract, Min, Max };
    enum class StencilOp : uint8 { Keep, Zero, Replace, IncrementClamp, DecrementClamp, IncrementWrap, DecrementWrap, Invert };
    enum class LogicOp : uint8
    {
        Clear, Set, Copy, CopyInverted, Noop, Inverted, And, Nand, Or, Nor, Xor, Equiv, AndReverse, AndInverted,
        OrReverse, OrInverted
    };
    enum class FilterMode : uint8 { Nearest, Linear };
    enum class MipFilterMode : uint8 { None, Nearest, Linear };
    enum class SamplerAddressMode : uint8 { Repeat, MirrorRepeat, ClampToEdge, ClampToBorder };
    enum class BorderColor : uint8 { TransparentBlack, OpaqueBlack, OpaqueWhite };
    enum class LoadOp : uint8 { Load, Clear, DontCare };
    enum class StoreOp : uint8 { Store, DontCare };
    enum class QueueType : uint8 { Graphics, Compute, Copy };
    // AI copy-paste end

    struct BufferDesc
    {
        sizeT size;
        BufferUsage usage;
        std::string debugName;
    };

    struct TextureDesc
    {
        TextureType type = TextureType::Texture2D;
        ImgFormat format = ImgFormat::Unknown;
        uint32 width = 1;
        uint32 height = 1;
        uint32 mipLevels = 1;
        uint32 arrayLayers = 1;
        SampleCount  sampleCount = SampleCount::Samples1;
        TextureUsage usage = TextureUsage::None;
        std::string debugName;
    };

    struct SamplerDesc
    {
        FilterMode magFilter = FilterMode::Linear;
        FilterMode minFilter = FilterMode::Linear;
        MipFilterMode mipFilter = MipFilterMode::Linear;
        SamplerAddressMode addressU = SamplerAddressMode::Repeat;
        SamplerAddressMode addressV = SamplerAddressMode::Repeat;
        float32 mipLodBias = 0.0f;
        float32 minLod = 0.0f;
        float32 maxLod = 0.0f;
        float32 maxAnisotropy = 1.0f;
        bool anisotropyEnable = false;
        CompareOp compareOp = CompareOp::Never;
        bool compareEnable = false;
        BorderColor borderColor = BorderColor::TransparentBlack;
        std::string debugName;
    };

    struct ShaderStageDesc
    {
        ShaderStage stage;
        const byte* bytecode;
        sizeT bytecodeSize;
        std::string entryPoint = "main";
        std::string debugName;
    };


    struct VertexAttributeDesc
    {
        uint32 location;
        uint32 binding;
        VertFormat format;
        sizeT offset;
    };

    struct VertexBindingDesc
    {
        uint32 binding;
        uint32 stride;
        bool perInstance;
    };

    struct VertexLayoutDesc
    {
        wtl::vector<VertexAttributeDesc> attributes;
        wtl::vector<VertexBindingDesc> bindings;
    };

    struct RasterizerDesc
    {
        CullMode cullMode = CullMode::Back;
        FrontFace frontFace = FrontFace::CounterClockwise;
        FillMode fillMode = FillMode::Solid;
        bool depthClampEnable = false;
        bool depthBiasEnable = false;
        float32 depthBiasConstant = 0.0f;
        float32 depthBiasClamp = 0.0f;
        float32 depthBiasSlope = 0.0f;
        float32 lineWidth = 1.0f;
    };

    struct StencilFaceDesc
    {
        StencilOp failOp = StencilOp::Keep;
        StencilOp passOp = StencilOp::Keep;
        StencilOp depthFailOp = StencilOp::Keep;
        CompareOp compareOp = CompareOp::Always;
        byte writeMask = max_byte;
        byte compareMask = max_byte;
    };

    struct DepthStencilDesc
    {
        bool depthTestEnable = false;
        bool depthWriteEnable = true;
        CompareOp depthCompareOp = CompareOp::Less;
        bool stencilTestEnable = false;
        StencilFaceDesc front;
        StencilFaceDesc back;
    };

    struct BlendAttachmentDesc
    {
        bool blendEnable = false;
        BlendFactor srcColorFactor = BlendFactor::One;
        BlendFactor dstColorFactor = BlendFactor::Zero;
        BlendOp colorOp = BlendOp::Add;
        BlendFactor srcAlphaFactor = BlendFactor::One;
        BlendFactor dstAlphaFactor = BlendFactor::Zero;
        BlendOp alphaOp = BlendOp::Add;
        byte colorWriteMask  = 0xF;  // 0 | 0 | 0 | 0 | R | G | B | A
    };

    struct BlendDesc
    {
        bool logicOpEnable = false;
        LogicOp logicOp = LogicOp::Noop;
        wtl::vector<BlendAttachmentDesc> attachments;
        float32 blendConstants[4] = {0,0,0,0};
    };

    enum class ResourceTableEntryType : uint8
    {
        UniformBuffer,
        StorageBuffer,
        Texture,
        Sampler,
        StorageTexture,
    };


    struct ResourceTableLayoutEntry
    {
        uint32 binding;
        ShaderStage stages;
        ResourceTableEntryType type;
        uint32 count = 1;
    };

    struct ResourceTableLayoutDesc
    {
        wtl::vector<ResourceTableLayoutEntry> entries;
        std::string debugName;
    };

    struct ResourceTableWrite
    {
        uint32 binding;
        uint32 arrayIndex = 0;
        ResourceTableEntryType type;
        BufferHandle buffer;
        sizeT bufferOffset = 0;
        sizeT bufferRange = max_sizeT;
        TextureHandle texture;
        SamplerHandle sampler;
    };

    struct ResourceTableUpdateDesc
    {
        wtl::vector<ResourceTableWrite> writes;
    };

    struct GraphicsPipelineDesc
    {
        wtl::vector<ShaderStageDesc> stages;
        VertexLayoutDesc vertexLayout;
        TopologyType topology = TopologyType::Triangle_List;
        bool primitiveRestartEnable = false;
        uint32 patchControlPoints = 0;
        RasterizerDesc rasterizer;
        DepthStencilDesc depthStencil;
        BlendDesc blend;

        std::array<ResourceTableLayoutHandle, 8> tableLayouts;
        uint32 colorAttachmentCount = 1;

        uint32 pushConstantsSize = 0;
        ShaderStage pushConstantsStages = ShaderStage::Vertex;

        ImgFormat colorAttachmentFormat = ImgFormat::BGRA8_UNorm;
        ImgFormat depthStencilFormat = ImgFormat::D24_UNorm_S8_UInt;
        SampleCount sampleCount = SampleCount::Samples1;

        std::string debugName;
    };

    struct ComputePipelineDesc
    {
        ShaderStageDesc stage;
        std::array<ResourceTableLayoutHandle, 8> tableLayouts;
        uint32 colorAttachmentCount = 1;
        uint32 pushConstantsSize = 0;
        ShaderStage pushConstantsStages = ShaderStage::Compute;
        std::string debugName;
    };

    struct FramebufferDesc
    {
        wtl::vector<TextureHandle> colorAttachments;
        TextureHandle depthStencilAttachment;
        uint32 width;
        uint32 height;
        uint32 layers = 1;
        std::string debugName;
    };

    struct AttachmentOps
    {
        LoadOp  loadOp = LoadOp::Clear;
        StoreOp storeOp = StoreOp::Store;
        LoadOp  stencilLoadOp = LoadOp::DontCare;
        StoreOp stencilStoreOp = StoreOp::DontCare;
        WEngine::Color clearColor = WEngine::Color::Black;
        float32 clearDepth = 1.0f;
        uint8 clearStencil = 0;
    };

    struct RenderPassBeginDesc
    {
        FramebufferHandle framebuffer;
        std::array<AttachmentOps, 8> colorAttachments;
        uint8 colorAttachmentCount = 0;
        AttachmentOps depthStencil;
        bool hasDepthStencil = false;
    };

    struct Viewport
    {
        WEngine::Vector2 pos;
        WEngine::Vector2 extent;
        float32 minDepth;
        float32 maxDepth;
    };

    struct Scissor
    {
        WEngine::Vector2 pos;
        WEngine::Vector2 extent;
    };

    enum class WindowServer : uint8
    {
        SDL,
        GLFW,
        Win32,
        Apple,
        PlayStation,
        Nintendo
    };

    struct InitDesc
    {
        // This will be cast and treated to the proper window type based on the window server
        void* window = nullptr;
        WindowServer windowServer = WindowServer::SDL;

        bool enableDebug = false;
        bool enableEditorMode = false;
        uint32 framesInFlight = 2;
        bool vsync = true;
        WEngine::Vector2 preferredSwapchainSize = { 800, 600 };
    };

    bool Init(const InitDesc& desc);
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
    BufferHandle CreateBuffer(const BufferDesc& desc, const void* initialData, sizeT initialDataSize);

    TextureHandle CreateTexture(const TextureDesc& desc);
    TextureHandle CreateTexture(const TextureDesc& desc, const byte* texData);

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


    // --------------------------------------- Swapchain ---------------------------------------
    TextureHandle AcquireSwapchainTexture();
    FramebufferHandle GetSwapchainFramebuffer();
    void Present();

    // ----------------------------------- Command Recording -----------------------------------
    CommandBufferHandle BeginCommandBuffer(QueueType queue = QueueType::Graphics);
    void EndCommandBuffer(CommandBufferHandle cmd);
    void SubmitCommandBuffer(CommandBufferHandle cmd);
    void SubmitCommandBuffers(const CommandBufferHandle* cmds, sizeT count);

    // ----- Render Passes -----
    void BeginRenderPass(CommandBufferHandle cmd, const RenderPassBeginDesc& desc);
    void EndRenderPass(CommandBufferHandle cmd);

    void BeginComputePass(CommandBufferHandle cmd);
    void EndComputePass(CommandBufferHandle cmd);

    void BeginCopyPass(CommandBufferHandle cmd);
    void EndCopyPass(CommandBufferHandle cmd);

    // ----------------------------------- Resource Binding ------------------------------------
    void BindGraphicsPipeline(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline);
    void BindComputePipeline(CommandBufferHandle cmd, ComputePipelineHandle  pipeline);
    void BindResourceTable(CommandBufferHandle cmd, uint32 slot, ResourceTableHandle table);
    void SetPushConstants(CommandBufferHandle cmd, const byte* data, sizeT size);
    void BindVertexBuffers(CommandBufferHandle cmd, uint32 firstBinding, const BufferHandle* buffers,
        const sizeT* offsets, sizeT count);
    void BindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, sizeT offset);


    // --------------------------------------- Commands ----------------------------------------
    // -------- Dynamic --------
    void SetViewport(CommandBufferHandle cmd, const Viewport& viewport);
    void SetScissor(CommandBufferHandle cmd, const Scissor& scissor);

    // ------- Draw call -------
    void Draw(CommandBufferHandle cmd, sizeT vertexCount, sizeT instanceCount, sizeT firstVertex, sizeT firstInstance);
    void DrawIndexed(CommandBufferHandle cmd, sizeT indexCount, sizeT instanceCount, sizeT firstIndex, sizeT vertexOffset,
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
    void CopyBufferToTexture(CommandBufferHandle cmd, BufferHandle src, sizeT srcOffset, TextureHandle dst,
        WEngine::Vector2 extent);

    // ---------------------------------------- ImGui ------------------------------------------
    void ConfigureImGui();
    void ImGuiNewFrame();
    void ImGuiEndFrame();
    void ImGuiRenderDrawData(CommandBufferHandle cmd);
    WEngine::Nullable<ImTextureID> TextureToImGui(TextureHandle texture);

    // ---------------------------------------- Stats ------------------------------------------
    sizeT GetVRAMUsage();
    sizeT GetDrawCallCountLastFrame();
}

