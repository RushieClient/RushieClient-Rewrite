#ifndef GAME_CLIENT_COMPONENTS_RCLIENT_RCLIENT_H
#define GAME_CLIENT_COMPONENTS_RCLIENT_RCLIENT_H

#include <game/client/component.h>

class CRClient : public CComponent
{
public:
	CRClient();
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
	void OnMessage(int MsgType, void *pRawMsg) override;
	void OnConsoleInit() override;
	void OnRender() override;
};

#endif //GAME_CLIENT_COMPONENTS_RCLIENT_RCLIENT_H
