#include "RenderHandler.h"
#include <iostream>
#include <queue>

#include <Engine/Util/Log.h>
#include <Engine/Core/Handlers/AssetRepo.h>

#include <glm/gtc/matrix_transform.hpp>

#include <Engine/imgui/imgui.h>
#include <Engine/imgui/implot.h>

#include <Engine/Core/System/Iris.h>

#include "Engine/Types/DebugFlags.h"

#include "Input.h"
#include "Engine/EngineDefines.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/System/Haptic.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/TimeAnalysis.h"
#include "glm/gtc/quaternion.hpp"


using namespace WEngine;

RenderHandler::RenderHandler()
{
	InitSDL();

	Iris::InitDesc desc{};
	desc.window = m_window;

	if (!Iris::Init(desc))
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog("FATAL ERROR! GPU failed to initialize, aborting!");
		abort();
	}
	InitImGui();
	Iris::ConfigureImGui();
	PrepareRenderingSetup();
	m_textureTables.push_back({}); // dummy because UIDs are 1 ordered

	CoreSystems::GetAssetRepo()->LoadAllGPUAssets();

	m_projection = glm::perspective(
		glm::radians(90.0f),
		m_windowResolution.x / m_windowResolution.y,
		0.01f,
		1000.0f
		);

	m_primaryCmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
	Iris::FramebufferDesc frameDesc{};
	frameDesc.hasDepth = true;
	frameDesc.width = 1920;
	frameDesc.height = 1080;
	frameDesc.debugName = "Primary Framebuffer";
	frameDesc.resourceTableLayout = m_mainTableLayout;
	frameDesc.sampler = m_sampler;
	m_primaryFramebuffer = Iris::CreateFramebuffer(frameDesc);
}

void RenderHandler::EnableEditorMode(const Vector2& viewportResolution)
{
	m_isEditor = true;
	m_viewportResolution = viewportResolution;

	m_projection = glm::perspective(
		glm::radians(90.0f),
		m_viewportResolution.x / m_viewportResolution.y,
		0.01f,
		1000.0f
		);
}

Iris::FramebufferHandle RenderHandler::EditorGetViewportFramebuffer()
{
	return m_primaryFramebuffer;
}


void RenderHandler::BeginFrame()
{
	TimeSample sample("RenderHandler::BeginFrame");
	Iris::BeginFrame();
	Iris::AcquireSwapchainTexture();
	Iris::ImGuiNewFrame();
	Iris::BeginCommandBuffer(m_primaryCmd);

	Iris::RenderPassBeginDesc desc{};
	desc.colorAttachment.clearColor = m_camColor;
	desc.framebuffer = m_primaryFramebuffer;
	Iris::BeginRenderPass(m_primaryCmd, desc);

	Iris::Viewport vp{};
	vp.extent = {1920.0f, 1080.0f};
	vp.maxDepth = 1.0f;
	Iris::SetViewport(m_primaryCmd, vp);


	Vector3 camPos = m_camera.position;
	Quaternion camRot = m_camera.rotation;

	glm::quat q(camRot.w, camRot.x, camRot.y, camRot.z);

	m_viewMatrix = glm::mat4_cast(glm::conjugate(q));
	m_viewMatrix = glm::translate(m_viewMatrix, glm::vec3(-camPos.x, camPos.y, -camPos.z));
}

void RenderHandler::RenderFrame()
{
	TimeSample sample("RenderHandler::RenderFrame");
	auto vpGLM = m_projection * m_viewMatrix;

	wtl::vector<Iris::BufferHandle> vertBuffs{CoreSystems::GetAssetRepo()->GetVertexBuffer()};
	wtl::vector<sizeT> vertOffs{0};

	Iris::BindGraphicsPipeline(m_primaryCmd, m_statPipeline);
	Iris::BindVertexBuffers(m_primaryCmd, 0, vertBuffs, vertOffs);
	Iris::BindIndexBuffer(m_primaryCmd, CoreSystems::GetAssetRepo()->GetIndexBuffer(), 0);

	Mat4x4 vp = Glm4x4ToMat4x4(vpGLM);

	for (const auto& plan : m_renderPlanQueue)
		RenderSinglePlan(plan, vp);

	Iris::BindGraphicsPipeline(m_primaryCmd, m_basicPipeline);
	Iris::BindVertexBuffers(m_primaryCmd, 0, vertBuffs, vertOffs);
	Iris::BindIndexBuffer(m_primaryCmd, CoreSystems::GetAssetRepo()->GetIndexBuffer(), 0);

	for (const auto& mission : m_renderQueue)
		RenderSingleMission(mission, vpGLM);

	Iris::EndRenderPass(m_primaryCmd);
	Iris::EndCommandBuffer(m_primaryCmd);
	Iris::SubmitCommandBuffer(m_primaryCmd);

	RenderScreen();

	Iris::Present();
	m_renderQueue.clear();
	m_renderPlanQueue.clear();
	Iris::EndFrame();
	m_currentBoundTexture = 0;
}

void RenderHandler::RenderSingleMission(const RenderMission& mission, const glm::mat4& vp)
{
	TimeSample sample("RenderHandler::RenderSingleMission");
	if (!CoreSystems::GetAssetRepo()->IsTextureDoneLoading(mission.textureUID))
		return;
	MeshAssetMission meshMission{};
	meshMission.uid = mission.meshUID;
	CoreSystems::GetAssetRepo()->GetAsset(meshMission);

	sizeT indexSize = sizeof(uint32);
	sizeT vertexSize = sizeof(float32) * 3 + sizeof(float32) * 3 + sizeof(float32) * 2;

	sizeT indexCount = (meshMission.model.indexSize - meshMission.model.indexOffset) / indexSize;
	sizeT indexOffset = meshMission.model.indexOffset / indexSize;
	sizeT vertOffset = meshMission.model.vertexOffset / vertexSize;

	Mat4x4 mvp = Glm4x4ToMat4x4(vp * CalcModelMatrixGLM(mission.transform));

	if (mission.textureUID != m_currentBoundTexture)
		Iris::BindResourceTable(m_primaryCmd, m_basicPipeline, 0, m_textureTables[mission.textureUID]);
	m_currentBoundTexture = mission.textureUID;
	Iris::SetPushConstants(m_primaryCmd, m_basicPipeline, (byte*)&mvp, sizeof(mvp));
	Iris::DrawIndexed(m_primaryCmd, indexCount, 1, indexOffset, vertOffset, 0);
}

void RenderHandler::RenderSinglePlan(const RenderPlan &plan, const Mat4x4 &vp)
{
	TimeSample sample("RenderHandler::RenderSinglePlan");
	Iris::BindVertexBuffers(m_primaryCmd, 1, {plan.statBuffer}, {0});
	for (const auto& part : plan.parts)
	{
		if (!CoreSystems::GetAssetRepo()->IsTextureDoneLoading(part.textureUID))
			continue;
		MeshAssetMission meshMission{};
		meshMission.uid = part.meshUID;
		CoreSystems::GetAssetRepo()->GetAsset(meshMission);

		sizeT indexSize = sizeof(uint32);
		sizeT vertexSize = sizeof(float32) * 3 + sizeof(float32) * 3 + sizeof(float32) * 2;

		sizeT indexCount = (meshMission.model.indexSize - meshMission.model.indexOffset) / indexSize;
		sizeT indexOffset = meshMission.model.indexOffset / indexSize;
		sizeT vertOffset = meshMission.model.vertexOffset / vertexSize;

		if (part.textureUID != m_currentBoundTexture)
			Iris::BindResourceTable(m_primaryCmd, m_basicPipeline, 0, m_textureTables[part.textureUID]);
		m_currentBoundTexture = part.textureUID;
		Iris::SetPushConstants(m_primaryCmd, m_basicPipeline, (byte*)&vp, sizeof(vp));
		Iris::DrawIndexed(m_primaryCmd, indexCount, part.count, indexOffset, vertOffset, part.offset);
	}
}

void RenderHandler::RenderScreen()
{
	Iris::BeginCommandBuffer(m_screenCmd);

	Iris::RenderPassBeginDesc desc{};
	desc.colorAttachment.clearColor = m_camColor;
	Iris::BeginRenderPass(m_screenCmd, desc);

	Iris::Viewport vp{};
	vp.extent = {1920.0f, 1080.0f};
	vp.maxDepth = 1.0f;
	Iris::SetViewport(m_screenCmd, vp);

	wtl::vector<Iris::BufferHandle> vertBuffs{m_screenMesh};
	wtl::vector<sizeT> vertOffs{0};

	Iris::BindGraphicsPipeline(m_screenCmd, m_screenPipeline);
	Iris::BindVertexBuffers(m_screenCmd, 0, vertBuffs, vertOffs);

	Iris::BindFramebuffer(m_screenCmd, m_screenPipeline, 0, m_primaryFramebuffer);
	Iris::Draw(m_screenCmd, 4, 1, 0, 0);

	Iris::ImGuiRenderDrawData(m_screenCmd);

	Iris::EndRenderPass(m_screenCmd);
	Iris::EndCommandBuffer(m_screenCmd);
	Iris::SubmitCommandBuffer(m_screenCmd);
}

void RenderHandler::UpdateCamera(const Transform &trans)
{
	m_camera = trans;
}

void RenderHandler::UpdateCamera(const Vector3& position, const Quaternion& rotation)
{
	m_camera.position = position;
	m_camera.rotation = rotation;
}

void RenderHandler::UpdateCameraColor(const Color& backColor)
{
	m_camColor = backColor;
}

void RenderHandler::AddToRenderQueue(RenderMission& mission)
{
	if (mission.meshUID == 0 || mission.textureUID == 0)
		return;
	m_renderQueue.push_back(mission);
}

void RenderHandler::AddPlanToRenderQueue(RenderPlan& plan)
{
	if (plan.statBuffer == 0)
		return;
	m_renderPlanQueue.push_back(plan);
}

void RenderHandler::RegisterTexture(Iris::TextureHandle handle)
{
	auto table = Iris::CreateResourceTable(m_mainTableLayout);

	Iris::ResourceTableUpdateDesc desc{};
	Iris::ResourceTableWrite write{};

	write.binding = 0;
	write.type = Iris::ResourceTableEntryType::Texture;
	write.texture = handle;
	write.sampler = m_sampler;

	desc.writes.push_back(write);
	Iris::UpdateResourceTable(table, desc);

	// this assumes that the asset repo sends them in order. If not, then we will know exactly when this
	// needs to be addressed.
	m_textureTables.push_back(table);
}


void RenderHandler::PrepareRenderingSetup()
{
	LoadShaders();
	CreateTables();
	CreatePipelines();
	CreateScreen();
}

void RenderHandler::CreateScreen()
{
	m_screenCmd = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);

	float32 screen[] = {
		//  pos          uv
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	Iris::BufferDesc buffDesc{};
	buffDesc.debugName = "Screen Mesh";
	buffDesc.usage = Iris::BufferUsage::Vertex;
	buffDesc.size = sizeof(screen);
	m_screenMesh = Iris::CreateBuffer(buffDesc, (byte*)screen, sizeof(screen));

	SpirVAssetMission mission{};
	mission.name = "screen";
	mission.shaderType = SpirVAssetMission::VertexShader;
	CoreSystems::GetAssetRepo()->GetAsset(mission);

	Iris::ShaderStageDesc desc{};
	desc.debugName = "Screen Shader";
	desc.stage = Iris::ShaderStage::Vertex;
	desc.entryPoint = "main";
	desc.bytecode = mission.shaderCode;
	desc.bytecodeSize = mission.shaderSize;
	m_screenVertexShader = Iris::CreateShader(desc);

	mission.name = "screen";
	mission.shaderType = SpirVAssetMission::FragmentShader;
	CoreSystems::GetAssetRepo()->GetAsset(mission);

	desc.debugName = "Screen Fragment Shader";
	desc.stage = Iris::ShaderStage::Fragment;
	desc.bytecode = mission.shaderCode;
	desc.bytecodeSize = mission.shaderSize;
	m_screenFragmentShader = Iris::CreateShader(desc);

	// we will use basic 2D rendering for the screen
	// |     Name     |  Size  |
	// |--------------|--------|
	// |Position      | 8 bytes|
	// |UV            | 8 bytes|
	Iris::VertexLayoutDesc vertDesc{};
	Iris::VertexBindingDesc binding{};
	binding.binding = 0;
	binding.perInstance = false;
	binding.stride = 16;
	vertDesc.bindings.push_back(binding);

	Iris::VertexAttributeDesc attribute{};
	attribute.binding = 0;
	attribute.location = 0;
	attribute.format = Iris::VertFormat::Float2;
	vertDesc.attributes.push_back(attribute);

	attribute.location = 1;
	vertDesc.attributes.push_back(attribute);

	Iris::RasterizerDesc rastDesc{};
	rastDesc.frontFace = Iris::FrontFace::CounterClockwise;

	Iris::GraphicsPipelineDesc pipeDesc{};
	pipeDesc.debugName = "Screen Pipeline";
	pipeDesc.vertexShader = m_screenVertexShader;
	pipeDesc.fragmentShader = m_screenFragmentShader;
	pipeDesc.vertexLayout = vertDesc;
	pipeDesc.rasterizer = rastDesc;
	pipeDesc.topology = Iris::TopologyType::Triangle_Strip;
	pipeDesc.depthStencil = Iris::DepthStencilDesc{};
	pipeDesc.blend = Iris::BlendDesc{};

	pipeDesc.tableLayouts[0] = m_mainTableLayout;
	pipeDesc.tableAttachmentCount = 1;

	m_screenPipeline = Iris::CreateGraphicsPipeline(pipeDesc);
}

void RenderHandler::LoadShaders()
{
	// I just realized that this kills the purpose of Iris but we can revisit this later,
	SpirVAssetMission mission{};
	mission.name = "basic";
	mission.shaderType = SpirVAssetMission::VertexShader;
	CoreSystems::GetAssetRepo()->GetAsset(mission);

	Iris::ShaderStageDesc desc{};
	desc.debugName = "Simple Vertex Shader";
	desc.stage = Iris::ShaderStage::Vertex;
	desc.entryPoint = "main";
	desc.bytecode = mission.shaderCode;
	desc.bytecodeSize = mission.shaderSize;
	m_vertexShader = Iris::CreateShader(desc);

	mission.name = "basicInst";
	mission.shaderType = SpirVAssetMission::VertexShader;
	CoreSystems::GetAssetRepo()->GetAsset(mission);

	desc.debugName = "Instanced Vertex Shader";
	desc.bytecode = mission.shaderCode;
	desc.bytecodeSize = mission.shaderSize;
	m_statVertexShader = Iris::CreateShader(desc);

	mission.name = "basic";
	mission.shaderType = SpirVAssetMission::FragmentShader;
	CoreSystems::GetAssetRepo()->GetAsset(mission);

	desc.debugName = "Main Fragment Shader";
	desc.stage = Iris::ShaderStage::Fragment;
	desc.bytecode = mission.shaderCode;
	desc.bytecodeSize = mission.shaderSize;
	m_fragmentShader = Iris::CreateShader(desc);
}

void RenderHandler::CreateTables()
{
	Iris::ResourceTableLayoutDesc desc{};
	desc.debugName = "Main Table Layout";

	Iris::ResourceTableLayoutEntry texEntry{};
	texEntry.binding = 0;
	texEntry.stages = Iris::ShaderStage::Fragment;
	texEntry.type = Iris::ResourceTableEntryType::Texture;
	texEntry.count = 1; // one texture per table
	desc.entries.push_back(texEntry);

	m_mainTableLayout = Iris::CreateResourceTableLayout(desc);

	Iris::SamplerDesc samplerDesc{};
	samplerDesc.debugName = "Main Sampler";
	m_sampler = Iris::CreateSampler(samplerDesc);
}

void RenderHandler::CreatePipelines()
{
	// the asmf file thats loaded in is laid out like this:
	// |     Name     |  Size  |
	// |--------------|--------|
	// |Position      |12 bytes|
	// |Normal        |12 bytes|
	// |UV            | 8 bytes|
	Iris::VertexLayoutDesc vertDesc{};
	Iris::VertexBindingDesc binding{};
	binding.binding = 0;
	binding.perInstance = false;
	binding.stride = 32;
	vertDesc.bindings.push_back(binding);

	Iris::VertexAttributeDesc attribute{};
	attribute.binding = 0;
	attribute.location = 0;
	attribute.format = Iris::VertFormat::Float3;

	vertDesc.attributes.push_back(attribute);

	attribute.location = 1;
	vertDesc.attributes.push_back(attribute);

	attribute.location = 2;
	attribute.format = Iris::VertFormat::Float2;
	vertDesc.attributes.push_back(attribute);

	Iris::RasterizerDesc rastDesc{};
	rastDesc.frontFace = Iris::FrontFace::CounterClockwise;

	Iris::DepthStencilDesc depthDesc{};
	depthDesc.depthTestEnable = true;
	depthDesc.depthWriteEnable = true;
	depthDesc.depthCompareOp = Iris::CompareOp::Less;

	Iris::GraphicsPipelineDesc desc{};
	desc.debugName = "Main Pipeline";
	desc.vertexShader = m_vertexShader;
	desc.fragmentShader = m_fragmentShader;
	desc.vertexLayout = vertDesc;
	desc.rasterizer = rastDesc;
	desc.depthStencil = depthDesc;
	desc.blend = Iris::BlendDesc{}; // default one is good enough

	desc.tableLayouts[0] = m_mainTableLayout;
	desc.tableAttachmentCount = 1;

	desc.pushConstantsSize = sizeof(Mat4x4);

	m_basicPipeline = Iris::CreateGraphicsPipeline(desc);

	// ---- now the instanced one ----

	binding.binding = 1;
	binding.perInstance = true;
	binding.stride = sizeof(Mat4x4);
	vertDesc.bindings.push_back(binding);

	attribute.binding = 1;
	attribute.format = Iris::VertFormat::Float4;
	// this is still the stupidest way to send a matrix to the gpu.
	attribute.location = 3;
	vertDesc.attributes.push_back(attribute);
	attribute.location = 4;
	vertDesc.attributes.push_back(attribute);
	attribute.location = 5;
	vertDesc.attributes.push_back(attribute);
	attribute.location = 6;
	vertDesc.attributes.push_back(attribute);

	desc.vertexLayout = vertDesc;
	desc.vertexShader = m_statVertexShader;

	m_statPipeline = Iris::CreateGraphicsPipeline(desc);
}

Mat4x4 RenderHandler::CalcModelMatrix(const Transform &transform)
{
	return Glm4x4ToMat4x4(CalcModelMatrixGLM(transform));
}

glm::mat4 RenderHandler::CalcModelMatrixGLM(const Transform &transform)
{
	glm::quat q(transform.rotation.w, transform.rotation.x,
				transform.rotation.y, transform.rotation.z);

	glm::mat4 modelMatrix = glm::mat4_cast(q);

	modelMatrix[0] *= transform.size.x;
	modelMatrix[1] *= transform.size.y;
	modelMatrix[2] *= transform.size.z;

	modelMatrix[3] = glm::vec4(transform.position.x, -transform.position.y, transform.position.z, 1.0f);
	return modelMatrix;
}

// just so i can close this part of code.
namespace WEngine
{
	void RenderHandler::InitSDL()
	{
		SDL_InitFlags initFlags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD;
		SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "1");

		if (!SDL_Init(initFlags))
		{
			WLog::SetConsoleError();
			WLog::ConsoleLog(std::format("SDL Initialisation failed: {}", SDL_GetError()));
			abort();
		}

		SDL_DisplayID display = SDL_GetPrimaryDisplay();
		m_displayMode = const_cast<SDL_DisplayMode*>(SDL_GetCurrentDisplayMode(display));

		Uint32 windowFlags = SDL_WINDOW_BORDERLESS;
	#if GPU_BACKEND == GPU_VULKAN
		windowFlags |= SDL_WINDOW_VULKAN;
	#endif


		if (m_displayMode != nullptr)
		{
			m_windowResolution.x = m_displayMode->w;
			m_windowResolution.y = m_displayMode->h;
		}
		else
		{
			WLog::SetConsoleWarning();
			WLog::ConsoleLog(std::format("Couldnt grab display mode, defaulting back to 800x600: {}", SDL_GetError()));
			m_windowResolution.x = 800;
			m_windowResolution.y = 600;
		}

		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, EngineSettings::engineName.c_str());
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 0);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 0);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, m_windowResolution.x);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, m_windowResolution.y);
		// As in the migration guide, this isnt optimal, but its ok
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, windowFlags);

		m_window = SDL_CreateWindowWithProperties(props);

		if (m_window == nullptr)
		{
			WLog::SetConsoleError();
			WLog::ConsoleLog("Window couldnt be opened");
			abort();
		}
		WLog::ConsoleLog(std::format("Window opened at resolution {}x{}", m_windowResolution.x, m_windowResolution.y));

		Haptic::Init(m_window);
	}
	void RenderHandler::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.IniFilename = nullptr;

		ImGui::StyleColorsDark();

		//Iris::SETTING_ConfigureImGui(m_window);

		ImGuiStyle& style = ImGui::GetStyle();

		style.FrameRounding = 4;
		style.WindowBorderSize = 0;
		style.GrabMinSize = 8;
		style.ScrollbarSize = 8;
		style.WindowRounding = 0;
		style.WindowTitleAlign = { 0.5f, 0.5f };

		ImPlotStyle& ipStyle = ImPlot::GetStyle();

		ipStyle.PlotDefaultSize = {200, 200};

		ImPlot::StyleColorsDark();
		auto colors = style.Colors;

		colors[ImGuiCol_Border] = ImVec4(0.50f, 0.43f, 0.43f, 0.50f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.48f, 0.16f, 0.16f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.98f, 0.26f, 0.26f, 0.67f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.98f, 0.26f, 0.26f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.98f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.98f, 0.26f, 0.26f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.10f, 0.10f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.98f, 0.26f, 0.26f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.98f, 0.26f, 0.26f, 0.95f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.80f);
		colors[ImGuiCol_Tab] = ImVec4(0.58f, 0.18f, 0.18f, 0.86f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.68f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.15f, 0.07f, 0.07f, 0.97f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.42f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.35f, 0.58f, 1.00f, 1.00f);


	}
}

