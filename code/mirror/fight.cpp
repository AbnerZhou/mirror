#include "fight.h"


fight::fight(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	map_normal = new fight_map(0);
	if (map_normal != nullptr)
	{
		ui.tabWidget_fight->addTab(map_normal, QStringLiteral("��ͨ��ͼ"));
	}

	map_limit = new fight_map(1);
	if (map_limit != nullptr)
	{
		ui.tabWidget_fight->addTab(map_limit, QStringLiteral("����"));
	}

// 	tower = new Tower(nullptr, w_player);
// 	if (fightmap != nullptr)
// 	{
// 		ui.tabWidget_fight->addTab(tower, QStringLiteral("��ս�ؾ�"));
// 	}

	QObject::connect(map_normal, &fight_map::SelectMap, this, &fight::on_SelectMap);
	QObject::connect(map_limit, &fight_map::SelectMap, this, &fight::on_SelectMap);
}

fight::~fight()
{
	delete map_normal;
}

void fight::on_SelectMap(qint32 id)
{
	currentMap = id;
	close();
}