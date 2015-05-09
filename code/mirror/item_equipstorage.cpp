#include <QMessageBox>
#include "item_equipstorage.h"

extern QVector<Info_basic_equip> g_EquipList;

Item_equipStorage::Item_equipStorage(RoleInfo *info, ListEquip *item, ListEquip *storageItem)
	: myRole(info), m_item(item), m_storageItem(storageItem)
{
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	CurrentPage = 1;

	connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(ShowItemInfo(int, int)));
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(QPoint)));
}

Item_equipStorage::~Item_equipStorage()
{

}


void Item_equipStorage::updateInfo()
{
	quint32 row_Count = ui.tableWidget->rowCount();
	quint32 Col_Count = ui.tableWidget->columnCount();
	quint32 row_cur = 0;
	quint32 col_cur = 0;

	QString strTmp = "";
	quint32 ID, nCount;
	QString Name;

	//���������������ʾ������ǰװ������С��֮ǰ����ʱ������β��ʾԭװ���ļ���
	ui.tableWidget->clear();
	for (ListEquip::const_iterator iter = m_storageItem->constBegin(); iter != m_storageItem->constEnd(); iter++)
	{
		const Info_basic_equip *EquipBasicInfo = GetEquipBasicInfo(iter->ID);
		if (EquipBasicInfo == nullptr)
		{
			continue;
		}
		ui.tableWidget->setItem(row_cur, col_cur++, new QTableWidgetItem(QIcon(EquipBasicInfo->icon), strTmp));
		if (col_cur >= Col_Count)
		{
			++row_cur;
			col_cur = 0;
		}

		if (row_cur >= row_Count)
		{
			//��ӵ��ڶ�ҳ��
			break;	//�ݲ�����
		}
	}
}

void Item_equipStorage::ShowItemInfo(int row, int column)
{
	ShowItemInfo_equip(row, column, CurrentPage, m_storageItem, myRole);
}

void Item_equipStorage::ShowContextMenu(QPoint pos)
{
	quint32 index = GetCurrentCellIndex(CurrentPage);
	const Info_Equip equip = m_storageItem->at(index);

	if (m_item->size() >= g_bag_maxSize)
	{
		QString message = QStringLiteral("����������");
		QMessageBox::critical(this, QStringLiteral("��ʾ"), message);
	}
	else
	{
		m_item->append(equip);
		m_storageItem->removeAt(index);

		emit UpdateEquipInfoSignals();
	}
}