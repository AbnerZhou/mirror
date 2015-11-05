#include "fight.h"


fight::fight(CPlayer *const w_player)
	: QWidget(NULL)
{
	ui.setupUi(this);

	fightmap = new fight_map(0, w_player);
	if (fightmap != nullptr)
	{
		ui.tabWidget_fight->addTab(fightmap, QStringLiteral("��ͨ��ͼ"));
	}

	tower = new Tower(nullptr, w_player);
	if (fightmap != nullptr)
	{
		ui.tabWidget_fight->addTab(tower, QStringLiteral("��ս�ؾ�"));
	}
}

fight::~fight()
{
	delete fightmap;
	delete tower;
}
