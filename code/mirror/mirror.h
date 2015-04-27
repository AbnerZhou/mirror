#ifndef MIRROR_H
#define MIRROR_H

#include <QtWidgets/QMainWindow>
#include "ui_mirror.h"

#include "role.h"
#include "fight.h"
#include "city.h"

class mirror : public QMainWindow
{
	Q_OBJECT

public:
	mirror(QWidget *parent = 0);
	~mirror();

	//�ӽ�ɫһЩ���ߣ��˺�����Ҫ�������Ե��ߡ�
	void GiveSomeItem(void);

private:
	//����ְҵ�ӳ���Ϣ���ݿ�
	bool LoadJobSet();

	//���ص�����Ϣ���ݿ�
	bool LoadItemList(const QString &dbName);

	//����װ����Ϣ���ݿ�
	bool LoadEquipList(const QString &dbName);

	//���ع���ֲ���Ϣ���ݿ�
	bool LoadDistribute();
	//������ͨ����Ϣ���ݿ�
	bool LoadMonster();
	//����BOSS����Ϣ���ݿ�
	bool LoadBoss();
	//���ع��ﱩ���趨���ݿ�
	bool LoadDropSet();

private slots:
	void tabChanged(int index);

private:
	Ui::mirrorClass ui;

	RoleInfo roleInfo;						//��ɫ������Ϣ
	MapItem m_bag_item;			//���߱���
	MapItem m_storage_item;		//���ֿ߲�
	ListEquip m_bag_equip;		//װ������
	ListEquip m_storage_equip;	//װ���ֿ�

	role *m_tab_role;
	fight *m_tab_fight;
	city *m_tab_city;
};

#endif // MIRROR_H
