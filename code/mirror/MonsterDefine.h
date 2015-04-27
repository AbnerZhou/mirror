#ifndef _MONSTERDEFINE_H
#define _MONSTERDEFINE_H

#include <QImage>
#include "def_DataType.h"

struct MonsterInfo {
	monsterID ID;
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
	quint32	interval;	//�������
};

struct Info_Distribute {
	mapID ID;
	QVector<quint32> normal;
	QVector<quint32> boss;
};

struct Rational {
	itemID ID;
	//quint32 num;		//ʼ��Ϊ1
	quint32 den;
};
typedef QList<Rational> ListDrop;
typedef QMap<monsterID, ListDrop>  mapDrop;

#endif //#ifndef _MONSTERDEFINE_H

