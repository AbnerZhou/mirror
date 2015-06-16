#include "task.h"

task::task(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	int nTaskID;
	QString str[5] = { QStringLiteral("����"), QStringLiteral("֧��"), QStringLiteral("�ճ�"), QStringLiteral("ѭ��"), QStringLiteral("����") };
	for (int i = 0; i < 5; i++)
	{
		nTaskID = 500000 + i * 1000;
		QTreeWidgetItem *propType = new QTreeWidgetItem(ui.treeWidget);
		propType->setText(0, str[i]);
		propType->setText(1, QString::number(nTaskID));
		for (int j = 1; j < 5; j++)
		{
			QTreeWidgetItem *propItem = new QTreeWidgetItem(propType);
			propItem->setText(0, QStringLiteral("�ȴ����"));
			propItem->setText(1, QString::number(nTaskID + j));
		}
	}
	ui.treeWidget->expandAll();
	ui.treeWidget->hideColumn(1);

	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, qint32)), this, SLOT(itemClicked(QTreeWidgetItem *, qint32)));
}

task::~task()
{

}

void task::itemClicked(QTreeWidgetItem * item, int column)
{
	ui.textEdit->setText(item->text(1));

// 	QString title = QStringLiteral("������ʾ");
// 	if (g_roleAddition.taskStep >= g_task_main_list.size())
// 	{
// 		QMessageBox::information(this, title, QStringLiteral("û�п�������"));
// 		return;
// 	}
// 	const info_task &task = g_task_main_list[g_roleAddition.taskStep];
// 
// 	QMessageBox *msgBox = new QMessageBox;
// 	QString strTmp = task.msg;
// 	QString strGiveItem;
// 	msgBox->setText(strTmp);
// 	msgBox->setWindowTitle(title);
// 
// 	strTmp = QStringLiteral("������:");
// 	strGiveItem = "";
// 	foreach(const itemID id, task.giveItem)
// 	{
// 		strGiveItem += " ";
// 		if (id > g_itemID_start_equip && id <= g_itemID_stop_equip)
// 		{
// 			const Info_basic_equip *EquipBasicInfo = Item_Base::GetEquipBasicInfo(id);
// 			if (EquipBasicInfo != nullptr)
// 			{
// 				strGiveItem += EquipBasicInfo->name + QStringLiteral(" ����: ") + QString::number(task.giveCount);
// 			}
// 		}
// 		else if (id > g_itemID_start_item && id <= g_itemID_stop_item)
// 		{
// 			const Info_Item *itemInfo = Item_Base::FindItem_Item(id);
// 			if (itemInfo != nullptr)
// 			{
// 				strGiveItem += itemInfo->name + QStringLiteral(" ����: ") + QString::number(task.giveCount);
// 			}
// 		}
// 		else
// 		{
// 			strGiveItem += QStringLiteral("δ֪���� ������1");
// 		}
// 	}
// 	msgBox->setInformativeText(strTmp + strGiveItem);
// 
// 	QPushButton *YsBtn = msgBox->addButton(QStringLiteral(" ���Ѿ��������� "), QMessageBox::AcceptRole);
// 	QPushButton *NoBtn = msgBox->addButton(QStringLiteral(" �����ȥ "), QMessageBox::RejectRole);
// 	msgBox->setDefaultButton(NoBtn);
// 	msgBox->exec();
// 	bool bReject = (msgBox->clickedButton() == NoBtn);
// 	delete msgBox;
// 
// 	bool bTaskFinish = false;
// 
// 	if (bReject)
// 	{
// 		return;
// 	}
// 
// 	if (task.requireItem > g_itemID_start_equip && task.requireItem <= g_itemID_stop_equip)
// 	{
// 		for (quint32 i = 0; i < m_bag_equip->size(); i++)
// 		{
// 			if (m_bag_equip->at(i).ID == task.requireItem)
// 			{
// 				m_bag_equip->removeAt(i);
// 				bTaskFinish = true;
// 				break;
// 			}
// 		}
// 	}
// 	else if (task.requireItem > g_itemID_start_item && task.requireItem <= g_itemID_stop_item)
// 	{
// 		if (m_bag_item->value(task.requireItem) > task.requireCount)
// 		{
// 			m_bag_item->insert(task.requireItem, m_bag_item->value(task.requireItem) - task.requireCount);
// 			bTaskFinish = true;
// 		}
// 		else if (m_bag_item->value(task.requireItem) == task.requireCount)
// 		{
// 			m_bag_item->remove(task.requireItem);
// 			bTaskFinish = true;
// 		}
// 	}
// 	else
// 	{
// 		//nothing
// 	}
// 
// 
// 	if (bTaskFinish)
// 	{
// 		foreach(const itemID id, task.giveItem)
// 		{
// 			if (id > g_itemID_start_equip && id <= g_itemID_stop_equip)
// 			{
// 				Info_Equip equip = { 0 };
// 				equip.ID = id;
// 				m_bag_equip->append(equip);
// 
// 				m_tab_equipBag.updateInfo();
// 			}
// 			else if (id > g_itemID_start_item && id <= g_itemID_stop_item)
// 			{
// 				m_bag_item->insert(id, m_bag_item->value(id) + task.giveCount);
// 
// 				m_tab_itemBag.updateInfo();
// 			}
// 			else
// 			{
// 				//nothing;
// 			}
// 		}
// 
// 		QMessageBox *finishTaskBox = new QMessageBox;
// 		finishTaskBox->setText(QStringLiteral("�����Ʒ�� ") + strGiveItem);
// 		finishTaskBox->setWindowTitle(title);
// 		finishTaskBox->exec();
// 		delete finishTaskBox;
// 
// 		g_roleAddition.taskStep++;
// 	}
// 	else
// 	{
// 		QMessageBox::critical(this, title, QStringLiteral("С�ӣ���Ҫ��ƭ�����˼ҡ�"));
// 	}
}