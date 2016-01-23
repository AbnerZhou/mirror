#include "fight.h"


fight::fight(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	fightmap = new fight_map(0);
	if (fightmap != nullptr)
	{
		ui.tabWidget_fight->addTab(fightmap, QStringLiteral("��ͨ��ͼ"));
	}

// 	tower = new Tower(nullptr, w_player);
// 	if (fightmap != nullptr)
// 	{
// 		ui.tabWidget_fight->addTab(tower, QStringLiteral("��ս�ؾ�"));
// 	}

	QObject::connect(fightmap, &fight_map::SelectMap, this, &fight::on_SelectMap);
}

fight::~fight()
{
	delete fightmap;
}

void fight::on_SelectMap(qint32 id)
{
	currentMap = id;
	close();
}