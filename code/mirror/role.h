#ifndef ROLE_H
#define ROLE_H

#include <QWidget>
#include "ui_role.h"
#include "mytabframe.h"
#include "bag_item.h"
#include "storage_item.h"

#include "def_item_equip.h"
#include "equipinfo.h"

class role : public myTabFrame
{
	Q_OBJECT

public:
	role(RoleInfo *roleInfo, MapItem *bag_item, MapItem *storage_item);
	~role();
	const static qint32		MaxLv = 100;

public:
	virtual void updateRoleInfo(void);

protected:
	//QLabel������Ӧclicked, rightClicked���¼�����Ҫ��eventFilter������
	bool eventFilter(QObject *obj, QEvent *ev);

private:
	//���ش浵
	void LoadRole(void);
	//�����浵
//	bool CreateRole(void);
	//��ʾ��ɫ��Ϣ������
	void DisplayRoleInfo(void);
	//��ʾ��ɫ����װ��
	void DisplayEquip();
	//��ʾ����װ������ϸ����
	void DisplayEquipInfo(QPoint pos, const Info_equip &equip);
	//����װ��ID��ȫ��װ���б��в�ѯ��ָ��װ������ϸ����
	const Info_equip *FineEquip(quint32 id);
	//������������������Ϣ���ݿ�
	void LoadExpSetting();
	//�ۼӵ�ǰװ�������Լӳɵ������Լӳ���Ϣ��
	void Add_EquipAddPara(const Info_equip &equip);
	void Sub_EquipAddPara(const Info_equip &equip);

private slots:
	void on_btn_mirror_save_clicked();
	void on_btn_role_strength_clicked();
	void on_btn_role_wisdom_clicked();
	void on_btn_role_spirit_clicked();
	void on_btn_role_life_clicked();
	void on_btn_role_agility_clicked();
	void on_btn_role_lvUp_clicked();

private:
	Ui::role ui;

	bag_item m_tab_bagItem;
	storage_item m_tab_storageItem;
	EquipInfo *m_dlg_equipInfo;

	QVector<QLabel *> EquipmentGrid;
	
	RoleInfo *myRole;
	MapItem *m_bag_item;
	MapItem *m_storage_item;

	Info_equip equip_add;
};

#endif // ROLE_H
