#include "SteamStore.h"

#include <Engine/Util/Log.h>

#include <Engine/Util/Env.h>
#include <sstream>
#include <iostream>

#include "Engine/Core/System/Memory.h"

using namespace WEngine;

const char* overlayWindowsStr[] = { "friends", "community", "players", "settings", "officialgamegroup", "stats", "achievements" };

// no idea what this is doing
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

SteamStore::SteamStore() 
{
#if STEAM
	if (!SteamAPI_Init())
	{
		WLog::SetConsoleError();
		WLog::ConsoleLog("Steam API Init failed!");
		return;
	}

	SteamInput()->Init(true);

	m_initSuccess = true;

	m_controllers = wNewArr(InputHandle_t, STEAM_INPUT_MAX_COUNT);
	
	m_steamID = SteamUser()->GetSteamID();
	m_steamID64 = m_steamID.ConvertToUint64();
	//m_steamWebAPIKey = GetEnv("STEAM_WEB_API_KEY"); // Youll have to put your own in the env variable
#endif // STEAM
}

SteamStore::~SteamStore()
{
#if STEAM
	SteamAPI_Shutdown();
	SteamInput()->Shutdown();
#endif // STEAM
}

void SteamStore::FetchInput()
{
	SteamInput()->RunFrame();
	m_numControllers = SteamInput()->GetConnectedControllers(m_controllers);
	if (m_numControllers != 0 && m_controllers != nullptr)
	{
		m_mainController = m_controllers[0];
		m_isControllerConnected = true;
	}
	else
	{
		m_isControllerConnected = false;
	}
}

std::string SteamStore::GetSteamAccountName()
{
#if STEAM
	return SteamFriends()->GetPersonaName();
#else
	return "STEAM DISABLED!";
#endif // STEAM
}

void SteamStore::DispatchCallbacks()
{
#if STEAM
	SteamAPI_RunCallbacks();
#endif // STEAM
}

void SteamStore::UnlockAchievement(std::string achievementApiName)
{
#if STEAM
	
	if (!m_initSuccess)
		return;

	SteamUserStats()->SetAchievement(achievementApiName.c_str());

	if (!SteamUserStats()->StoreStats())
	{
		WLog::SetConsoleWarning();
		WLog::ConsoleLog(std::format("Achievement {} could not be unlocked!", achievementApiName));
		return;
	}

	WLog::SetConsoleSuccess();
	WLog::ConsoleLog(std::format("Achievement {} unlocked!", achievementApiName));

#endif // STEAM
}

void SteamStore::OpenOverlay(OverlayWindows overlay)
{
#if STEAM
	SteamFriends()->ActivateGameOverlay(overlayWindowsStr[overlay]);
#endif // STEAM
}

void SteamStore::OpenControllerBinds()
{
	if (!m_isControllerConnected)
		return;
	SteamInput()->ShowBindingPanel(m_mainController);
}


// For copy and paste
#if STEAM
#endif // STEAM