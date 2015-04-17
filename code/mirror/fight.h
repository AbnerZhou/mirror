#ifndef FIGHT_H
#define FIGHT_H

#include <QWidget>
#include "ui_fight.h"
#include "mytabframe.h"
#include "fight_map.h"

class fight : public myTabFrame
{
	Q_OBJECT

public:
	fight(RoleInfo *roleInfo, MapItem *bag_item);
	~fight();

public:
	//���½�ɫ��Ϣ����Ӧ�ؼ���ˢ�½���
	virtual void updateRoleInfo(void);

private slots:
	

private:
	Ui::fight ui;

	fight_map *fightmap[7];

	RoleInfo *myRole;
	MapItem *m_bag_item;
};

#endif // FIGHT_H
