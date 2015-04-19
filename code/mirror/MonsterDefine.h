#ifndef _MONSTERDEFINE_H
#define _MONSTERDEFINE_H

#include <QImage>

struct MonsterInfo {
	quint32 ID;
	QString name;
	QImage Head;
	qint32 level;
	qint32 exp;
	qint32 hp;			//�������ֵ
	qint32 mp;			//���ħ��ֵ
	quint32 DC1;		//�﹥����
	quint32 DC2;		//�﹥����
	quint32 MC1;		//ħ������
	quint32 MC2;		//ħ������
	quint32 AC;			//���
	quint32 MAC;		//ħ��
	quint32	interval;//�������
};

struct Info_Distribute {
	quint32 mapID;
	QVector<quint32> normal;
	QVector<quint32> boss;
};

#endif //#ifndef _MONSTERDEFINE_H

