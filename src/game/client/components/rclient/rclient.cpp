#include "rclient.h"

#include "base/str.h"
#include "engine/shared/config.h"
#include "game/client/gameclient.h"
#include <engine/shared/json.h>

CRClient::CRClient()
{
	CRClient::OnReset();
}

void CRClient::OnReset()
{

}

void CRClient::OnInit()
{

}

void CRClient::OnRender()
{
	if(m_pRClientDDstatsTask && m_pRClientDDstatsTask->State() == EHttpState::DONE)
	{
		FinishRclientDDstatsProfile();
		ResetRclientDDstatsProfile();
	}
	if(g_Config.m_RcPlayerClanAutoChange)
		DummyConnectedClan(Client()->DummyConnected());
}

void CRClient::OnConsoleInit()
{
	Console()->Register("rc_find_player_from_ddstats", "s[type]", CFGFLAG_CLIENT, ConFindPlayerFromDdstats, this, "Fetch player from DDstats");
	Console()->Register("rc_find_skin_from_ddstats", "s[type]", CFGFLAG_CLIENT, ConFindSkinFromDdstats, this, "Fetch player's skin from DDstats");
	Console()->Register("rc_copy_skin_from_ddstats", "s[type]", CFGFLAG_CLIENT, ConCopySkinFromDdstats, this, "Fetch and copy player's skin from DDstats");
	Console()->Register("rc_find_skin", "r[player]", CFGFLAG_CLIENT, ConFindSkin, this, "Find skin");
	Console()->Register("rc_copy_skin", "r[player]", CFGFLAG_CLIENT, ConCopySkin, this, "Copy skin");
	Console()->Register("rc_find_player", "r[player]", CFGFLAG_CLIENT, ConFindPlayer, this, "Find Player");
	Console()->Register("rc_copy_color", "r[player]", CFGFLAG_CLIENT, ConCopyColor, this, "Copy Color skin");
	Console()->Register("rc_backup_player_profile", "", CFGFLAG_CLIENT, ConBackupPlayerProfile, this, "Backup player profile");
	Console()->Register("rc_tracker_add", "r[player]", CFGFLAG_CLIENT, ConTrackerAdd, this, "Add player to tracker");
	Console()->Register("rc_tracker_remove", "r[player]", CFGFLAG_CLIENT, ConTrackerRemove, this, "Remove player from tracker");
	Console()->Register("rc_tracker_reset", "", CFGFLAG_CLIENT, ConTrackerReset, this, "Reset tracker");
}

void CRClient::OnMessage(int MsgType, void *pRawMsg)
{

}

void CRClient::OnStateChange(int NewState, int OldState)
{

}

// Need things
static int FindPlayerClientId(CGameClient *pGameClient ,const char *Nickname)
{
	int ClientID = -1;
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(str_comp_nocase(pGameClient->m_aClients[i].m_aName, Nickname) == 0)
		{
			ClientID = i;
			break;
		}
	}
	if(ClientID == -1)
	{
		ClientID = str_toint(Nickname);
	}
	if(ClientID >= 0 && ClientID < MAX_CLIENTS)
		return ClientID;
	else
		return -1;
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif

static void FastPrint(CGameClient *pGameClient, const char *pName, const char *pFmt, ...)
{
	char aBuf[256];
	va_list Args;
	va_start(Args, pFmt);
	str_format_v(aBuf, sizeof(aBuf), pFmt, Args);
	va_end(Args);
	pGameClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, pName, aBuf);
}

static void FastEcho(CGameClient *pGameClient, const char *pFmt, ...)
{
	char aBuf[256];
	va_list Args;
	va_start(Args, pFmt);
	str_format_v(aBuf, sizeof(aBuf), pFmt, Args);
	va_end(Args);
	pGameClient->Echo(aBuf);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

//Dummy clan
void CRClient::DummyConnectedClan(const bool IsDummyConnected)
{
	if(IsDummyConnected && !m_DummyConnectedPrevState)
	{
		m_DummyConnectedPrevState = IsDummyConnected;
		str_copy(g_Config.m_PlayerClan, g_Config.m_RcPlayerClanWithDummy, sizeof(g_Config.m_PlayerClan));
		GameClient()->SendInfo(false);
	}
	else if(!IsDummyConnected && m_DummyConnectedPrevState)
	{
		m_DummyConnectedPrevState = IsDummyConnected;
		str_copy(g_Config.m_PlayerClan, g_Config.m_RcPlayerClanNoDummy, sizeof(g_Config.m_PlayerClan));
		GameClient()->SendInfo(false);
	}
}


// Find/Copy Skin
static std::string TrimRight(const char *aInput)
{
	std::string Result(aInput);
	str_utf8_trim_right(Result.data());
	return Result;
}

void CRClient::ConFindPlayerFromDdstats(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	pThis->m_DDstatsSearchType = 1;
	str_copy(pThis->m_DDstatsSearchNickname, TrimRight(pResult->GetString(0)).c_str(), sizeof(pThis->m_DDstatsSearchNickname));
	pThis->FetchRclientDDstatsProfile();
}

void CRClient::ConFindSkinFromDdstats(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	pThis->m_DDstatsSearchType = 2;
	str_copy(pThis->m_DDstatsSearchNickname, TrimRight(pResult->GetString(0)).c_str(), sizeof(pThis->m_DDstatsSearchNickname));
	pThis->FetchRclientDDstatsProfile();
}

void CRClient::ConCopySkinFromDdstats(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	pThis->m_DDstatsSearchType = 3;
	str_copy(pThis->m_DDstatsSearchNickname, TrimRight(pResult->GetString(0)).c_str(), sizeof(pThis->m_DDstatsSearchNickname));
	pThis->FetchRclientDDstatsProfile();
}

void CRClient::FetchRclientDDstatsProfile()
{
	if(m_pRClientDDstatsTask && !m_pRClientDDstatsTask->Done())
	{
		return;
	}
	char aUrl[256];
	char aEncodedNickname[256];
	EscapeUrl(aEncodedNickname, sizeof(aEncodedNickname), m_DDstatsSearchNickname);
	str_format(aUrl, sizeof(aUrl), "https://ddstats.tw/profile/json?player=%s", aEncodedNickname);
	m_pRClientDDstatsTask = HttpGet(aUrl);
	m_pRClientDDstatsTask->Timeout(CTimeout{20000, 0, 500, 10});
	m_pRClientDDstatsTask->IpResolve(IPRESOLVE::V4);
	Http()->Run(m_pRClientDDstatsTask);
}

static void PrintPlayerInfo(CGameClient *pGameClient , const char *Nickname, const char *Skin, const char *Clan, const int Country, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	FastPrint(pGameClient , "Info","- Nickname: %s", Nickname);
	FastPrint(pGameClient , "Info","- Skin name: %s", Skin);
	FastPrint(pGameClient , "Info","- Clan: %s", Clan);
	FastPrint(pGameClient , "Info","- Country: %d", Country);
	if(CustomColor)
	{
		pGameClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 1");
		FastPrint(pGameClient , "Info","- Body Color: %d", SkinColorBodyint);
		FastPrint(pGameClient , "Info","- Feet Color: %d", SkinColorFeetint);
	}
	else
		pGameClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 0");
}

static void PrintSkinInfo(CGameClient *pGameClient, const char *Skin, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	FastPrint(pGameClient , "Info","- Skin name: %s", Skin);
	if(CustomColor)
	{
		pGameClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 1");
		FastPrint(pGameClient , "Info","- Body Color: %d", SkinColorBodyint);
		FastPrint(pGameClient , "Info","- Feet Color: %d", SkinColorFeetint);
	}
	else
		pGameClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 0");
}

static void PrintColorInfo(CGameClient *pGameClient, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	if(CustomColor)
	{
		pGameClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 1");
		FastPrint(pGameClient , "Info","- Body Color: %d", SkinColorBodyint);
		FastPrint(pGameClient , "Info","- Feet Color: %d", SkinColorFeetint);
	}
	else
		pGameClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 0");
}

void CRClient::ApplySkinToPlayer(const char *Skin, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	if(g_Config.m_ClDummy == 1)
	{
		str_copy(DummySkinBeforeCopyPlayer, g_Config.m_ClDummySkin, sizeof(DummySkinBeforeCopyPlayer));
		DummyUseCustomColorBeforeCopyPlayer = g_Config.m_ClDummyUseCustomColor;
		DummyBodyColorBeforeCopyPlayer = g_Config.m_ClDummyColorBody;
		DummyFeetColorBeforeCopyPlayer = g_Config.m_ClDummyColorFeet;
		str_copy(g_Config.m_ClDummySkin, Skin, sizeof(g_Config.m_ClDummySkin));
		g_Config.m_ClDummyUseCustomColor = CustomColor;
		g_Config.m_ClDummyColorBody = SkinColorBodyint;
		g_Config.m_ClDummyColorFeet = SkinColorFeetint;
		GameClient()->SendDummyInfo(false);
	}
	if(g_Config.m_ClDummy == 0)
	{
		str_copy(PlayerSkinBeforeCopyPlayer, g_Config.m_ClPlayerSkin, sizeof(PlayerSkinBeforeCopyPlayer));
		PlayerUseCustomColorBeforeCopyPlayer = g_Config.m_ClPlayerUseCustomColor;
		PlayerBodyColorBeforeCopyPlayer = g_Config.m_ClPlayerColorBody;
		PlayerFeetColorBeforeCopyPlayer = g_Config.m_ClPlayerColorFeet;
		str_copy(g_Config.m_ClPlayerSkin, Skin, sizeof(g_Config.m_ClPlayerSkin));
		g_Config.m_ClPlayerUseCustomColor = CustomColor;
		g_Config.m_ClPlayerColorBody = SkinColorBodyint;
		g_Config.m_ClPlayerColorFeet = SkinColorFeetint;
		GameClient()->SendInfo(false);
	}
}

void CRClient::ApplyColorToPlayer(const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	if(g_Config.m_ClDummy == 1)
	{
		str_copy(PlayerSkinBeforeCopyPlayer, g_Config.m_ClPlayerSkin, sizeof(PlayerSkinBeforeCopyPlayer));
		DummyUseCustomColorBeforeCopyPlayer = g_Config.m_ClDummyUseCustomColor;
		DummyBodyColorBeforeCopyPlayer = g_Config.m_ClDummyColorBody;
		DummyFeetColorBeforeCopyPlayer = g_Config.m_ClDummyColorFeet;
		g_Config.m_ClDummyUseCustomColor = CustomColor;
		g_Config.m_ClDummyColorBody = SkinColorBodyint;
		g_Config.m_ClDummyColorFeet = SkinColorFeetint;
		GameClient()->SendDummyInfo(false);
	}
	if(g_Config.m_ClDummy == 0)
	{
		str_copy(PlayerSkinBeforeCopyPlayer, g_Config.m_ClPlayerSkin, sizeof(PlayerSkinBeforeCopyPlayer));
		PlayerUseCustomColorBeforeCopyPlayer = g_Config.m_ClPlayerUseCustomColor;
		PlayerBodyColorBeforeCopyPlayer = g_Config.m_ClPlayerColorBody;
		PlayerFeetColorBeforeCopyPlayer = g_Config.m_ClPlayerColorFeet;
		g_Config.m_ClPlayerUseCustomColor = CustomColor;
		g_Config.m_ClPlayerColorBody = SkinColorBodyint;
		g_Config.m_ClPlayerColorFeet = SkinColorFeetint;
		GameClient()->SendInfo(false);
	}
}

void CRClient::FinishRclientDDstatsProfile()
{
	json_value *pJson = m_pRClientDDstatsTask->ResultJson();
	if(!pJson)
	{
		GameClient()->Echo("No that player");
		m_DDstatsSearchType = 0;
		return;
	}
	const json_value &Json = *pJson;
	const json_value &Nickname = Json["name"];
	const json_value &Clan = Json["clan"];
	const json_value &Country = Json["country"];
	const json_value &Skin = Json["skin_name"];
	const json_value &SkinColorBody = Json["skin_color_body"];
	const json_value &SkinColorFeet = Json["skin_color_feet"];

	if(Nickname.type == json_string)
	{
		int Countryint = Country.u.integer;
		int SkinColorBodyint = SkinColorBody.u.integer;
		int SkinColorFeetint = SkinColorFeet.u.integer;
		int CustomColor = SkinColorFeetint != 0 || SkinColorBodyint != 0;
		if(m_DDstatsSearchType == 1)
			PrintPlayerInfo(GameClient(), Nickname.u.string.ptr, Skin.u.string.ptr, Clan.u.string.ptr, Countryint, CustomColor, SkinColorBodyint, SkinColorFeetint);
		if(m_DDstatsSearchType == 2)
			PrintSkinInfo(GameClient(), Skin.u.string.ptr, CustomColor, SkinColorBodyint, SkinColorFeetint);
		if(m_DDstatsSearchType == 3)
		{
			PrintSkinInfo(GameClient(), Skin.u.string.ptr, CustomColor, SkinColorBodyint, SkinColorFeetint);
			ApplySkinToPlayer(Skin.u.string.ptr, CustomColor, SkinColorBodyint, SkinColorFeetint);
		}
		m_DDstatsSearchType = 0;
	}
	json_value_free(pJson);
}

void CRClient::ResetRclientDDstatsProfile()
{
	if(m_pRClientDDstatsTask)
	{
		m_pRClientDDstatsTask->Abort();
		m_pRClientDDstatsTask = nullptr;
	}
}

void CRClient::ConFindSkin(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	int ClientId = FindPlayerClientId(pThis->GameClient(), TrimRight(pResult->GetString(0)).c_str());
	if(ClientId == -1)
	{
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", "Invalid client ID");
		pThis->GameClient()->Echo("No that player on server");
		return;
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintSkinInfo(pThis->GameClient(), ClientData.m_aSkinName, ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
	}
}

void CRClient::ConFindPlayer(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	int ClientId = FindPlayerClientId(pThis->GameClient(), TrimRight(pResult->GetString(0)).c_str());
	if(ClientId == -1)
	{
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", "Invalid client ID");
		pThis->GameClient()->Echo("No that player on server");
		return;
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintPlayerInfo(pThis->GameClient(), ClientData.m_aName, ClientData.m_aSkinName, ClientData.m_aClan, ClientData.m_Country, ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
	}
}

void CRClient::ConCopySkin(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	int ClientId = FindPlayerClientId(pThis->GameClient(), TrimRight(pResult->GetString(0)).c_str());
	if(ClientId == -1)
	{
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", "Invalid client ID");
		pThis->GameClient()->Echo("No that player on server");
		return;
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintSkinInfo(pThis->GameClient(), ClientData.m_aSkinName, ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
		pThis->ApplySkinToPlayer(ClientData.m_aSkinName, ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
	}
}

void CRClient::ConCopyColor(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	int ClientId = FindPlayerClientId(pThis->GameClient(), TrimRight(pResult->GetString(0)).c_str());
	if(ClientId == -1)
	{
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", "Invalid client ID");
		pThis->GameClient()->Echo("No that player on server");
		return;
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintColorInfo(pThis->GameClient(), ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
		pThis->ApplyColorToPlayer(ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
	}
}

void CRClient::ConBackupPlayerProfile(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pSelf = (CRClient *)pUserData;
	if(g_Config.m_ClDummy == 1)
	{
		if(str_length(pSelf->DummySkinBeforeCopyPlayer) > 0)
		{
			str_copy(g_Config.m_ClDummySkin, pSelf->DummySkinBeforeCopyPlayer, sizeof(g_Config.m_ClDummySkin));
			g_Config.m_ClDummyUseCustomColor = pSelf->DummyUseCustomColorBeforeCopyPlayer;
			g_Config.m_ClDummyColorBody = pSelf->DummyBodyColorBeforeCopyPlayer;
			g_Config.m_ClDummyColorFeet = pSelf->DummyFeetColorBeforeCopyPlayer;
			pSelf->GameClient()->SendDummyInfo(false);
		}
		else
		{
			pSelf->GameClient()->Echo("There no info of player/skin copy");
		}
	}
	if(g_Config.m_ClDummy == 0)
	{
		if(str_length(pSelf->PlayerSkinBeforeCopyPlayer) > 0)
		{
			str_copy(g_Config.m_ClPlayerSkin, pSelf->PlayerSkinBeforeCopyPlayer, sizeof(g_Config.m_ClPlayerSkin));
			g_Config.m_ClPlayerUseCustomColor = pSelf->PlayerUseCustomColorBeforeCopyPlayer;
			g_Config.m_ClPlayerColorBody = pSelf->PlayerBodyColorBeforeCopyPlayer;
			g_Config.m_ClPlayerColorFeet = pSelf->PlayerFeetColorBeforeCopyPlayer;
			pSelf->GameClient()->SendInfo(false);
		}
		else
		{
			pSelf->GameClient()->Echo("There no info of player/skin copy");
		}
	}
}

// Tracker
void CRClient::ConTrackerAdd(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	int ClientId = FindPlayerClientId(pThis->GameClient(), TrimRight(pResult->GetString(0)).c_str());
	if(ClientId == -1)
	{
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Tracker", "Invalid client ID");
		pThis->GameClient()->Echo("No that player on server");
		return;
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aName[0] && ClientData.ClientId())
	{
		FastPrint(pThis->GameClient(), "Tracker", "Added player: %s", ClientData.m_aName);
		FastEcho(pThis->GameClient(), "[[green]]Tracker: Added player: %s", ClientData.m_aName);
		pThis->m_vPlayersInTracker.push_back({ClientData.ClientId(), ClientData.m_aName});
	}
}

void CRClient::ConTrackerRemove(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	int ClientId = FindPlayerClientId(pThis->GameClient(), TrimRight(pResult->GetString(0)).c_str());
	if(ClientId == -1)
	{
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Tracker", "Invalid client ID");
		pThis->GameClient()->Echo("No that player on server");
		return;
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aName[0] && ClientData.ClientId())
	{
		for(size_t i = 0; i < pThis->m_vPlayersInTracker.size(); i++)
		{
			if(ClientData.ClientId() == pThis->m_vPlayersInTracker[i].m_ClientId)
			{
				FastPrint(pThis->GameClient(), "Tracker", "Removed player: %s", pThis->m_vPlayersInTracker[i].m_Nickname.c_str());
				FastEcho(pThis->GameClient(), "[[red]]Tracker: Removed player: %s", pThis->m_vPlayersInTracker[i].m_Nickname.c_str());
				pThis->m_vPlayersInTracker.erase(pThis->m_vPlayersInTracker.begin() + i);
				return;
			}
		}
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Tracker", "Player not in tracker");
		pThis->GameClient()->Echo("Player not in tracker");
	}
}

void CRClient::ConTrackerReset(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	for(size_t i = 0; i < pThis->m_vPlayersInTracker.size(); i++)
	{
		FastPrint(pThis->GameClient(), "Tracker", "Removed player: %s", pThis->m_vPlayersInTracker[i].m_Nickname.c_str());
		FastEcho(pThis->GameClient(), "[[red]]Tracker: Removed player: %s", pThis->m_vPlayersInTracker[i].m_Nickname.c_str());
	}
	pThis->m_vPlayersInTracker.clear();
}

void CRClient::TrackerClientIdRemove(int ClientId)
{
	for(size_t i = 0; i < m_vPlayersInTracker.size(); i++)
	{
		if(ClientId == m_vPlayersInTracker[i].m_ClientId)
		{
			FastPrint(GameClient(), "Tracker", "Removed player: %s", m_vPlayersInTracker[i].m_Nickname.c_str());
			FastEcho(GameClient(), "[[red]]Tracker: Removed player: %s", m_vPlayersInTracker[i].m_Nickname.c_str());
			m_vPlayersInTracker.erase(m_vPlayersInTracker.begin() + i);
			return;
		}
	}
}