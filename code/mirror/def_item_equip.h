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

enum SecretType {
	st_null = 0,
	st_hp,
	st_hpr,
	st_hpd,
	st_mp,
	st_mpr,
	st_mpd,
	st_acc,
	st_macc,
	st_luck,
	st_speed
};

struct Info_basic_equip {
	itemID ID;
	QString name;
	QPixmap icon;
	quint32 lv;
	SecretType st;
	quint32 sbv;
	quint32 sgv;
	quint32 luck;
	quint32 acc;	//׼ȷ
	quint32 sacred;	//��ʥ
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

struct info_equip_secret {
	//��������
	qint32 hp;
	qint32 hpr;
	qint32 hpd;
	qint32 mp;
	qint32 mpr;
	qint32 mpd;
	qint32 acc;
	qint32 macc;
	qint32 luck;
	qint32 speed;

	//�ɳ����ԣ�Ϊ��ǰ��ɫ�ȼ��İٷֱ�
	qint32 ghp;
	qint32 ghpr;
	qint32 ghpd;
	qint32 gmp;
	qint32 gmpr;
	qint32 gmpd;
};



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

struct info_formula_liandan {
	itemID ID;
	itemID	m1_t;
	quint32	m1_c;
	itemID	m2_t;
	quint32	m2_c;
	itemID	m3_t;
	quint32	m3_c;
	itemID	m4_t;
	quint32	m4_c;
	itemID	m5_t;
	quint32	m5_c;
};

#endif //#ifndef _DEF_ITEM_EQUIP_H