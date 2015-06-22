#ifndef _ROLEDEFINE_H
#define _ROLEDEFINE_H

#include <QPixmap>
#include "def_DataType.h"
#include "def_System_para.h"
#include "def_item_equip.h"


const QString def_vocation[4] = {
	QStringLiteral("��ְҵ"),
	QStringLiteral("սʿ"),
	QStringLiteral("��ʦ"),
	QStringLiteral("��ʿ")
};


struct info_buff {
	qint32 ID;
	QString name;
	qint32 time;
	qint32 rhp;
	qint32 ac;
	qint32 mac;
};
typedef QVector<info_buff> vecBuff;

struct realBuff
{
	quint32 id;
	QString name;
	QPixmap icon;
	qint32 time;
	qint32 rhp;
	qint32 ac;
	qint32 mac;
};

struct Info_skill {
	skillID ID;
	QString name;
	QPixmap icon;
	quint32 type;
	quint32 level;		//�����б��б�ʾ���ȼ�����ɫ�����д���ǰ�ȼ�
	quint32 spell[3];
	quint32 cd;
	quint32 times;
	quint32 damage[3];	//0��1�����ܼӳ� 1:2�����ܼӳ� 2:3�����ܼӳ�
	quint32 buff;
	QString descr;
};
struct skill_fight
{
	skillID id;
	QString name;
	QPixmap icon;
	qint32 type;
	qint32 level;
	qint32 spell;
	qint32 cd;
	qint32 cd_c;
	qint32 times;
	qint32 damage;
	qint32 buff;
};
struct roleSkill
{
	skillID id;
	quint32 level;
};

typedef QVector<roleSkill> VecRoleSkill;

struct Info_jobAdd {
	qint32 level;		//�ȼ�
	qint64 exp;			//�ȼ����辭��
	qint32 hp;			//Ѫ
	qint32 mp;			//��
	qint32 dc1;			//����_����
	qint32 dc2;			//����_����
	qint32 mc1;			//ħ��
	qint32 mc2;			//ħ��
	qint32 sc1;			//����
	qint32 sc2;			//����
	qint32 ac1;			//���
	qint32 ac2;			//���
	qint32 mac1;		//ħ��
	qint32 mac2;		//ħ��
};
typedef QMap<quint32, QVector<Info_jobAdd>> mapJobAdd;

struct RoleInfo {
	char name[128];			//��ɫ��
	quint32 vocation;		//ְҵ
	quint32 gender;			//�Ա�
	quint64 coin;			//���
	quint64 gold;			//Ԫ��
	quint64 reputation;		//����
	quint64 exp;			//��ǰ����ֵ
	quint64 lvExp;			//��ǰ�������辭��ֵ
	quint32 level;			//�ȼ�
//	quint32 hp;				//Ѫ
	quint8 hp_1;
	quint8 mark_1;
	quint8 hp_2;
	quint8 mark_2;
	quint8 hp_3;
	quint8 mark_3;
	quint8 hp_4;
	quint8 mark_4;
//	quint32 mp;				//��
	quint8 mp_1;
	quint8 mark_5;
	quint8 mp_2;
	quint8 mark_6;
	quint8 mp_3;
	quint8 mark_7;
	quint8 mp_4;
	quint8 mark_8;
//	quint32 dc1;			//����_����
//	quint32 dc2;			//����_����
	quint8 dc1_1;
	quint8 mark_9;
	quint8 dc1_2;
	quint8 mark_10;
	quint8 dc1_3;
	quint8 mark_11;
	quint8 dc1_4;
	quint8 mark_12;
	quint8 dc2_1;
	quint8 mark_13;
	quint8 dc2_2;
	quint8 mark_14;
	quint8 dc2_3;
	quint8 mark_15;
	quint8 dc2_4;
	quint8 mark_16;
//	quint32 mc1;			//ħ��
//	quint32 mc2;			//ħ��
	quint8 mc1_1;
	quint8 mark_17;
	quint8 mc1_2;
	quint8 mark_18;
	quint8 mc1_3;
	quint8 mark_19;
	quint8 mc1_4;
	quint8 mark_20;
	quint8 mc2_1;
	quint8 mark_21;
	quint8 mc2_2;
	quint8 mark_22;
	quint8 mc2_3;
	quint8 mark_23;
	quint8 mc2_4;
	quint8 mark_24;
//	quint32 sc1;			//����
//	quint32 sc2;			//����
	quint8 sc1_1;
	quint8 mark_25;
	quint8 sc1_2;
	quint8 mark_26;
	quint8 sc1_3;
	quint8 mark_27;
	quint8 sc1_4;
	quint8 mark_28;
	quint8 sc2_1;
	quint8 mark_29;
	quint8 sc2_2;
	quint8 mark_30;
	quint8 sc2_3;
	quint8 mark_31;
	quint8 sc2_4;
	quint8 mark_32;
//	quint32 ac1;			//���
//	quint32 ac2;			//���
	quint8 ac1_1;
	quint8 mark_33;
	quint8 ac1_2;
	quint8 mark_34;
	quint8 ac1_3;
	quint8 mark_35;
	quint8 ac1_4;
	quint8 mark_36;
	quint8 ac2_1;
	quint8 mark_37;
	quint8 ac2_2;
	quint8 mark_38;
	quint8 ac2_3;
	quint8 mark_39;
	quint8 ac2_4;
	quint8 mark_40;
//	quint32 mac1;			//ħ��
//	quint32 mac2;			//ħ��
	quint8 mac1_1;
	quint8 mark_41;
	quint8 mac1_2;
	quint8 mark_42;
	quint8 mac1_3;
	quint8 mark_43;
	quint8 mac1_4;
	quint8 mark_44;
	quint8 mac2_1;
	quint8 mark_45;
	quint8 mac2_2;
	quint8 mark_46;
	quint8 mac2_3;
	quint8 mark_47;
	quint8 mac2_4;
	quint8 mark_48;
//	quint32 intervel;		//�������
	quint8 intervel_1;
	quint8 mark_49;
	quint8 intervel_2;
	quint8 mark_50;
	quint8 luck;			//����
	quint8 mark_51;
	VecRoleSkill skill;	//ս��ʱʹ�õļ���
};

struct roleAddition
{
	quint32 strength;		//���Ե㣺����
	quint32 wisdom;			//���Ե㣺�ǻ�
	quint32 spirit;			//���Ե㣺����
	quint32 life;			//���Ե㣺����
	quint32 agility;		//���Ե㣺����
	quint32 potential;		//Ǳ�ܵ㡣���ڷ��䵽��Ӧ���Ե㡣

	Info_Equip vecEquip[MaxEquipCountForRole];	//��ɫ���ϵ�װ��

	quint32 taskStep;
	quint32 reserved1[100];
	quint32 reserved2[100];
	quint32 reserved3[100];
	quint32 reserved4[80];
};

#endif //#ifndef _ROLEDEFINE_H

