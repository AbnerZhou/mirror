#include "fight_map.h"
#include <QMessageBox>
#include "MonsterDefine.h"
#include "Player.h"

extern QWidget *g_widget;
extern QMap<mapID, Info_Distribute> g_MonsterDistribute;

fight_map::fight_map(qint32 mapType)
	: QWidget(NULL), mapType(mapType)
{
	ui.setupUi(this);

	DisplayMap();
	ui.listWidget->setMovement(QListView::Static);
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(itemClicked(QListWidgetItem *)));
}

fight_map::~fight_map()
{

}

void fight_map::DisplayMap()
{
	qint32 nStart = (mapType) * 1000;
	qint32 nStop = 24;
//	qint32 nStop = (m_mapID + 1) * 1000;
	QListWidgetItem *item;

	foreach(const Info_Distribute &dis, g_MonsterDistribute)
	{
		if (dis.ID < nStart)
		{
			continue;
		}
		else if (dis.ID < nStop)
		{
			item = new QListWidgetItem(dis.img, dis.name);
			item->setWhatsThis(QString::number(dis.ID));
			
			ui.listWidget->addItem(item);
		}
		else
		{
			break;
		}
	}
}

void fight_map::itemClicked(QListWidgetItem * item)
{
	qint32 id= item->whatsThis().toInt();
	if (PlayerIns.get_maxMapID() >= id -1)
	{
		emit SelectMap(id);
	}
	else
	{
		QMessageBox::critical(this, QStringLiteral("����"), QStringLiteral("�����ǰһ��ͼ��BOSS"));
	}	
}