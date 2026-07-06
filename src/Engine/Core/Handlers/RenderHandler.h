#pragma once

// This file does not contain LLM generated documentation

#include <queue>
#include <SDL3/SDL.h>

#include "Engine/Math/Vector.h"
#include "Engine/Types/Rendering/InstanceData.h"
#include "Engine/Types/Rendering/LightingInfo.h"
#include "Engine/Types/Rendering/RenderMission.h"
#include "Engine/Types/Rendering/GPU/Framebuffer.h"
#include "Engine/Types/Rendering/GPU/Material.h"
#include "Engine/Types/Rendering/GPU/Shader.h"
#include "Engine/Types/Rendering/GPU/StatBufKey.h"
#include "Engine/WTL/deque.h"

namespace WEngine
{
	class CameraComponent;

	/**
	 * This handler is the middle man between any component that wishes to draw something and Iris. It is directly responsible for Iris getting passed correct values.
	 *
	 * In addition to that, this handler also performs optimizations on the recorded render missions.
	 */
	class RenderHandler
	{
	public:
		RenderHandler();

	private:

		struct ModelGroup
		{
			Model groupID;
			wtl::vector<RenderMission> missions;
		};

		struct MaterialGroup
		{
			Material groupID;
			wtl::vector<ModelGroup> models;
		};

		struct StationaryObjStaged
		{
			StatBufKey key;
			Model model;
			Material material;
			wtl::vector<InstanceData> instData;
		};

		struct StationaryRenderMission
		{
			Model model;
			Material material;
			wtl::vector<StatBufKey> references;
		};

		struct SkyboxInfo
		{
			Model skyModel;
			Material skyMaterial;
		};

	public:
		/**
		 * Changes the behaviour of the render handler to account for the editor.
		 * @param viewportResolution Resolution of the viewport.
		 */
		void EnableEditorMode(const Vector2& viewportResolution);

		/**
		 * Gets the framebuffer that is meant to be used by the viewport.
		 * @return Framebuffer handle of the viewport.
		 * @warning This should only be called by WEDGE.
		 */
		Framebuffer EditorGetViewportFramebuffer();

		/**
		 * Starts the render pass and begins recording render missions.
		 * @warning This should only be called by the game loop.
		 */
		void BeginFrame();

		/**
		 * Finishes render mission collection and renders everything.
		 * @warning This should only be called by the game loop.
		 */
		void RenderFrame();

		/**
		 * Sets the given camera to be primary camera used for rendering the frame.
		 * @param camera Pointer to the camera component.
		 */
		void RegisterCamera(CameraComponent* camera);

		/**
		 * Records a given render mission to the queue.
		 * @param mission Mission to be recorded.
		 */
		void AddToRenderQueue(RenderMission& mission);

		/**
		 * Records a stationary object as a stationary object.
		 * @param key Handle to the sector key.
		 * @param model Handle to the model to be added.
		 * @param material Handle to the material to be added.
		 * @param transform Position of the stationary object.
		 */
		void RecordStationaryAdd(StatBufKey key, Model model, Material material, const Transform& transform);

		/**
		 * Pushes the recorded stationary data to Iris.
		 * @warning This should only be called by the game loop.
		 */
		void PushStationaryData();

		/**
		 * Records new sunlight data.
		 * @param light New sunlight data.
		 * @note This does not push anything to Iris, it records it for later.
		 */
		void SetSunlight(const Sunlight& light);

		/**
		 * Records new ambient light data.
		 * @param light New ambient light data.
		 * @note This does not push anything to Iris, it records it for later.
		 */
		void SetAmbientLight(const AmbientLight& light);

		/**
		 * Records new time of day data.
		 * @param time New time of day data.
		 * @note This does not push anything to Iris, it records it for later.
		 * @note The time is expected to be between 0 and 1. 0 being start of the day, 1 being end of the day.
		 */
		void SetLightTime(float32 time);

		/**
		 * Records the intensity of the sunlight.
		 * @param factor intensity of the sunlight.
		 * @note This value does not get used within any shader whatsoever. I have no idea why it exists.
		 */
		void SetSunlightColorFactor(float32 factor);

		/**
		 * Gets the currently recorded sunlight data.
		 * @return Currently recorded sunlight data.
		 * @note This has no guarantee of actually being the sunlight data used for rendering later, it's the currently recorded value which is subject to change,
		 */
		[[nodiscard]] const Sunlight& GetSunlight() const;

		/**
		 * Gets the currently recorded ambient light data.
		 * @return Currently recorded ambient light data.
		 * @note This has no guarantee of actually being the ambient light data used for rendering later, it's the currently recorded value which is subject to change,
		 */
		[[nodiscard]] const AmbientLight& GetAmbientLight() const;

		/**
		 * Gets the currently recorded time of day data.
		 * @return Currently recorded time of day data.
		 * @note This has no guarantee of actually being the time of day data used for rendering later, it's the currently recorded value which is subject to change,
		 */
		[[nodiscard]] float32 GetLightTime() const;

	private:
		void PreparePPFramebuffers();
		void PrepareSkybox();
		void RenderSkybox();
		void LoadPPShaderSingle(const std::string& name);
		void LoadPPShaders();

		Mat4x4 CalcModelMatrix(const Transform& transform);
		Mat4x4 CalcMVPMatrix(const Transform& transform);

		void RenderSingleMission(RenderMission& mission);
		void RenderModelGroup(const ModelGroup& group, Material material);

		void RenderPostProcessingShaders();

		void SortStationary(RenderMission& mission);

		void InsertModelIntoShaderGroup(RenderMission& mission, MaterialGroup& materialGroup);

		void SortMissions();
		void CleanSortedMissions();

		void InitSDL();
		void InitImGui();

	private:
		Vector2 m_windowResolution;
		SDL_DisplayMode* m_displayMode = nullptr;
		SDL_Window* m_window = nullptr;

		CameraComponent* m_camera = nullptr;

		wtl::deque<RenderMission> m_renderQueue;
		wtl::deque<StationaryObjStaged> m_stationaryAddQueue;

		wtl::vector<MaterialGroup> m_sortedMissions;
		wtl::vector<StationaryRenderMission> m_stationaryMissions;

		glm::mat4 m_projection;
		glm::mat4 m_viewMatrix;

		bool m_isEditor = false;
		Framebuffer m_viewportFb{};
		std::array<Framebuffer, 2> m_ppFramebuffers{};
		uint8 m_currentPPFramebuffer = 0;
		Vector2 m_viewportResolution{};

		SkyboxInfo m_skyboxInfo;
		LightingInfo m_lighting;

		Shader m_screenShader;
		// Make it work first, make a pretty later ahh code
		wtl::vector<Shader> m_ppShaders;
	};
}
