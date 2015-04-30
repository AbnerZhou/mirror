#ifndef FIGHT_FIGHT_H
#define FIGHT_FIGHT_H

#include <QDialog>

#include "ui_fight_fight.h"
#include "RoleDefine.h"
#include "MonsterDefine.h"
#include "ItemDefine.h"
#include "def_item_equip.h"

#include "fight_info.h"

const quint32 Max_monster = 15;

class fight_fight : public QDialog
{
	Q_OBJECT

public:
	fight_fight(QWidget* parent, qint32 id, RoleInfo *info, MapItem *bag_item, ListEquip *bag_equip);
	~fight_fight();
	void setMapID(qint32 id) { m_mapID = id; }

private:
	void timerEvent(QTimerEvent *event);

private slots:
	void on_btn_quit_clicked(void);
	void on_btn_start_clicked(void);
	void on_btn_statistics_clicked(void);
	
	void on_checkBox_auto_clicked(void);

private:
	//��ʼ������
	void InitUI(void);
	//��ȡ��ɫ������Ϣ��Ȼ����ݹ�������������ħ�������ٵ������Ϣ������ʾ�����档
	void Cacl_Display_Role_Value();

	//��ʾ��ǰѡ��������Ϣ������
	void Display_CurrentMonsterInfo();
	void DisplayConciseFightInfo(void);
	void DisplayDropBasic(quint32 nDropExp, quint32 nDropCoin, quint32 nDropRep);
	void CalcDropItemsAndDisplay(monsterID id);

	//���ص��߱����еĲ���ҩƷ���Զ���ҩ�����б���
	void LoadItem();
	const Info_Item* FindItem(quint32 ID);
	const Info_Item* FindItem(const QString &name);

	//Ϊ��ǰ��ͼ�������
	bool AssignMonster(QVector<MonsterInfo> normalList, QVector<MonsterInfo> bossList, QVector<Info_Distribute> Distribute);

	//�غ�
	void Action_role(void);
	void Action_monster(void);

	//������ÿ�غ�ֻ��ִ������һ������
	void Step_role_UsingItem_hp(void);
	void Step_role_UsingItem_mp(void);
	void Step_role_NormalAttack(void);
	void Step_role_SkillAttack(void);

	//�����Զ���ҩ�����б�ĵ�����ʾ�ı�
	QString Generate_ItemComboBox_Text(const QString &name, const QString &type, quint32 value, quint32 count);
	//���ɵ��ι���������Ϣ�ĵ�����ʾ�ı�
	QString Generate_Display_LineText(const QString &str1, const QString &skill, const QString &str2, quint32 damage);

private:
	Ui::fight_fight ui;
	static bool bCheckHp, bCheckMp, bCheckQuickFight, bCheckConcise, bCheckFindBoss;

	qint32 m_mapID;
	RoleInfo *myRole;
	MapItem *m_bag_item;
	ListEquip *m_bag_equip;

	fight_info *m_dlg_fightInfo;

	MonsterInfo *monster_cur;
	quint32 monster_normal_assign[Max_monster], monster_boss_assign[Max_monster], monster_normal_count, monster_boss_count;

	qint32 role_hp_c, role_rhp, role_mp_c, role_rmp;
	qint32 monster_cur_hp, monster_cur_rhp, monster_cur_mp, monster_cur_rmp;

	bool bFighting, bKeepFight, bBoss;
	qint32 nFightTimer, nDelayTimer, nShowStatusRound, nRound;
	qint32 nCount_attack, nCount_parry, nCount_item , nCount_count, nCount_exp, nCount_coin, nCount_rep;
	double time_remain_role, time_remain_monster, time_remain;

	time_t t_Count_start;
};

#endif // FIGHT_FIGHT_H
