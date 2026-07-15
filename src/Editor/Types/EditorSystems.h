#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WEngine
{
	class AssetRepo;
	class RenderHandler;
}
namespace WEditor
{

	class Editor;
	class EditorUIHandler;
	class MenubarHandler;
	class CompSettingsRepo;

	struct EditorSystems
	{
		friend Editor;
	private:
		_GLOBAL_ WEngine::RenderHandler* renderHandler;
		_GLOBAL_ WEngine::AssetRepo* assetRepo;
		_GLOBAL_ EditorUIHandler* editorUIHandler;
		_GLOBAL_ MenubarHandler* menubarHandler;
		_GLOBAL_ CompSettingsRepo* compSettingsRepo;

		_GLOBAL_ bool* isEditorRunning;

		_GLOBAL_ float32 dt;

	public:
		static WEngine::RenderHandler* GetRenderHandler() { return renderHandler; }
		static WEngine::AssetRepo* GetAssetRepo() { return assetRepo; }
		static EditorUIHandler* GetEditorUIHandler() { return editorUIHandler; }
		static MenubarHandler* GetMenubarHandler() { return menubarHandler; }
		static CompSettingsRepo* GetCompSettingsRepo() { return compSettingsRepo; }
		static float32 GetDt() { return dt; }


		/**
	 	 * Check if the game is running.
	 	 * @return Pointer to a boolean indicating whether the game is running.
	 	 */
		static bool* IsEditorRunning() { return isEditorRunning; }
		/**
		 * Shutdown the game by setting the game running flag to false.
		 */
		static void ShutdownEditor() { *isEditorRunning = false; }
	};
}