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
	quint32 ID;
	QString name;
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
};

const info_buff def_info_buff[6] = {
	{ 0, QStringLiteral("��"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, QStringLiteral("�����"), 0, 0, 2, 4, 0, 0, 0, 0, 0, 0 },
	{ 2, QStringLiteral("��ʥս����"), 2, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 3, QStringLiteral("ħ����"), 2, 4, 2, 4, 0, 0, 0, 0, 0, 0 },
	{ 4, QStringLiteral("�ٻ�����"), 1, 2, 1, 2, 2, 4, 0, 0, 0, 0 },
	{ 5, QStringLiteral("�ٻ�����"), 2, 3, 2, 3, 0, 0, 0, 0, 3, 5 }
};

struct Info_skill {
	skillID ID;
	QString name;
	QPixmap icon1;
	QPixmap icon2;
	quint32 level;		//�����б��б�ʾ���ȼ�����ɫ�����д���ǰ�ȼ�
	quint32 spell[3];
	quint32 cd;
	quint32 times;
	quint32 damage[3];	//0��1�����ܼӳ� 1:2�����ܼӳ� 2:3�����ܼӳ�
	quint32 buff;
	quint32 buff_time;
	QString descr;
};

struct roleSkill
{
	skillID id;
	quint32 level;
};

typedef QVector<roleSkill> VecRoleSkill;

struct Info_jobAdd {
	quint32 level;			//�ȼ�
	quint32 hp;				//Ѫ
	quint32 mp;				//��
	quint32 dc1;			//����_����
	quint32 dc2;			//����_����
	quint32 mc1;			//ħ��
	quint32 mc2;			//ħ��
	quint32 sc1;			//����
	quint32 sc2;			//����
	quint32 ac1;			//���
	quint32 ac2;			//���
	quint32 mac1;			//ħ��
	quint32 mac2;			//ħ��
};
typedef QMap<quint32, QVector<Info_jobAdd>> mapJobAdd;

struct RoleInfo {
	QString name;			//��ɫ��
	quint32 vocation;		//ְҵ
	quint32 gender;			//�Ա�
	quint64 coin;			//���
	quint64 gold;			//Ԫ��
	quint64 reputation;		//����
	quint64 exp;			//��ǰ����ֵ
	quint64 lvExp;			//��ǰ�������辭��ֵ
	quint32 level;			//�ȼ�
	quint32 hp;				//Ѫ
	quint32 mp;				//��
	quint32 dc1;			//����_����
	quint32 dc2;			//����_����
	quint32 mc1;			//ħ��
	quint32 mc2;			//ħ��
	quint32 sc1;			//����
	quint32 sc2;			//����
	quint32 ac1;			//���
	quint32 ac2;			//���
	quint32 mac1;			//ħ��
	quint32 mac2;			//ħ��
	quint32 intervel;		//�������
	quint32 luck;			//����
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
	quint32 reserved4[100];
};

#endif //#ifndef _ROLEDEFINE_H

