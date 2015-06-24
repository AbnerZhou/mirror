#include <QMessageBox>

#include "item_equipbag.h"

extern RoleInfo_False g_falseRole;

extern QVector<Info_basic_equip> g_EquipList;
extern Dlg_Detail *m_dlg_detail;

item_equipBag::item_equipBag(RoleInfo *info, ListEquip *item, ListEquip *storageItem)
	: myRole(info), m_item(item), m_storageItem(storageItem)
{
	ui.btn_sale->setVisible(true);
//	ui.btn_sort->setVisible(true);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	CurrentPage = 1;
	ui.edit_page_all->setText(QString::number(4));

	popMenu = new QMenu();
	action_use = new QAction(QStringLiteral("װ��"), this);
	action_storage = new QAction(QStringLiteral("���"), this);
	action_sale = new QAction(QStringLiteral("����"), this);
	popMenu->addAction(action_use);
	popMenu->addAction(action_storage);
	popMenu->addAction(action_sale);

	connect(ui.btn_sale, SIGNAL(clicked()), this, SLOT(on_btn_sale_clicked()));
	connect(ui.btn_pgUp, SIGNAL(clicked()), this, SLOT(on_btn_pgUp_clicked()));
	connect(ui.btn_pgDn, SIGNAL(clicked()), this, SLOT(on_btn_pgDn_clicked()));
	connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(ShowItemInfo(int, int)));
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(QPoint)));

	connect(action_use, SIGNAL(triggered(bool)), this, SLOT(on_action_use(bool)));
	connect(action_storage, SIGNAL(triggered(bool)), this, SLOT(on_action_storage(bool)));
	connect(action_sale, SIGNAL(triggered(bool)), this, SLOT(on_action_sale(bool)));
}

item_equipBag::~item_equipBag()
{

}

void item_equipBag::updateInfo()
{
	quint32 row_Count = ui.tableWidget->rowCount();
	quint32 Col_Count = ui.tableWidget->columnCount();
	quint32 row_cur = 0;
	quint32 col_cur = 0;

	QString strTmp = "";

	pages = (m_item->size() + row_Count * Col_Count - 1 ) / (row_Count * Col_Count);
	if (pages == 0)
		pages = 1;
	if (CurrentPage > pages)
		CurrentPage = pages;

	ui.edit_page_cur->setText(QString::number(CurrentPage));
	if (pages == 1)
	{
		ui.btn_pgDn->setEnabled(false);
		ui.btn_pgUp->setEnabled(false);
	}
	else if (CurrentPage == 1)
	{
		ui.btn_pgDn->setEnabled(true);
		ui.btn_pgUp->setEnabled(false);
	}
	else if (CurrentPage >= pages)
	{
		ui.btn_pgDn->setEnabled(false);
		ui.btn_pgUp->setEnabled(true);
	}
	else
	{
		ui.btn_pgDn->setEnabled(true);
		ui.btn_pgUp->setEnabled(true);
	}

	//���������������ʾ������ǰװ������С��֮ǰ����ʱ������β��ʾԭװ���ļ���
	ui.tableWidget->clear();
	ListEquip::const_iterator iter = m_item->constBegin();
	for (quint32 i = 0; i < (CurrentPage - 1) * (row_Count * Col_Count); i++, iter++) { ; }

	for (; iter != m_item->constEnd(); iter++)
	{	
		const Info_basic_equip *EquipBasicInfo = GetEquipBasicInfo(iter->ID);
		if (EquipBasicInfo == nullptr)
		{
			continue;
		}

		ui.tableWidget->setItem(row_cur, col_cur++, new QTableWidgetItem(EquipBasicInfo->icon, strTmp));
		if (col_cur >= Col_Count)
		{
			++row_cur;
			col_cur = 0;
		}
	}
}

void item_equipBag::on_btn_pgUp_clicked()
{
	if (CurrentPage > 1)
	{
		--CurrentPage;
		updateInfo();
	}
}
void item_equipBag::on_btn_pgDn_clicked()
{
	if (CurrentPage < pages)
	{
		++CurrentPage;
		updateInfo();
	}
}

void item_equipBag::ShowItemInfo(int row, int column)
{
	ShowItemInfo_equip(row, column, CurrentPage, m_item, myRole);
}

void item_equipBag::ShowContextMenu(QPoint pos)
{
	m_dlg_detail->hide();

	//����һ��հ׵�Ԫ�񣬲������Ҽ��˵���
	if (m_item->size() > GetCurrentCellIndex(CurrentPage))
	{
		popMenu->exec(ui.tableWidget->mapToGlobal(pos));
	}
}

void item_equipBag::on_action_use(bool checked)
{
	quint32 index = GetCurrentCellIndex(CurrentPage);
	const Info_Equip equip = m_item->at(index);
	quint32 nTmp;

	const Info_basic_equip *EquipBasicInfo = Item_Base::GetEquipBasicInfo(equip.ID);
	if (EquipBasicInfo == nullptr)
	{
		return;		//unknown equipment ID��
	}
	//��ȡ�����װ�������
	int Type = (equip.ID - g_itemID_start_equip) / 1000;

	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ��
	bool bSatisfy = false;
	switch (EquipBasicInfo->need)
	{
	case 0: 
		nTmp = (myRole->level >> 1) - 1;
		bSatisfy = (nTmp >= EquipBasicInfo->needLvl);
		break;
	case 1: 
		nTmp = myRole->dc2_1 << 24 | myRole->dc2_2 << 16 | myRole->dc2_3 << 8 | myRole->dc2_4;
		bSatisfy = (nTmp >= EquipBasicInfo->needLvl);
		break;
	case 2: 
		nTmp = myRole->mc2_1 << 24 | myRole->mc2_2 << 16 | myRole->mc2_3 << 8 | myRole->mc2_4;
		bSatisfy = (nTmp >= EquipBasicInfo->needLvl);
		break;
	case 3: 
		nTmp = myRole->sc2_1 << 24 | myRole->sc2_2 << 16 | myRole->sc2_3 << 8 | myRole->sc2_4;
		bSatisfy = (nTmp >= EquipBasicInfo->needLvl);
		break;
	default:
		break;
	}
	if (Type == g_equipType_clothes_m || Type == g_equipType_clothes_f)
	{
		//��ǰװ��Ϊ�·������ж��Ա�
		bSatisfy = bSatisfy && (myRole->gender == (Type - 1));
	}

	if (!bSatisfy)
	{
		QString message = QStringLiteral("��δ�ﵽ������װ�������Ҫ��");
		QMessageBox::critical(this, QStringLiteral("��ʾ"), message);
	}
	else
	{
		emit wearEquip(equip.ID, index);
	}
}
void item_equipBag::on_action_storage(bool checked)
{
	quint32 index = GetCurrentCellIndex(CurrentPage);
	const Info_Equip equip = m_item->at(index);

	if (m_storageItem->size() >= g_storage_maxSize)
	{
		QString message = QStringLiteral("�ֿ�������");
		QMessageBox::critical(this, QStringLiteral("��ʾ"), message);
	}
	else
	{
		m_storageItem->append(equip);
		m_item->removeAt(index);

		emit UpdateEquipInfoSignals();
	}
}
void item_equipBag::on_action_sale(bool checked)
{
	quint32 index = GetCurrentCellIndex(CurrentPage);
	const Info_Equip equip = m_item->at(index);

	const Info_basic_equip *EquipBasicInfo = GetEquipBasicInfo(equip.ID);
	if (EquipBasicInfo != NULL)
	{
		myRole->coin += (EquipBasicInfo->price >> 1) << 1;		//һ��۸�����

		g_falseRole.coin += EquipBasicInfo->price >> 1;

		emit UpdatePlayerInfoSignals();

		m_item->removeAt(index);
		updateInfo();
	}
}

void item_equipBag::on_btn_sale_clicked()
{
	QString message = QStringLiteral("���ȷ�Ͻ��۳�����������װ�����Ƿ�ȷ�ϣ�");
	QMessageBox msgBox(QMessageBox::Information, QStringLiteral("һ������"), message);
	QPushButton *YsBtn = msgBox.addButton(QStringLiteral(" ȷ�� "), QMessageBox::AcceptRole);
	QPushButton *NoBtn = msgBox.addButton(QStringLiteral(" ȡ�� "), QMessageBox::RejectRole);
	msgBox.exec();
	if (msgBox.clickedButton() == YsBtn)
	{
		for (ListEquip::const_iterator iter = m_item->begin(); iter != m_item->end(); iter++)
		{
			const Info_basic_equip *EquipBasicInfo = GetEquipBasicInfo(iter->ID);
			if (EquipBasicInfo != nullptr)
			{
				myRole->coin += (EquipBasicInfo->price >> 2) << 1;		//һ������ֻ��1/4�۸�

				g_falseRole.coin += EquipBasicInfo->price >> 2;
			}
		}
		m_item->clear();
		emit UpdatePlayerInfoSignals();
	}
}
void item_equipBag::on_btn_sort_clicked()
{
	//��һ�����������㷨
}