#ifndef GAME_CLIENT_COMPONENTS_RCLIENT_RCLIENT_H
#define GAME_CLIENT_COMPONENTS_RCLIENT_RCLIENT_H

#include "engine/shared/http.h"
#include <engine/shared/console.h>
#include <game/client/component.h>

class CRClient : public CComponent
{
	//Find/Copy Skin
	static void ConFindPlayerFromDdstats(IConsole::IResult *pResult, void *pUserData);
	static void ConFindSkinFromDdstats(IConsole::IResult *pResult, void *pUserData);
	static void ConCopySkinFromDdstats(IConsole::IResult *pResult, void *pUserData);
	static void ConFindSkin(IConsole::IResult *pResult, void *pUserData);
	static void ConCopySkin(IConsole::IResult *pResult, void *pUserData);
	static void ConFindPlayer(IConsole::IResult *pResult, void *pUserData);
	static void ConCopyColor(IConsole::IResult *pResult, void *pUserData);
	static void ConBackupPlayerProfile(IConsole::IResult *pResult, void *pUserData);
	std::shared_ptr<CHttpRequest> m_pRClientDDstatsTask = nullptr;
	void FetchRclientDDstatsProfile();
	void FinishRclientDDstatsProfile();
	void ResetRclientDDstatsProfile();
	char m_DDstatsSearchNickname[32];
	int m_DDstatsSearchType = 0; //1-FindPlayer 2-FindSkin 3-CopySkin
	void ApplySkinToPlayer(const char *Skin, const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint);
	void ApplyColorToPlayer(const int CustomColor, const int SkinColorBodyint, const int SkinColorFeetint);
	char PlayerSkinBeforeCopyPlayer[42];
	int PlayerUseCustomColorBeforeCopyPlayer = 0;
	int PlayerBodyColorBeforeCopyPlayer = 0;
	int PlayerFeetColorBeforeCopyPlayer = 0;
	char DummySkinBeforeCopyPlayer[42];
	int DummyUseCustomColorBeforeCopyPlayer = 0;
	int DummyBodyColorBeforeCopyPlayer = 0;
	int DummyFeetColorBeforeCopyPlayer = 0;

	//Dummy clan
	void DummyConnectedClan(bool IsDummyConnected);
	bool m_DummyConnectedPrevState = false;
public:
	CRClient();
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
	void OnMessage(int MsgType, void *pRawMsg) override;
	void OnConsoleInit() override;
	void OnRender() override;
	void OnReset() override;
	void OnStateChange(int NewState, int OldState) override;
};

#endif //GAME_CLIENT_COMPONENTS_RCLIENT_RCLIENT_H
