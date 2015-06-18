#ifndef _DEF_ITEM_EQUIP_H
#define _DEF_ITEM_EQUIP_H

#include <QUuid>
#include <QPixmap>

#include "def_DataType.h"

const QString equip_need_info[4] = {
	QStringLiteral("��Ҫ�ȼ�"),
	QStringLiteral("��Ҫ����"),
	QStringLiteral("��Ҫħ��"),
	QStringLiteral("��Ҫ����")
};

struct Info_basic_equip {
	itemID ID;
	QString name;
	QPixmap icon;
	quint32 lv;
	quint32 luck;
	quint32 acc;	//׼ȷ
	quint32 ag;		//����	
	quint32 spd;	//�ٶ�
	quint32 md;		//ħ�����
	quint32 ep;		//��������
	quint32 ed;		//�����˺�
	quint32 ac1;
	quint32 ac2;
	quint32 mac1;
	quint32 mac2;
	quint32 dc1;
	quint32 dc2;
	quint32 mc1;
	quint32 mc2;
	quint32 sc1;
	quint32 sc2;
	quint32 need;
	quint32 needLvl;
	quint32 price;
};

struct Info_StateEquip
{
	QPixmap img;
	qint32 offset_x;
	qint32 offset_y;
};

struct EquipExtra {
	quint32 acc;			//��ʷ�������Ժ��һ���ȥ����
	quint32 luck;
	quint32 ac;
	quint32 mac;
	quint32 dc;
	quint32 mc;
	quint32 sc;
};

struct Info_Equip {
//	QUuid uuid;
	itemID ID;
	quint32 lvUp;
	quint32 extraAmount;
	EquipExtra extra;
};
typedef QList<Info_Equip> ListEquip;


struct info_formula {
	itemID ID;
	quint32	npc;
	itemID	m_m;
	itemID	m1_t;
	quint32	m1_c;
	itemID	m2_t;
	quint32	m2_c;
	itemID	m3_t;
	quint32	m3_c;
	itemID	m4_t;	
	quint32	m4_c;
};

#endif //#ifndef _DEF_ITEM_EQUIP_H