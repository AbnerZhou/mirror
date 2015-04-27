#ifndef _DEF_ITEM_EQUIP_H
#define _DEF_ITEM_EQUIP_H

#include <QUuid>
#include <QPixmap>

#include "def_DataType.h"

const QString equip_need_info[4] = {
	QStringLiteral(" ��Ҫ�ȼ�"),
	QStringLiteral(" ��Ҫ����"),
	QStringLiteral(" ��Ҫħ��"),
	QStringLiteral(" ��Ҫ����")
};

struct Info_equip {
	itemID ID;
	QString name;
	QPixmap icon;
	quint32 acc;	//׼ȷ
	quint32 luck;	
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
	QString msg;
};

struct EquitExtra {
	itemID ID;
	QUuid uuid;
	quint32 level;
};
typedef QList<itemID> ListEquip;

#endif //#ifndef _DEF_ITEM_EQUIP_H