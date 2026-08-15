#pragma once
#include <Engine/Types/AssetMission.h>

#include <unordered_map>
#include <string>
#include <Engine/Types/Refcounted.h>

#include "Engine/Types/Rendering/Iris/Handles.h"
#include "Engine/Types/Rendering/Iris/IrisAssetComms.h"

namespace WEngine
{
	struct TextureInfo;
	/**
	 * AssetRepo handles the loading and unloading of game assets such as sprites, shaders, YAML files, atlas info, audio clips, and UI sheets.
	 */
	class AssetRepo
	{
		struct ASMFHeader
		{
			char identifier[4];
			uint64 vertCount;
			uint64 indCount;
		};
	public:
		/**
		 * Constructs an AssetRepo object and initializes the data path.
		 */
		AssetRepo();
		~AssetRepo() = default;
	private:
		std::string m_dataPath;
		std::unordered_map<std::string, AudioClip> m_audioRepo;
		std::unordered_map<std::string, Ref<uint64>> m_textureRepo;

		Iris::BufferHandle m_vertexBuffer;
		Iris::BufferHandle m_indexBuffer;

		// These should be fine-tuned in the final optimization pass of the game long after the content lock.
		_GLOBAL_CEX_ sizeT TexturesPerUpload_XS = 64;
		_GLOBAL_CEX_ sizeT TexturesPerUpload_S = 64;
		_GLOBAL_CEX_ sizeT TexturesPerUpload_M = 64;
		_GLOBAL_CEX_ sizeT TexturesPerUpload_L = 16;
		_GLOBAL_CEX_ sizeT TexturesPerUpload_X = 4;
		std::array<Iris::BufferHandle, TexturesPerUpload_XS> m_transferBuffers_XS;	// for 128 or lower
		std::array<Iris::BufferHandle, TexturesPerUpload_S> m_transferBuffers_S;	// for 256 or lower
		std::array<Iris::BufferHandle, TexturesPerUpload_M> m_transferBuffers_M;	// for 512 or lower
		std::array<Iris::BufferHandle, TexturesPerUpload_L> m_transferBuffers_L;	// for 1024 or lower
		std::array<Iris::BufferHandle, TexturesPerUpload_X> m_transferBuffers_X;	// for 2048 or lower
		wtl::vector<std::pair<TextureInfoDDS, Iris::TextureHandle>> m_textures;
		wtl::vector<bool> m_texturesDone;

	public:
		void LoadAllGPUAssets();
		void TickTextureUpload();
		/**
		 * Gets the asset specified by the mission parameter.
		 * @tparam T The type of asset mission to handle (e.g., SpriteAssetMission, ShaderAssetMission, etc.).
		 * @param mission A reference to the asset mission object containing information about the requested asset.
		 */
		template<class T = AssetMissionBase>
		void GetAsset(T& mission);
		/**
		 * Gets the data path where assets are stored.
		 * @return A string containing the data path.
		 */
		std::string GetDataPath() const { return m_dataPath; }

	private:
		TextureInfo LoadTexturePNG(const std::string& path);
		TextureInfoDDS LoadTextureDDS(const std::string& path);
		AudioClip* LoadAudioWAV(const std::string& name);
		std::string LoadTextFile(const std::string& path);

		void LoadSpirVFromGlsl(SpirVAssetMission& mission);
		void LoadSpirVFromSpv(SpirVAssetMission& mission);

		// ----- GPU Preloading -----
		void PrepareTransferBuffers();
		bool CheckForPackages();
		void ParsePackageTable(wtl::vector<std::pair<sizeT, sizeT>>& container, const std::string& tableName);
		void ExtractPackage(const wtl::vector<std::pair<sizeT, sizeT>>& locations, wtl::vector<byte*>& files,
			const std::string& package);
		ASMFHeader ReadASMFHeader(const byte* data);
		void ParseAndUploadMeshes(const wtl::vector<byte*>& meshFiles);
		void ParseTextures(const wtl::vector<byte*>& texFiles);
	};
};

