#include "AssetRepo.h"

#include <Engine/Util/Log.h>
#include <Engine/EngineDefines.h>
#include <Engine/Core/System/OS.h>
#include <fstream>
#include <sstream>

#include <Engine/gl/include/stb_image.h>
#define TINYGLTF_IMPLEMENTATION
#include <chrono>
#include <filesystem>
#include <Engine/gl/gltf/tiny_gltf_v3.h>
#include <yaml-cpp/yaml.h>
#include <tinyxml2.h>

#include "Engine/imgui/imgui.h"
#include <shaderc/shaderc.hpp>

#include "RNGHandler.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/TextureInfo.h"
#include "Engine/Types/Rendering/VertextData.h"
#include "Engine/Types/Rendering/Iris/IrisAssetComms.h"
#include "Engine/Util/TextureSwizzler.h"
#include "Engine/Types/Rendering/DDS.h"


using namespace WEngine;

AssetRepo::AssetRepo()
{
	m_dataPath = OS::GetProcessPath();
	WLog::ConsoleLog(std::format("Data path:\n\t{}", m_dataPath));

	// Remove the executable name
#ifdef WE_Windows
	sizeT found = m_dataPath.find_last_of("\\");
#endif
#ifdef WE_Linux
	sizeT found = m_dataPath.find_last_of('/');
#endif
	if (found != std::string::npos) { 
		m_dataPath = m_dataPath.substr(0, found + 1); 
	}

	m_dataPath += EngineSettings::dataPath;

	WLog::SetConsoleInfo();
	WLog::ConsoleLog(std::format("Data path:\n\t{}", m_dataPath));
}

template<>
void AssetRepo::GetAsset<ShaderAssetMission>(ShaderAssetMission& mission) 
{
	mission.vertexShaderSource = LoadTextFile(GetDataPath() + EngineSettings::shaderPath + mission.name + "Vertex.glsl");
	mission.fragmentShaderSource = LoadTextFile(GetDataPath() + EngineSettings::shaderPath + mission.name + "Fragment.glsl");
}

template<>
void AssetRepo::GetAsset<YamlAssetMission>(YamlAssetMission& mission)
{
	const std::string file = LoadTextFile(GetDataPath() + EngineSettings::sectorPath + mission.name + ".yaml");
	mission.root = YAML::Load(file);
}

template<>
void AssetRepo::GetAsset<AudioClipAssetMission>(AudioClipAssetMission& mission)
{
	AudioClip* clip = &m_audioRepo[mission.name];
	if (clip->audioBuf == nullptr)
		clip = LoadAudioWAV(mission.name);
	mission.clip = clip;
}

template<>
void AssetRepo::GetAsset<UISheetAssetMission>(UISheetAssetMission& mission)
{
	const std::string file = LoadTextFile(GetDataPath() + EngineSettings::uiSheetPath + mission.name + ".yaml");
	auto descriptor = YAML::Load(file);

	if (!descriptor["uisheet"])
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("{} descriptor does not contain Node \"uisheet\"", mission.name));
	}

	auto sheet = descriptor["uisheet"];

	if (!sheet["document"])
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("{} descriptor does not contain Node \"document\"", mission.name));
	}

	std::string docPath = GetDataPath() + EngineSettings::uiSheetPath + "Documents/" + sheet["document"].as<std::string>() + ".uidoc";
	mission.document.LoadFile(docPath.c_str());
	if (mission.document.ErrorID() != 0)
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("Error while reading UI Sheet document, Line {}:\n{}", mission.document.ErrorLineNum(), mission.document.ErrorStr()));
	}
}

template<>
void AssetRepo::GetAsset<SpirVAssetMission>(SpirVAssetMission& mission)
{
#ifdef PACKAGE
	//LoadSpirVFromSpv(mission);
	LoadSpirVFromGlsl(mission);
#else
	LoadSpirVFromGlsl(mission);
#endif

}



TextureInfo AssetRepo::LoadTexturePNG(const std::string& path)
{
	TextureInfo info{};
	info.data = stbi_load(path.c_str(), &info.width, &info.height, &info.channels, 4);
	if (info.data == nullptr)
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("Failed to load texture {}", path));
		return TextureInfo{};
	}
	sizeT size = info.width * info.height * 4;
	WAllocator::ReportExternalAllocation(size);
	WLog::SetConsoleSuccess();
	WLog::ConsoleLog(std::format("Loaded texture {}", path));
	return info;
}

TextureInfoDDS AssetRepo::LoadTextureDDS(const std::string &path)
{
	DDSFile dds = LoadDDS(path);
	if (dds.data.empty())
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("Failed to load texture {}", path));
		return {};
	}

	sizeT size = dds.data.size();
	byte* raw   = new byte[size];
	std::memcpy(raw, dds.data.data(), size);
	WAllocator::ReportExternalAllocation(size);

	TextureInfoDDS info{};
	info.data     = raw;
	info.width    = dds.width;
	info.height   = dds.height;
	info.mipCount = dds.mips;
	info.format   = dds.format;

	WLog::SetConsoleSuccess();
	WLog::ConsoleLog(std::format("Loaded texture {}", path));
	return info;
}

AudioClip* AssetRepo::LoadAudioWAV(const std::string& name)
{
	AudioClip clip{};
	SDL_AudioSpec spec;

	const std::string path = GetDataPath() + EngineSettings::audioPath + name + ".wav";

	bool res = SDL_LoadWAV(path.c_str(), &spec, &clip.audioBuf, &clip.audioLen);

	if (!res || spec.freq == 0)
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("Failed to load audio clip:\n\t{}", path));
		return nullptr;
	}

	clip.format = spec.format;
	clip.freq = spec.freq;
	clip.channels = spec.channels;

	m_audioRepo[name] = clip;
	return &m_audioRepo[name];
}

std::string AssetRepo::LoadTextFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open()) 
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("Failed to open file:\n\t{}", path));
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}

void AssetRepo::LoadSpirVFromGlsl(SpirVAssetMission &mission)
{
	std::string path = GetDataPath() + EngineSettings::shaderPath + mission.name;
	shaderc_shader_kind kind = shaderc_glsl_infer_from_source; // just to shut up the compiler.
	switch (mission.shaderType)
	{
		case SpirVAssetMission::VertexShader:
			path += "Vertex";
			kind = shaderc_glsl_vertex_shader;
			break;
		case SpirVAssetMission::FragmentShader:
			path += "Fragment";
			kind = shaderc_glsl_fragment_shader;
			break;
		case SpirVAssetMission::GeometryShader:
			path += "Geometry";
			kind = shaderc_glsl_geometry_shader;
			break;
	}

	path += ".glsl";

	auto t0 = std::chrono::high_resolution_clock::now();
	auto shaderCode = LoadTextFile(path);

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

#ifdef DEBUG
	options.SetOptimizationLevel(shaderc_optimization_level_zero);
	options.SetGenerateDebugInfo();
#else
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
#endif
	auto res = compiler.CompileGlslToSpv(
		shaderCode,
		kind,
		"what.txt",
		options
	);

	if (res.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("Failed to compile shader from GLSL to Spir-V:\n\t{}\n\t{}", path, res.GetErrorMessage()));
		return;
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	WLog::ConsoleLog(std::format("Shader Compilation time: {}ms.",
		std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count()));

	sizeT wordCount = res.cend() - res.cbegin();
	mission.shaderCode = wNewArr(uint32, wordCount);
	std::copy(res.cbegin(), res.cend(), mission.shaderCode);
	mission.shaderSize = wordCount * sizeof(uint32);
}

void AssetRepo::LoadSpirVFromSpv(SpirVAssetMission &mission)
{
	std::string path = GetDataPath() + EngineSettings::shaderPath + mission.name;
	switch (mission.shaderType)
	{
		case SpirVAssetMission::VertexShader:
			path += "Vertex";
			break;
		case SpirVAssetMission::FragmentShader:
			path += "Fragment";
			break;
		case SpirVAssetMission::GeometryShader:
			path += "Geometry";
			break;
	}
	path += ".spv";
	std::ifstream file(path, std::ios::binary | std::ios::in | std::ios::ate);

	if (!file.is_open())
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog(std::format("Failed to open Spir-V shader:\n\t{}", path));
		mission.shaderSize = 0;
		return;
	}

	mission.shaderSize = file.tellg();
	file.seekg(0, std::ios::beg);

	mission.shaderCode = new uint32[mission.shaderSize / sizeof(uint32)];
	file.read(reinterpret_cast<char*>(mission.shaderCode), mission.shaderSize);
	file.close();
}

void AssetRepo::LoadAllGPUAssets()
{
	static bool everRan = false;
	if (everRan)
		return;
	everRan = true;

	if (!CheckForPackages())
		return;

	wtl::vector<std::pair<sizeT, sizeT>> textureTable;
	wtl::vector<std::pair<sizeT, sizeT>> meshTable;

	ParsePackageTable(meshTable, "Meshes.yaml");
	ParsePackageTable(textureTable, "Textures.yaml");

	wtl::vector<byte*> texFiles;
	wtl::vector<byte*> meshFiles;

	ExtractPackage(textureTable, texFiles, "Textures.pkg");
	ExtractPackage(meshTable, meshFiles, "Meshes.pkg");
	ParseAndUploadMeshes(meshFiles);
}

bool AssetRepo::CheckForPackages()
{
	std::string packPath = GetDataPath() + "/Packages/";
	std::string texPack = packPath + "Textures.pkg";
	std::string texPackTable = packPath + "Textures.yaml";
	std::string meshPack = packPath + "Meshes.pkg";
	std::string meshPackTable = packPath + "Meshes.yaml";

	if (!std::filesystem::exists(texPack))
	{
		WLog::SetConsoleWarning();
		WLog::ConsoleLog("Texture package not found");
		return false;
	}
	if (!std::filesystem::exists(texPackTable))
	{
		WLog::SetConsoleWarning();
		WLog::ConsoleLog("Texture table not found");
		return false;
	}
	if (!std::filesystem::exists(meshPack))
	{
		WLog::SetConsoleWarning();
		WLog::ConsoleLog("Mesh package not found");
		return false;
	}
	if (!std::filesystem::exists(meshPackTable))
	{
		WLog::SetConsoleWarning();
		WLog::ConsoleLog("Mesh table not found");
		return false;
	}
	return true;
}

void AssetRepo::ParsePackageTable(wtl::vector<std::pair<sizeT, sizeT>>& container, const std::string& tableName)
{
	std::string tablePath = GetDataPath() + "/Packages/" + tableName;
	std::string table = LoadTextFile(tablePath);

	YAML::Node root = YAML::Load(table);

	if (!root["Table"])
	{
		WLog::SetConsoleWarning();
		WLog::ConsoleLog(std::format("Package table {} is missing Table definition", tableName));
		return;
	}

	for (const auto& entry : root["Table"])
	{
		const YAML::Node& location = entry.second;

		std::pair<sizeT, sizeT> res;
		res.first = location["Offset"].as<sizeT>();
		res.second = location["Size"].as<sizeT>();
		container.push_back(res);
	}
}

void AssetRepo::ExtractPackage(const wtl::vector<std::pair<sizeT, sizeT>>& locations, wtl::vector<byte*>& files,
	const std::string& package)
{
	files.reserve(locations.size());

	std::string packPath = GetDataPath() + "/Packages/" + package;
	std::ifstream pack(packPath, std::ios::binary);
	if (!pack)
	{
		WLog::SetConsoleWarning();
		WLog::ConsoleLog(std::format("Cant load package, unable to open {}!", package));
		return;
	}

	// The package table has been validated a bajillion times on export. if we have an unexpected EOF,
	// then its the fault of the user.
	for (const auto& loc : locations) // first is offset, second is size. both in bytes
	{
		char* file = wNewArr(char, loc.second);
		files.push_back((byte*)file);

		pack.clear();
		pack.seekg(loc.first, std::ios::beg);
		pack.read(file, loc.second);
		std::streamsize got = pack.gcount();
		if (got < loc.second)
		{
			WLog::SetConsoleWarning();
			WLog::ConsoleLog("Something went wrong while unpacking. Verify game files on steam.");
		}
	}
}

AssetRepo::ASMFHeader AssetRepo::ReadASMFHeader(const byte *data)
{
	ASMFHeader h{};
	std::memcpy(h.identifier, data + 0, 4);
	std::memcpy(&h.vertCount, data + 4, 8);
	std::memcpy(&h.indCount,  data + 12, 8);
	return h;
}

void AssetRepo::ParseAndUploadMeshes(const wtl::vector<byte*>& meshFiles)
{
	wtl::vector<ASMFHeader> meshFileHeaders;
	meshFileHeaders.reserve(meshFiles.size());

	const sizeT headerSize = 20;
	const sizeT vertSize = 32;
	const sizeT indexSize = 4;
	sizeT totalVertCount = 0;
	sizeT totalIndCount = 0;
	for (const auto* mesh : meshFiles)
	{
		ASMFHeader header = ReadASMFHeader(mesh);
		meshFileHeaders.push_back(header);
		totalVertCount += header.vertCount;
		totalIndCount += header.indCount;
	}

	// the idea is: we combine all meshes into one payload that we send to the GPU.
	byte* vertexPayload = wNewArr(byte, totalVertCount * vertSize);
	byte* indexPayload = wNewArr(byte, totalIndCount * indexSize);
	sizeT vertHead = 0;
	sizeT indHead = 0;

	for (sizeT i = 0; i < meshFiles.size(); i++)
	{
		std::memcpy(vertexPayload + vertHead, meshFiles[i] + headerSize, meshFileHeaders[i].vertCount * vertSize);
		sizeT indexOffset = meshFileHeaders[i].vertCount * vertSize + headerSize;
		std::memcpy(indexPayload + indHead, meshFiles[i] + indexOffset, meshFileHeaders[i].indCount * indexSize);
		vertHead += meshFileHeaders[i].vertCount * vertSize;
		indHead += meshFileHeaders[i].indCount * indexSize;
	}

	for (auto* mesh : meshFiles)
		wFree(mesh);

	Iris::BufferDesc desc{};
	desc.debugName = "Mesh Vertex Payload";
	desc.size = vertHead;
	desc.usage = Iris::BufferUsage::Vertex;
	vertexBuffer = Iris::CreateBuffer(desc, vertexPayload, vertHead);
	desc.debugName = "Mesh Index Payload";
	desc.size = indHead;
	desc.usage = Iris::BufferUsage::Index;
	indexBuffer = Iris::CreateBuffer(desc, indexPayload, indHead);

	wFree(vertexPayload);
	wFree(indexPayload);
}
