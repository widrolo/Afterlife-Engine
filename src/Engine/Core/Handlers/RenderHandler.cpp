#include "RenderHandler.h"
#include <iostream>
#include <queue>

#include <Engine/Util/Log.h>
#include <Engine/Components/Rendering/CameraComponent.h>
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
#include "glm/gtc/quaternion.hpp"


using namespace WEngine;

Iris::CommandBufferHandle tempHandle;
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

	CoreSystems::GetAssetRepo()->LoadAllGPUAssets();

	m_projection = glm::perspective(
		glm::radians(90.0f),
		m_windowResolution.x / m_windowResolution.y,
		0.01f,
		1000.0f
		);

	tempHandle = Iris::CreateCommandBuffer(Iris::QueueType::Graphics);
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

Framebuffer RenderHandler::EditorGetViewportFramebuffer()
{
	return m_viewportFb;
}


void RenderHandler::BeginFrame()
{
	Iris::BeginFrame();
	Iris::AcquireSwapchainTexture();
	Iris::ImGuiNewFrame();
	Iris::BeginCommandBuffer(tempHandle);

	Iris::RenderPassBeginDesc desc{};
	desc.colorAttachment.clearColor = m_camera->GetBackColor();
	Iris::BeginRenderPass(tempHandle, desc);

	Iris::Viewport vp{};
	vp.extent = {1920.0f, 1080.0f};
	Iris::SetViewport(tempHandle, vp);
}

void RenderHandler::RenderFrame()
{

	//SortMissions(false);
	//SortMissions(true);
	for (auto& materialGroup : m_sortedMissions)
	{
		for (auto& modelGroup : materialGroup.models)
		{
			RenderModelGroup(modelGroup, materialGroup.groupID);
		}
	}

	Mat4x4 vp = Glm4x4ToMat4x4(m_projection * m_viewMatrix);

	Iris::BindGraphicsPipeline(tempHandle, m_mainPipeline);

	for (const auto& mission : m_renderQueue)
		RenderSingleMission(mission, vp);

	if (!Engine::GetCla().testMode)
	{
		// temp until we have proper drawing again
		if (m_isEditor)
		{
			Iris::ImGuiRenderDrawData(tempHandle);
		}
		else
		{
			Iris::ImGuiRenderDrawData(tempHandle);
		}
	}
	else
	{
		Iris::ImGuiRenderDrawData(tempHandle);
	}

	Iris::EndRenderPass(tempHandle);
	Iris::EndCommandBuffer(tempHandle);
	Iris::SubmitCommandBuffer(tempHandle);
	Iris::Present();
	m_renderQueue.clear();
	CleanSortedMissions();
	Iris::EndFrame();
}

void RenderHandler::RenderSingleMission(const RenderMission& mission, const Mat4x4& vp)
{
	MeshAssetMission meshMission{};
	meshMission.uid = mission.meshUID;
	CoreSystems::GetAssetRepo()->GetAsset(meshMission);

	sizeT indexSize = sizeof(uint32) * 3; // cause 3 indexes per triangle (i think)
	sizeT vertexSize = sizeof(float32) * 3 + sizeof(float32) * 3 + sizeof(float32) * 2;

	sizeT indexCount = meshMission.model.indexSize / indexSize;
	sizeT indexOffset = meshMission.model.indexOffset / indexSize;
	sizeT vertOffset = meshMission.model.vertexOffset / vertexSize;

	wtl::vector<Iris::BufferHandle> vertBuffs{CoreSystems::GetAssetRepo()->GetVertexBuffer()};
	wtl::vector<sizeT> vertOffs{0};
	Iris::BindVertexBuffers(tempHandle, 0, vertBuffs,vertOffs);
	Iris::BindIndexBuffer(tempHandle, CoreSystems::GetAssetRepo()->GetIndexBuffer(), 0);
	Iris::BindResourceTable(tempHandle, m_mainPipeline, 0, m_textureTables[mission.textureUID]);
	Iris::DrawIndexed(tempHandle, indexCount, 1, indexOffset, vertOffset, 0);
}

void RenderHandler::RegisterCamera(CameraComponent *camera)
{
	m_camera = camera;
}

void RenderHandler::AddToRenderQueue(RenderMission& mission)
{
	if (mission.meshUID == 0 || mission.textureUID == 0)
		return;
	m_renderQueue.push_back(mission);
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
}

void RenderHandler::LoadShaders()
{
	// I just realized that this kills the purpose of Iris but we can revisit this later,
	SpirVAssetMission mission{};
	mission.name = "basic";
	mission.shaderType = SpirVAssetMission::VertexShader;
	CoreSystems::GetAssetRepo()->GetAsset(mission);

	Iris::ShaderStageDesc desc{};
	desc.debugName = "Main Vertex Shader";
	desc.stage = Iris::ShaderStage::Vertex;
	desc.entryPoint = "main";
	desc.bytecode = mission.shaderCode;
	desc.bytecodeSize = mission.shaderSize;
	m_vertexShader = Iris::CreateShader(desc);


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

	Iris::GraphicsPipelineDesc desc{};
	desc.debugName = "Main Pipeline";
	desc.vertexShader = m_vertexShader;
	desc.fragmentShader = m_fragmentShader;
	desc.vertexLayout = vertDesc;
	desc.rasterizer = Iris::RasterizerDesc{}; // default one is good enough
	desc.depthStencil = Iris::DepthStencilDesc{}; // default one is good enough
	desc.blend = Iris::BlendDesc{}; // default one is good enough

	desc.tableLayouts[0] = m_mainTableLayout;
	desc.tableAttachmentCount = 1;

	desc.pushConstantsSize = sizeof(Mat4x4);

	m_mainPipeline = Iris::CreateGraphicsPipeline(desc);
}

Mat4x4 RenderHandler::CalcModelMatrix(const Transform &transform)
{
	glm::quat q(transform.rotation.w, transform.rotation.x,
				transform.rotation.y, transform.rotation.z);

	glm::mat4 modelMatrix = glm::mat4_cast(q);

	modelMatrix[0] *= transform.size.x;
	modelMatrix[1] *= transform.size.y;
	modelMatrix[2] *= transform.size.z;

	modelMatrix[3] = glm::vec4(transform.position.x, -transform.position.y, transform.position.z, 1.0f);

	return Glm4x4ToMat4x4(modelMatrix);
}

void RenderHandler::RenderModelGroup(const ModelGroup &group, Material material)
{
	wtl::vector<InstanceData> instances(group.missions.size());

	for (sizeT i = 0; i < group.missions.size(); i++)
	{
		Mat4x4 model = CalcModelMatrix(group.missions[i].transform);
		instances[i] = {model};
	}

	Mat4x4 vp = Glm4x4ToMat4x4(m_projection * m_viewMatrix);

	//Iris::DRAWCALL_DrawModelInstanced(group.groupID, material, vp, instances);
}


void RenderHandler::InsertModelIntoShaderGroup(RenderMission &mission, MaterialGroup &materialGroup)
{
	bool foundModel = false;
	for (sizeT i = 0; i < materialGroup.models.size(); ++i)
	{
		if (materialGroup.models[i].groupID == mission.meshUID)
		{
			foundModel = true;
			materialGroup.models[i].missions.push_back(mission);
			break;
		}
	}
	if (!foundModel)
	{
		ModelGroup group;
		group.groupID = mission.meshUID;
		group.missions.push_back(mission);
		materialGroup.models.push_back(group);
	}
}

/*
 * As clarification because the rationale might not be obvious: we need to draw opaque objects first.
 * That is because every transparent object also writes to the stencil buffer, so any opaque object that
 * would have been drawn after does not get drawn.
 */
void RenderHandler::SortMissions(bool transparentPass)
{
	for (auto& mission : m_renderQueue)
	{
		//if (transparentPass)
		//{
		//	auto def = Iris::GetShaderDef(mission.material);
		//	if (!def.HasValue())
		//		continue;
		//	if (!def.GetValue().transparent)
		//		continue;
		//}
		//if (!transparentPass)
		//{
		//	auto def = Iris::GetShaderDef(mission.material);
		//	if (!def.HasValue())
		//		continue;
		//	if (def.GetValue().transparent)
		//		continue;
		//}
		if (mission.isStationary)
		{
			//SortStationary(mission);
			continue;
		}
		bool foundShader = false;
		for (auto& m_sortedMission : m_sortedMissions)
		{
			if (m_sortedMission.groupID == mission.textureUID)
			{
				foundShader = true;
				InsertModelIntoShaderGroup(mission, m_sortedMission);
				continue;
			}
		}
		if (!foundShader)
		{
			MaterialGroup group;
			group.groupID = mission.textureUID;
			InsertModelIntoShaderGroup(mission, group);
			m_sortedMissions.push_back(group);
		}
	}
}

void RenderHandler::CleanSortedMissions()
{
	for (auto& shaderGroup : m_sortedMissions)
	{
		for (auto& modelGroup : shaderGroup.models)
		{
			modelGroup.missions.clear();
		}
		shaderGroup.models.clear();
	}
	m_sortedMissions.clear();
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

