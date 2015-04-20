#ifndef SHOP_H
#define SHOP_H

#include <QWidget>
#include "ui_city_drugs.h"
#include "ItemDefine.h"
#include "RoleDefine.h"

class shop : public QWidget
{
	Q_OBJECT

public:
	shop(qint32 type, RoleInfo *roleInfo, MapItem *bag_item);
	~shop();

private:
	//���ڱ��ؼ��Ĵ�С
	void AdjustTableWidget(void);
	//��ʾ�����۵���
	bool DisplayItemList(void);

	

private slots:
	void cellClicked(int row, int column);

private:
	Ui::drugs ui;

	qint32 m_ShopType;
	RoleInfo *myRole;
	MapItem *m_bag_item;
};

#endif // SHOP_H
