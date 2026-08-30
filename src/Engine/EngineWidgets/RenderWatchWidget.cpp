#include "RenderWatchWidget.h"

#include "Engine/EngineDefines.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/RenderPasses/ForwardPass.h"
#include "Engine/Core/RenderPasses/Storage/GTAOSettings.h"
#include "Engine/Core/RenderPasses/Storage/Passes.h"
#include "Engine/Core/System/Iris.h"

using namespace WEngine;

void RenderWatchWidget::Setup()
{
    m_widgetName = "Render Watch";
}

void RenderWatchWidget::RenderInternal()
{
    SetSize({800, 600});
    Header();
    ImGui::SeparatorText("Vram Statistics");
    VramDisplay();
    ImGui::SeparatorText("Rendering Statistics");
    RenderDisplay();
    ShowPassSettings();
}

void RenderWatchWidget::Header() const
{
    if (ImGui::BeginTable("gpuInfoTable", 3))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        switch (GPU_BACKEND)
        {
            case GPU_VULKAN:    ImGui::Text("Renderer: VULKAN");    break;
            case GPU_D3D12:     ImGui::Text("Renderer: D3D12");     break;
            case GPU_METAL:     ImGui::Text("Renderer: METAL");     break;
            case GPU_AGC:       ImGui::Text("Renderer: AGC");       break;
            case GPU_NVN:       ImGui::Text("Renderer: NVN");       break;
            default:
                ImGui::Text("Renderer: Unknown");
                break;
        }
        ImGui::TableNextColumn();
        static auto gpu = Iris::GetGPUInfo();
        ImGui::Text("Total VRAM: %.1fGB", (float64)gpu.totalVram / (float64)GB);
        ImGui::TableNextColumn();
        ImGui::Text("%s", gpu.gpuName.c_str());
        ImGui::EndTable();
    }
}

void RenderWatchWidget::VramDisplay() const
{
    auto vram = Iris::GetVRAMStats();

    ImGui::Text("Vram Used: %.2fGB", (float64)vram.total / (float64)GB);

    const uint32 height = 150;
    uint64 bufferTotal = vram.vertexBuffers + vram.indexBuffers + vram.uniformBuffer +
        vram.storageBuffers + vram.transferBuffers;
    uint64 texTotal = vram.colorTextures + vram.depthTextures + vram.framebuffers;
    uint64 shaderTotal = vram.vertexShader + vram.geometryShader + vram.tesselationShader +
        vram.fragmentShaders + vram.computeShaders;

    ImGui::BeginChild("buffUsage", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, height));
    ImGui::Text("Buffers");
    ImGui::Text("Total: %.2fMB", (float64)bufferTotal / (float64)MB);
    VramDisplayBuffers();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("texUsage", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, height));
    ImGui::Text("Textures");
    ImGui::Text("Total: %.2fMB", (float64)texTotal / (float64)MB);
    VramDisplayTextures();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("shaUsage", ImVec2(ImGui::GetContentRegionAvail().x, height));
    ImGui::Text("Shaders");
    ImGui::Text("Total: %.2fMB", (float64)shaderTotal / (float64)MB);
    VramDisplayShaders();
    ImGui::EndChild();
}

void RenderWatchWidget::PrintVramUsage(const std::string& category, sizeT sizeInBytes) const
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", category.c_str());
    ImGui::TableNextColumn();
    ImGui::Text("%.2fMB", (float64)sizeInBytes / (float64)MB);
}

void RenderWatchWidget::VramDisplayBuffers() const
{
    auto vram = Iris::GetVRAMStats();
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("buffUsageTable", 2, flags))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Buffer Type");
        ImGui::TableNextColumn();
        ImGui::Text("Memory");

        PrintVramUsage("Vertex", vram.vertexBuffers);
        PrintVramUsage("Index", vram.indexBuffers);
        PrintVramUsage("Uniform", vram.uniformBuffer);
        PrintVramUsage("Storage", vram.storageBuffers);
        PrintVramUsage("Transfer", vram.transferBuffers);

        ImGui::EndTable();
    }
}

void RenderWatchWidget::VramDisplayTextures() const
{
    auto vram = Iris::GetVRAMStats();
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("texUsageTable", 2, flags))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Texture Type");
        ImGui::TableNextColumn();
        ImGui::Text("Memory");

        PrintVramUsage("Color", vram.colorTextures);
        PrintVramUsage("Depth Stencil", vram.depthTextures);
        PrintVramUsage("Framebuffers", vram.framebuffers);

        ImGui::EndTable();
    }
}

void RenderWatchWidget::VramDisplayShaders() const
{
    auto vram = Iris::GetVRAMStats();
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("shaUsageTable", 2, flags))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Shader Stage");
        ImGui::TableNextColumn();
        ImGui::Text("Memory");

        PrintVramUsage("Vertex", vram.vertexShader);
        PrintVramUsage("Geometry", vram.geometryShader);
        PrintVramUsage("Tesselation", vram.tesselationShader);
        PrintVramUsage("Fragment", vram.fragmentShaders);
        PrintVramUsage("Compute", vram.computeShaders);

        ImGui::EndTable();
    }
}

void RenderWatchWidget::RenderDisplay() const
{
    auto draw = Iris::GetDrawCallStats();
    auto bind = Iris::GetBindingStats();
    const uint32 height = 190;

    ImGui::BeginChild("drawcalls", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, height));
    ImGui::Text("Drawcalls");
    ImGui::Text("Total: %llu", draw.total);
    RenderDisplayDrawcall();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("bindings", ImVec2(ImGui::GetContentRegionAvail().x, height));
    ImGui::Text("Bindings");
    ImGui::Text("Total: %llu", bind.total);
    RenderDisplayBindings();
    ImGui::EndChild();
}

void RenderWatchWidget::PrintRenderUsage(const std::string &category, sizeT count) const
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", category.c_str());
    ImGui::TableNextColumn();
    ImGui::Text("%llu", count);
}

void RenderWatchWidget::RenderDisplayDrawcall() const
{
    auto draw = Iris::GetDrawCallStats();
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("DrawcallsTable", 2, flags))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Call Type");
        ImGui::TableNextColumn();
        ImGui::Text("Call Count");

        PrintRenderUsage("Draw", draw.draw);
        PrintRenderUsage("DrawIndexed", draw.drawIndexed);
        PrintRenderUsage("DrawIndirect", draw.drawIndirect);
        PrintRenderUsage("DrawIndirectIndexed", draw.drawIndirectIndexed);
        PrintRenderUsage("Dispatch", draw.dispatch);
        PrintRenderUsage("DispatchIndirect", draw.dispatchIndirect);
        PrintRenderUsage("Copy Buffer to Buffer", draw.copyBuffToBuff);
        PrintRenderUsage("Copy Buffer to Texture", draw.copyBuffToTex);

        ImGui::EndTable();
    }
}

void RenderWatchWidget::RenderDisplayBindings() const
{
    auto bind = Iris::GetBindingStats();
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
    if (ImGui::BeginTable("BindingsTable", 2, flags))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Binding Type");
        ImGui::TableNextColumn();
        ImGui::Text("Binding Count");

        PrintRenderUsage("Graphics Pipeline", bind.graphicsPipelineBinds);
        PrintRenderUsage("Compute Pipeline", bind.computePipelineBinds);
        PrintRenderUsage("Vertex Buffer", bind.vertexBinds);
        PrintRenderUsage("Index Buffer", bind.indexBinds);
        PrintRenderUsage("Resource Table", bind.tableBinds);

        ImGui::EndTable();
    }
}

void RenderWatchWidget::ShowPassSettings()
{
    //ImGui::DragFloat("GTAO Someval", &Rendering::Passes::gtao->GetSettings().someVal, 0.01, 0, 1);
}

