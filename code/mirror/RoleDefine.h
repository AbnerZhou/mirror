#ifndef _ROLEDEFINE_H
#define _ROLEDEFINE_H

#include <QPixmap>
#include "def_DataType.h"
#include "def_System_para.h"
#include "def_item_equip.h"
#include "def_skill.h"

const QString def_vocation[4] = {
	QStringLiteral("��ְҵ"),
	QStringLiteral("սʿ"),
	QStringLiteral("��ʦ"),
	QStringLiteral("��ʿ")
};

const QString def_xiulian[12] = {
	QStringLiteral("����"),
	QStringLiteral("����"),
	QStringLiteral("�ᵤ"),
	QStringLiteral("ԪӤ"),
	QStringLiteral("����"),
	QStringLiteral("Ӥ��"),
	QStringLiteral("�ʶ�"),
	QStringLiteral("����"),
	QStringLiteral("��ʵ"),
	QStringLiteral("����"),
	QStringLiteral("����"),
	QStringLiteral("����")
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

typedef QMap<skillID, roleSkill2> MapRoleSkill;

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

#endif //#ifndef _ROLEDEFINE_H

