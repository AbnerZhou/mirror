#ifndef ROLE_H
#define ROLE_H

#include <QWidget>
#include "ui_role.h"
#include "mytabframe.h"
#include "item_itembag.h"
#include "item_equipbag.h"
#include "item_equipstorage.h"

class role : public myTabFrame
{
	Q_OBJECT

public:
	role(RoleInfo *roleInfo, VecRoleSkill *skill, MapItem *bag_item, MapItem *storage_item, ListEquip *bag_equip, ListEquip *storage_equip);
	~role();
	const static qint32		MaxLv = 100;

public:
	virtual void updateRoleInfo(void);
	void UpdateEquipInfo(void);
	void UpdateItemInfo(void);

protected:
	//QLabel������Ӧclicked, rightClicked���¼�����Ҫ��eventFilter������
	bool eventFilter(QObject *obj, QEvent *ev);

private:
	//��ʾ��ɫ��Ϣ������
	void DisplayRoleInfo(void);
	//��ʾ��ɫ����װ��
	void DisplayEquip();
	//��ʾ����װ������ϸ����
	void DisplayEquipInfo(QPoint pos, const Info_basic_equip *BasicInfo, const Info_Equip *Equip);
	
	//�ۼӵ�ǰװ�������Լӳɵ������Լӳ���Ϣ��
	void EquipAddPara_Add(const Info_basic_equip &equip, const EquipExtra &extra, quint32 lvUp);
	void EquipAddPara_Sub(const Info_basic_equip &equip, const EquipExtra &extra, quint32 lvUp);

private slots:
	void on_btn_mirror_save_clicked();
	void on_btn_role_strength_clicked();
	void on_btn_role_wisdom_clicked();
	void on_btn_role_spirit_clicked();
	void on_btn_role_life_clicked();
	void on_btn_role_agility_clicked();
	void on_btn_role_lvUp_clicked();

	void on_btn_skill_clicked();
	void on_btn_task_clicked();

	void on_checkBox_autoSave_clicked(void) { emit autoSave(ui.checkBox_autoSave->isChecked()); }
	void on_checkBox_bkSound_clicked(void) { emit bkSound(ui.checkBox_bkSound->isChecked()); }
	
	//����װ��
	void on_wearEquip(quint32 ID_for_new, quint32 index);
	void on_usedItem(quint32 ID);

signals:
	void mirrorSave(void);
	void autoSave(bool);
	void bkSound(bool);

private:
	Ui::role ui;

	item_itemBag m_tab_itemBag;
	item_equipBag m_tab_equipBag;
	Item_equipStorage m_tab_equipStorage;

//	Dlg_Detail *m_dlg_detail;

	QVector<QLabel *> EquipmentGrid;
	
	RoleInfo *myRole;
	VecRoleSkill *m_skill_study;
	MapItem *m_bag_item;
	MapItem *m_storage_item;
	ListEquip *m_bag_equip;
	ListEquip *m_storage_equip;

	Info_basic_equip equip_add;
};

#endif // ROLE_H
