#pragma once

// This file does not contain LLM generated documentation

#include <queue>
#include <SDL3/SDL.h>

#include "Engine/Math/Vector.h"
#include "Engine/Types/Rendering/InstanceData.h"
#include "Engine/Types/Rendering/LightingInfo.h"
#include "Engine/Types/Rendering/RenderMission.h"
#include "Engine/Types/Rendering/Iris/Handles.h"
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
		//Framebuffer EditorGetViewportFramebuffer();

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


		void UpdateCamera(const Transform& trans);
		void UpdateCamera(const Vector3& position, const Quaternion& rotation);
		void UpdateCameraColor(const Color& backColor);

		/**
		 * Records a given render mission to the queue.
		 * @param mission Mission to be recorded.
		 */
		void AddToRenderQueue(RenderMission& mission);
		void AddPlanToRenderQueue(RenderPlan& plan);

		void RegisterTexture(Iris::TextureHandle handle);

	private:
		void PrepareRenderingSetup();
		void LoadShaders();
		void CreateTables();
		void CreatePipelines();

		void RenderSingleMission(const RenderMission& mission, const glm::mat4& vp);
		void RenderSinglePlan(const RenderPlan& plan, const Mat4x4& vp);

		Mat4x4 CalcModelMatrix(const Transform& transform);
		glm::mat4 CalcModelMatrixGLM(const Transform& transform);

		void SortMissions(bool transparentPass);
		void CleanSortedMissions();

		void InitSDL();
		void InitImGui();


	private:
		// while it was comfy to use the old shader system, a static one makes
		// more sense considering the style were going after.
		Iris::ShaderHandle m_vertexShader; // this is in this case the basic one.
		Iris::ShaderHandle m_statVertexShader; // this is in this case the basic one.
		Iris::ShaderHandle m_fragmentShader; // this is in this case the basic one.
		Iris::ResourceTableLayoutHandle m_mainTableLayout;
		Iris::GraphicsPipelineHandle m_basicPipeline;
		Iris::GraphicsPipelineHandle m_statPipeline;
		Iris::SamplerHandle m_sampler;

		wtl::vector<Iris::ResourceTableHandle> m_textureTables;
		uint64 m_currentBoundTexture = 0;

		Vector2 m_windowResolution;
		SDL_DisplayMode* m_displayMode = nullptr;
		SDL_Window* m_window = nullptr;

		Transform m_camera = Transform::Zero;
		Color m_camColor = Color::Black;

		wtl::deque<RenderMission> m_renderQueue;
		wtl::deque<RenderPlan> m_renderPlanQueue;

		glm::mat4 m_projection;
		glm::mat4 m_viewMatrix;

		bool m_isEditor = false;
		Vector2 m_viewportResolution{};
	};
}
