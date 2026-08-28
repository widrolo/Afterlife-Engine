#if GPU_BACKEND == GPU_VULKAN

#include <Engine/Core/System/Iris.h>

#include "IrisGlobals.h"
#include "Engine/Util/Log.h"
#include "Helpers/Helpers.h"
#include <array>

#include "Engine/Util/TimeAnalysis.h"

namespace Iris
{
    // Again, burden of performance is on the user.
    void BindGraphicsPipeline(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline)
    {
        WEngine::TimeSample sample("[Iris]BindGraphicsPipeline");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to bind pipeline!");
            return;
        }
        if (pipeline == 0 || pipeline > loadedPipelines.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid pipeline handle, refusing to bind pipeline!");
            return;
        }

        vkCmdBindPipeline(GetCurrentCmdBuff(cmd), VK_PIPELINE_BIND_POINT_GRAPHICS, loadedPipelines[pipeline - 1].pipeline);
        stats.bindStats.total++;
        stats.bindStats.graphicsPipelineBinds++;
    }

    void BindComputePipeline(CommandBufferHandle cmd, ComputePipelineHandle  pipeline)
    {
        WEngine::TimeSample sample("[Iris]BindComputePipeline");
        PrintNotImplemented("BindComputePipeline");
        // not implemented but its fine.
        stats.bindStats.total++;
        stats.bindStats.computePipelineBinds++;
    }

    void BindResourceTable(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, uint32 slot, ResourceTableHandle table)
    {
        WEngine::TimeSample sample("[Iris]BindResourceTable");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to bind resource table!");
            return;
        }
        if (pipeline == 0 || pipeline > loadedPipelines.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid pipeline handle, refusing to bind resource table!");
            return;
        }
        if (table == 0 || table > loadedResourceTables.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid table handle, refusing to bind resource table!");
            return;
        }

        vkCmdBindDescriptorSets(GetCurrentCmdBuff(cmd), VK_PIPELINE_BIND_POINT_GRAPHICS, loadedPipelines[pipeline - 1].layout,
            slot, 1, &loadedResourceTables[table - 1].set, 0, nullptr);
        stats.bindStats.total++;
        stats.bindStats.tableBinds++;
    }

    void BindFramebuffer(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, uint32 slot, FramebufferHandle fb)
    {
        WEngine::TimeSample sample("[Iris]BindFramebuffer");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to bind framebuffer!");
            return;
        }
        if (pipeline == 0 || pipeline > loadedPipelines.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid pipeline handle, refusing to bind framebuffer!");
            return;
        }
        if (fb == 0 || fb > loadedRenderTargets.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid framebuffer handle, refusing to bind framebuffer!");
            return;
        }

        Vulkan_RenderTarget& rt = loadedRenderTargets[fb - 1];

        vkCmdBindDescriptorSets(GetCurrentCmdBuff(cmd), VK_PIPELINE_BIND_POINT_GRAPHICS, loadedPipelines[pipeline - 1].layout,
            slot, 1, &rt.descSets[rt.currentImage], 0, nullptr);
        stats.bindStats.total++;
        stats.bindStats.tableBinds++;
    }

    void SetPushConstants(CommandBufferHandle cmd, GraphicsPipelineHandle pipeline, const byte* data, sizeT size)
    {
        WEngine::TimeSample sample("[Iris]SetPushConstants");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to bind push constants!");
            return;
        }
        if (pipeline == 0 || pipeline > loadedPipelines.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid pipeline handle, refusing to bind push constants!");
            return;
        }
        if (data == nullptr)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Passed null data, refusing to bind push constants!");
            return;
        }
        if (size == 0)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Passed data of size zero, refusing to bind push constants!");
            return;
        }
        // we will later have a push constants function just for compute. Also, 0 offset is okay because we pass all info once.
        vkCmdPushConstants(GetCurrentCmdBuff(cmd), loadedPipelines[pipeline - 1].layout,
            loadedPipelines[pipeline - 1].pushStageFlags, 0, size, data);
    }

    void BindVertexBuffers(CommandBufferHandle cmd, uint32 firstBinding, const wtl::vector<BufferHandle>& buffers,
        const wtl::vector<sizeT>& offsets)
    {
        WEngine::TimeSample sample("[Iris]BindVertexBuffers");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to bind vertex buffers!");
            return;
        }

        if (buffers.empty())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("No buffers provided, refusing to bind vertex buffers!");
            return;
        }

        if (buffers.size() > 16)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Too many vertex buffers provided, refusing to bind vertex buffers!");
            return;
        }

        if (buffers.size() != offsets.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Buffer and offsets count is not equal, refusing to bind vertex buffers!");
            return;
        }

        // even if one buffer is off, we dont want to bind anything.
        for (const auto& buff : buffers)
        {
            if (buff == 0 || buff > loadedBuffers.size())
            {
                WEngine::WLog::SetConsoleWarning();
                WEngine::WLog::ConsoleLog("Invalid vertex buffer handle, refusing to bind vertex buffers!");
                return;
            }

            if (loadedBuffers[buff - 1].usage != BufferUsage::Vertex)
            {
                WEngine::WLog::SetConsoleWarning();
                WEngine::WLog::ConsoleLog("Provided buffer is not vertex buffer, refusing to bind vertex buffers!");
                return;
            }
        }

        std::array<VkBuffer, 16> buffs;
        std::array<VkDeviceSize, 16> offs;

        for (sizeT i = 0; i < buffers.size(); ++i)
        {
            buffs[i] = loadedBuffers[buffers[i] - 1].buffer;
            offs[i] = offsets[i];
        }

        // evil rvalue trick
        VkBuffer* buffPtr = buffs.data();
        VkDeviceSize* offPtr = offs.data();

        vkCmdBindVertexBuffers(GetCurrentCmdBuff(cmd), firstBinding, buffers.size(), buffPtr, offPtr);
        stats.bindStats.total++;
        stats.bindStats.vertexBinds++;
    }

    void BindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, sizeT offset)
    {
        WEngine::TimeSample sample("[Iris]BindIndexBuffer");
        if (cmd == 0 || cmd > loadedCommandBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid command buffer handle, refusing to bind index buffers!");
            return;
        }

        if (buffer == 0 || buffer > loadedBuffers.size())
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Invalid index buffer handle, refusing to bind index buffers!");
            return;
        }

        if (loadedBuffers[buffer - 1].usage != BufferUsage::Index)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::ConsoleLog("Provided buffer is not index buffer, refusing to bind index buffers!");
            return;
        }

        vkCmdBindIndexBuffer(GetCurrentCmdBuff(cmd), loadedBuffers[buffer - 1].buffer, offset, VK_INDEX_TYPE_UINT32);
        stats.bindStats.total++;
        stats.bindStats.indexBinds++;
    }
}

#endif
