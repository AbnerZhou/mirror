#include <QMessageBox>

#include "item_equipbag.h"
#include "dlg_detail.h"
#include "player.h"

extern QWidget *g_widget;

extern Dlg_Detail *g_dlg_detail;

item_equipBag::item_equipBag(QWidget *parent)
	:Item_Base(parent)
{
	ui.btn_sale->setVisible(true);
	ui.btn_sort->setVisible(true);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	bag_equip = &PlayerIns.get_bag_equip();

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
	connect(ui.btn_sort, SIGNAL(clicked()), this, SLOT(on_btn_sort_clicked()));
	connect(ui.btn_pgUp, SIGNAL(clicked()), this, SLOT(on_btn_pgUp_clicked()));
	connect(ui.btn_pgDn, SIGNAL(clicked()), this, SLOT(on_btn_pgDn_clicked()));
	connect(ui.tableWidget, SIGNAL(cellEntered(int, int)), this, SLOT(ShowItemInfo(int, int)));
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(QPoint)));

	connect(action_use, SIGNAL(triggered(bool)), this, SLOT(on_action_use()));
	connect(action_storage, SIGNAL(triggered(bool)), this, SLOT(on_action_storage()));
	connect(action_sale, SIGNAL(triggered(bool)), this, SLOT(on_action_sale()));
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

	pages = (bag_equip->size() + row_Count * Col_Count - 1 ) / (row_Count * Col_Count);
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
	auto iter = bag_equip->constBegin();
	for (quint32 i = 0; i < (CurrentPage - 1) * (row_Count * Col_Count); i++, iter++) { ; }

	for (; iter != bag_equip->constEnd(); iter++)
	{	
		const Info_basic_equip *EquipBasicInfo = GetEquipBasicInfo(iter->ID);
		if (EquipBasicInfo == nullptr)
		{
			continue;
		}
		QTableWidgetItem *twItem = new QTableWidgetItem(EquipBasicInfo->icon,strTmp);
		twItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		//twItem->setBackground(QBrush(EquipBasicInfo->icon));
		ui.tableWidget->setItem(row_cur, col_cur++, twItem);
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
	ShowItemInfo_equip(row, column, CurrentPage, bag_equip);
}

void item_equipBag::ShowContextMenu(QPoint pos)
{
	g_dlg_detail->hide();

	//�һ��ǿհ׵�Ԫ�񣬲ŵ����Ҽ��˵���
	if (bag_equip->size() > GetCurrentCellIndex(CurrentPage))
	{
		popMenu->exec(ui.tableWidget->mapToGlobal(pos));
	}
}

void item_equipBag::on_action_use()
{
	quint32 index = GetCurrentCellIndex(CurrentPage);
	const Info_Equip &equip = bag_equip->at(index);
	const CHuman &CurEdt = PlayerIns.get_edt_current();
	qint32 nTmp;
	QString message = QStringLiteral("��δ�ﵽ������װ�������Ҫ��");

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
		bSatisfy = (CurEdt.get_lv() >= EquipBasicInfo->needLvl);
		break;
	case 1: 
		bSatisfy = (CurEdt.get_dc2() >= EquipBasicInfo->needLvl);
		break;
	case 2: 
		bSatisfy = (CurEdt.get_mc2() >= EquipBasicInfo->needLvl);
		break;
	case 3: 
		bSatisfy = (CurEdt.get_sc2() >= EquipBasicInfo->needLvl);
		break;
	default:
		break;
	}
	if (Type == g_equipType_clothes_m || Type == g_equipType_clothes_f)
	{
		//��ǰװ��Ϊ�·������ж��Ա�
		bool bTmp = (CurEdt.get_gender() == (Type - 1));
		bSatisfy = bSatisfy && bTmp;
		if (!bTmp)
		{
			QString strTmp = (Type == g_equipType_clothes_m) ? QStringLiteral("��") : QStringLiteral("Ů");
			message = QStringLiteral("�Ա𲻷�,��װ��Ϊ%1��װ��").arg(strTmp);
		}	
	}

	if (bSatisfy)	{
		nTmp = PlayerIns.get_edt_current().wearEquip(index);
		updateInfo();
		emit UpdateDisplayEquip(nTmp);
	}	else	{
		QMessageBox::critical(g_widget, QStringLiteral("��ʾ"), message);
	}
}
void item_equipBag::on_action_storage()
{
	ListEquip &storage_equip = PlayerIns.get_storage_equip();

	quint32 index = GetCurrentCellIndex(CurrentPage);
	const Info_Equip equip = bag_equip->at(index);

	if (storage_equip.size() >= g_storage_maxSize)
	{
		QString message = QStringLiteral("�ֿ�������");
		QMessageBox::critical(g_widget, QStringLiteral("��ʾ"), message);
	}
	else
	{
		storage_equip.append(equip);
		bag_equip->removeAt(index);

		updateInfo();
		emit UpdateBag_StorageEquip();
	}
}
void item_equipBag::on_action_sale()
{
	quint32 index = GetCurrentCellIndex(CurrentPage);
	const Info_Equip equip = bag_equip->at(index);

	const Info_basic_equip *EquipBasicInfo = GetEquipBasicInfo(equip.ID);
	if (EquipBasicInfo != NULL)
	{
		PlayerIns.add_coin(EquipBasicInfo->price >> 1);	//һ��۸�����
		bag_equip->removeAt(index);
		updateInfo();
		emit UpdateCoin();
	}
}

void item_equipBag::on_btn_sale_clicked()
{
	QString message = QStringLiteral("���ȷ�Ͻ��۳�����������װ��(��������)���Ƿ�ȷ�ϣ�");
	QMessageBox msgBox(QMessageBox::Information, QStringLiteral("һ������"), message);
	QPushButton *YsBtn = msgBox.addButton(QStringLiteral(" ȷ�� "), QMessageBox::AcceptRole);
	QPushButton *NoBtn = msgBox.addButton(QStringLiteral(" ȡ�� "), QMessageBox::RejectRole);
	msgBox.exec();
	if (msgBox.clickedButton() == YsBtn)
	{
		ListEquip tmp;
		for (auto iter = bag_equip->constBegin(); iter != bag_equip->constEnd(); iter++)
		{
			const Info_basic_equip *EquipBasicInfo = GetEquipBasicInfo(iter->ID);
			if (EquipBasicInfo != nullptr)
			{
				if (EquipBasicInfo->lv == 9999)
				{
					tmp.append(*iter);
				}
				else
				{
					PlayerIns.add_coin(EquipBasicInfo->price >> 2);		//һ������ֻ��1/4�۸�
				}
			}
		}
		bag_equip->clear();
		for (ListEquip::const_iterator iter = tmp.begin(); iter != tmp.end(); iter++)
		{
			bag_equip->append(*iter);
		}
		updateInfo();
		emit UpdateCoin();
	}
}
void item_equipBag::on_btn_sort_clicked()
{
	ListEquip tmp;
	qint32 j, k;
	if (bag_equip->size() <= 0)
	{
		return;
	}
	
	//ѡ�����򣬰�Ʒ�ʡ�
	//����һ�α������㽫ͬһƷ��װ�����뵽��ʱ�б��С����㷨�ٶ�ԼΪO(C * n) * 2,��ֱ�Ӽ�дΪO(n).
	k = g_specialEquip_MaxExtra;
	j = 0;
	do
	{
		for (auto iter = bag_equip->constBegin(); iter != bag_equip->constEnd(); iter++)
		{
			if (iter->extraAmount == k)
			{
				tmp.append(*iter);
			}
		}
	} while (k--);

	bag_equip->clear();
	for (auto iter = tmp.constBegin(); iter != tmp.constEnd(); iter++)
	{
		bag_equip->append(*iter);
	}
	updateInfo();
}