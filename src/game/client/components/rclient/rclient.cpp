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
	Console()->Register("ri_find_player_from_ddstats", "s[type]", CFGFLAG_CLIENT, ConFindPlayerFromDdstats, this, "Fetch player from DDstats");
	Console()->Register("ri_find_skin_from_ddstats", "s[type]", CFGFLAG_CLIENT, ConFindSkinFromDdstats, this, "Fetch player's skin from DDstats");
	Console()->Register("ri_copy_skin_from_ddstats", "s[type]", CFGFLAG_CLIENT, ConCopySkinFromDdstats, this, "Fetch and copy player's skin from DDstats");
	Console()->Register("find_skin", "r[player]", CFGFLAG_CLIENT, ConFindSkin, this, "Find skin");
	Console()->Register("copy_skin", "r[player]", CFGFLAG_CLIENT, ConCopySkin, this, "Copy skin");
	Console()->Register("find_player", "r[player]", CFGFLAG_CLIENT, ConFindPlayer, this, "Find Player");
	Console()->Register("copy_color", "r[player]", CFGFLAG_CLIENT, ConCopyColor, this, "Copy Color skin");
	Console()->Register("ri_backup_player_profile", "", CFGFLAG_CLIENT, ConBackupPlayerProfile, this, "Backup player profile");
}

void CRClient::OnMessage(int MsgType, void *pRawMsg)
{

}

void CRClient::OnStateChange(int NewState, int OldState)
{

}

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

static void DDstatsPrint(IConsole *pConsole, const char *pFmt, ...)
{
	char aBuf[256];
	va_list Args;
	va_start(Args, pFmt);
	str_format_v(aBuf, sizeof(aBuf), pFmt, Args);
	va_end(Args);
	pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", aBuf);
}

static void PrintPlayerInfo(IConsole *pConsole , const char *Nickname, const char *Skin, const char *Clan, const int Country, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	DDstatsPrint(pConsole ,"- Nickname: %s", Nickname);
	DDstatsPrint(pConsole ,"- Skin name: %s", Skin);
	DDstatsPrint(pConsole ,"- Clan: %s", Clan);
	DDstatsPrint(pConsole ,"- Country: %d", Country);
	if(CustomColor)
	{
		pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 1");
		DDstatsPrint(pConsole ,"- Body Color: %d", SkinColorBodyint);
		DDstatsPrint(pConsole ,"- Feet Color: %d", SkinColorFeetint);
	}
	else
		pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 0");
}

static void PrintSkinInfo(IConsole *pConsole, const char *Skin, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	DDstatsPrint(pConsole ,"- Skin name: %s", Skin);
	if(CustomColor)
	{
		pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 1");
		DDstatsPrint(pConsole ,"- Body Color: %d", SkinColorBodyint);
		DDstatsPrint(pConsole ,"- Feet Color: %d", SkinColorFeetint);
	}
	else
		pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 0");
}

static void PrintColorInfo(IConsole *pConsole, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint)
{
	if(CustomColor)
	{
		pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 1");
		DDstatsPrint(pConsole ,"- Body Color: %d", SkinColorBodyint);
		DDstatsPrint(pConsole ,"- Feet Color: %d", SkinColorFeetint);
	}
	else
		pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Info", "- Custom Color: 0");
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
			PrintPlayerInfo(GameClient()->Console(), Nickname.u.string.ptr, Skin.u.string.ptr, Clan.u.string.ptr, Countryint, CustomColor, SkinColorBodyint, SkinColorFeetint);
		if(m_DDstatsSearchType == 2)
			PrintSkinInfo(GameClient()->Console(), Skin.u.string.ptr, CustomColor, SkinColorBodyint, SkinColorFeetint);
		if(m_DDstatsSearchType == 3)
		{
			PrintSkinInfo(GameClient()->Console(), Skin.u.string.ptr, CustomColor, SkinColorBodyint, SkinColorFeetint);
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

static int FindPlayerClientId(CGameClient *pGameClient ,const char *Nickname) // 1-FindPlayer 2-FindSkin 3-CopySkin
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

void CRClient::ConFindSkin(IConsole::IResult *pResult, void *pUserData)
{
	CRClient *pThis = static_cast<CRClient *>(pUserData);
	int ClientId = FindPlayerClientId(pThis->GameClient(), TrimRight(pResult->GetString(0)).c_str());
	if(ClientId == -1)
	{
		pThis->GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", "Invalid client ID");
		pThis->GameClient()->Echo("No that player on server");
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintSkinInfo(pThis->GameClient()->Console(), ClientData.m_aSkinName, ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
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
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintPlayerInfo(pThis->GameClient()->Console(), ClientData.m_aName, ClientData.m_aSkinName, ClientData.m_aClan, ClientData.m_Country, ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
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
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintSkinInfo(pThis->GameClient()->Console(), ClientData.m_aSkinName, ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
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
	}
	const CGameClient::CClientData &ClientData = pThis->GameClient()->m_aClients[ClientId];
	if(ClientData.m_aSkinName[0])
	{
		PrintColorInfo(pThis->GameClient()->Console(), ClientData.m_UseCustomColor, ClientData.m_ColorBody, ClientData.m_ColorFeet);
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