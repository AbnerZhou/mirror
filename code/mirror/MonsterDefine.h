#ifndef _MONSTERDEFINE_H
#define _MONSTERDEFINE_H

#include <QImage>
#include <QIcon>
#include "def_DataType.h"

struct MonsterInfo {
	monsterID ID;
	QString name;
	QImage Head;
	qint32 level;
	qint32 exp;
	qint32 hp;			//�������ֵ
	qint32 mp;			//���ħ��ֵ
	qint32 DC1;		//�﹥����
	qint32 DC2;		//�﹥����
	qint32 MC1;		//ħ������
	qint32 MC2;		//ħ������
	qint32 AC;			//���
	qint32 MAC;		//ħ��
	qint32	interval;	//�������
};

struct Info_Distribute {
	mapID ID;
	QString name;
	QIcon img;
	qint32 need_lv;
	qint32 expend_rep;
	qint32 expend_item;
	QVector<monsterID> normal;
	QVector<monsterID> boss;
};

struct Rational {
	itemID ID;
	//quint32 num;		//ʼ��Ϊ1
	quint32 den;
};
typedef QList<Rational> ListDrop;
typedef QMap<monsterID, ListDrop>  mapDrop;

#endif //#ifndef _MONSTERDEFINE_H

