#include "mirror.h"
#include <QMessageBox>
#include <QFile>

QWidget *g_widget;
QList<ItemInfo> g_ItemList;

mirror::mirror(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	g_widget = this;

	if (!LoadItemList("./item_item.db") 
	 || !LoadEquipList(""))
	{
		QString message = QString::fromLocal8Bit("���ص���ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, tr("QMessageBox::critical()"), message);

		exit(0);
	}

	m_tab_fight = new fight(&roleInfo, &m_bag_item);
	ui.tabWidget_main->addTab(m_tab_fight, QString::fromLocal8Bit("ս��"));

	m_tab_role = new role(&roleInfo, &m_bag_item, &m_storage_item);
	ui.tabWidget_main->addTab(m_tab_role, QString::fromLocal8Bit("��ɫ"));

	m_tab_city = new city(&roleInfo, &m_bag_item);
	ui.tabWidget_main->addTab(m_tab_city, QString::fromLocal8Bit("����"));

#ifdef _DEBUG
	{
		ui.tabWidget_main->setCurrentIndex(2);
	}
#else
	{
		ui.tabWidget_main->setCurrentIndex(1);
	}
#endif
		
	
	
	connect(ui.tabWidget_main, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

mirror::~mirror()
{
	delete m_tab_fight;
	delete m_tab_role;
	delete m_tab_city;
}

void mirror::tabChanged(int index)
{
	QWidget *tab = ui.tabWidget_main->currentWidget();
	myTabFrame *frame = (myTabFrame *)tab;
	if (frame != NULL)
	{
		frame->updateRoleInfo();
	}
}

bool mirror::LoadItemList(const QString &dbName)
{
	QFile file(dbName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	ItemInfo item;
	quint32 type;
	QImage img;

	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> item.ID >> item.name >> img >> item.level >> item.coin >> item.gold >> type;
		out >> item.value >> item.EffectInfo >> item.specification;
		
		item.icon = QPixmap::fromImage(img);
		item.type = static_cast<EffectType>(type);

		g_ItemList.append(item);
	}

	file.close();
	return true;
}
bool mirror::LoadEquipList(const QString &dbName)
{
	return true;
}

void mirror::GiveSomeItem()
{
	quint32 nCount = 6;
	quint32 nTmpArr[] = { 0, 1, 4, 7, 9, 13 };
	quint32 nItemCount[] = { 100, 500, 300, 300, 1000, 20000 };
	
	quint32 ID;

	for (quint32 i = 0; i < nCount; i++)
	{
		ID = g_ItemList.at(nTmpArr[i]).ID;
		m_bag_item[ID] = nItemCount[i];
	}
}