#include "city_shop.h"
#include "dlg_count.h"
#include <QMessageBox>
#include <QMouseEvent>


extern QMap<itemID, Info_Item> g_ItemList;

city_shop::city_shop(QWidget *parent, CPlayer *const w_player)
	: QWidget(parent), m_ShopType(1), player(w_player)
{
	ui.setupUi(this);
	ui.listWidget->setMovement(QListView::Static);
	m_parrent = parent;

	m_bag_item = player->get_bag_item();

	DisplayItemList();
	ui.listWidget->setStyleSheet("QListWidget{ background:transparent} ");

	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(itemClicked(QListWidgetItem *)));
	connect(ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(itemDoubleClicked(QListWidgetItem *)));
}

city_shop::~city_shop()
{

}

bool city_shop::DisplayItemList(void)
{
	QListWidgetItem *ListItem;

	foreach(const Info_Item &item, g_ItemList)
	{
		if (item.sale == m_ShopType)
		{
			ListItem = new QListWidgetItem(item.icon, "");
			ListItem->setWhatsThis(QString::number(item.ID));
			ui.listWidget->addItem(ListItem);
		}	
	}
	return true;
}

void city_shop::itemClicked(QListWidgetItem * item)
{
	itemID id = item->whatsThis().toUInt();
	const Info_Item *itemitem = Item_Base::FindItem_Item(id);
	if (itemitem != nullptr)
	{
		QString strTmp = itemitem->name + QStringLiteral("  �ȼ�:") + QString::number(itemitem->level);
		strTmp += QStringLiteral("  ����:") + QString::number(itemitem->coin);
		ui.lbl_ItemName->setText(strTmp);
		ui.lbl_msg->setText(itemitem->descr);
	}
}

void city_shop::itemDoubleClicked(QListWidgetItem * item)
{
	itemID ID = item->whatsThis().toUInt();
	const Info_Item *itemitem = Item_Base::FindItem_Item(ID);
	if (itemitem == nullptr)
	{
		return;
	}

	quint32 nCount, nCost;
	quint32 price = itemitem->coin;							//��������ߵĵ���
	quint32 nMaxCount = player->get_coin() / price;			//��ҵ�ǰ�ʽ�ɹ�������������
	if (nMaxCount > 9999)
	{
		nMaxCount = 9999;									//�������������9999
	}

	//��������Ի���
	dlg_count *dlg = new dlg_count(this, QStringLiteral("������"), nMaxCount);
	if (QDialog::Accepted == dlg->exec())
	{
		nCount = dlg->getCount();
		nCost = price * nCount;
		if (nCost > player->get_coin())
		{
			QString message = QStringLiteral("���˲�Ҫ̫̰�ģ������е��ʽ����ֻ�ܹ���") + QString::number(nMaxCount);
			QMessageBox::critical(this, QStringLiteral("����"), message);
		}
		else
		{
			if (nCount > 0)
			{
				//��ֹ��'0'����Ʒ��
				player->set_coin(player->get_coin() - nCost);
				m_bag_item->insert(ID, m_bag_item->value(ID) + nCount);
			}
		}
	}
	delete dlg;
}

void city_shop::on_btn_close_clicked(void)
{
	close();
}

void city_shop::mouseMoveEvent(QMouseEvent * ev)
{
	QPoint point = m_parrent->mapFromGlobal(ev->globalPos()) - QPoint(30,30);
	move(point);
}