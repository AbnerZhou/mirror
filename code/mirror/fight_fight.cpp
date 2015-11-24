#include "fight_fight.h"
#include <QFile>
#include <QMessageBox>
#include <QThread>
#include <time.h>
#include <QKeyEvent>

#include "Item_Base.h"
#include "def_System_para.h"
#include "mirrorlog.h"
#include "CommonComponents.h"

#include "BasicMath.h"

//���岢��ʼ����̬���ݳ�Ա��
bool fight_fight::bCheckHp = false;
bool fight_fight::bCheckMp = false;
bool fight_fight::bCheckConcise = false;
bool fight_fight::bCheckFindBoss = false;
qint32 fight_fight::FilterAdd = 0;
qint32 fight_fight::FilterLvl = 0;
qint32 fight_fight::limit_rhp = 50;
qint32 fight_fight::limit_rmp = 50;

extern QVector<Info_jobAdd> g_JobAddSet;
extern QMap<skillID, Info_SkillBasic> g_SkillBasic;
extern QMap<qint32, Info_SkillDamage> g_SkillDamage;
extern QMap<qint32, Info_SkillBuff> g_SkillBuff;
extern QMap<qint32, Info_SkillSummon> g_SkillSummon;
extern QMap<itemID, Info_Item> g_ItemList;
extern QMap<itemID, Info_basic_equip> g_EquipList;
extern QMap<mapID, Info_Distribute> g_MonsterDistribute;
extern QMap<monsterID, MonsterInfo> g_MonsterInfo;
extern mapDrop	g_mapDropSet;

fight_fight::fight_fight(QWidget* parent, qint32 id, CPlayer *const w_player)
	: QDialog(parent), m_MainFrame(parent), m_mapID(id), player(w_player)
{
	ui.setupUi(this);
	InitUI();

	m_bag_item = player->get_bag_item();
	m_bag_equip = player->get_bag_equip();

	DisplayRoleinfo();
	DisplayRoleParameter();
	InitDrug_hp();
	InitDrug_mp();

	AssignMonster(g_MonsterInfo, g_MonsterDistribute);

	bFighting = bEnableBoss = false;
	bCheckHp = bCheckMp = true;
	nShowStatusRound = 0;
	nSkillIndex = 0;
	m_dlg_fightInfo = nullptr;

	nTimeOutTime = 5 * 60 * 1000;

	pet.set_hp_c(-1);
	PetDead();

	nFightTimer = startTimer(nFightInterval);
	ct_start = QDateTime::currentDateTime();
	nCount_normalMonster = nCount_boss = nCount_exp = nCount_coin = nCount_rep = 0;
	nElapse_pre_boss = 0;
	nCount_fail = nCount_timeout = 0;

	nXSpeedTimer = startTimer(nXSpeedInvterval);
	xSpeedTime.start();
	nXSpeedCount = 0;

	connect(ui.filter_add, SIGNAL(currentIndexChanged(int)), this, SLOT(pickFilterChange(int)));
	connect(ui.filter_level, SIGNAL(textChanged(const QString &)), this, SLOT(on_filter_level_textEdited(const QString &)));
}

fight_fight::~fight_fight()
{

}

void fight_fight::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		on_btn_quit_clicked();
	}
}
void fight_fight::on_btn_quit_clicked(void)
{
	limit_rhp = ui.edit_hp->text().toInt();
	limit_rmp = ui.edit_mp->text().toInt();
	close();
}
void fight_fight::on_comboBox_hp_currentIndexChanged(int index)
{
	QString strTmp = ui.comboBox_hp->currentText();
	QStringList strList = strTmp.split(" ");

	const Info_Item *itemItem = FindItem(strList.at(0));
	if (itemItem != nullptr)
	{
		SelectDrug_hp = itemItem->ID;
	}
	else
	{
		SelectDrug_hp = 0;
	}	
}
void fight_fight::on_comboBox_mp_currentIndexChanged(int index)
{
	QString strTmp = ui.comboBox_mp->currentText();
	QStringList strList = strTmp.split(" ");

	const Info_Item *itemItem = FindItem(strList.at(0));
	if (itemItem != nullptr)
	{
		SelectDrug_mp = itemItem->ID;
	}
	else
	{
		SelectDrug_mp = 0;
	}
}

void fight_fight::on_btn_statistics_clicked(void)
{
	if (m_dlg_fightInfo == nullptr)
	{
		m_dlg_fightInfo = new fight_info(this);
	}
	m_dlg_fightInfo->move(QPoint(730, 370));

	qint32 time = ct_start.secsTo(QDateTime::currentDateTime()) / 60;
	m_dlg_fightInfo->updateInfo(time, nCount_fail, nCount_timeout, nCount_normalMonster, nCount_boss, nCount_exp, nCount_coin, nCount_rep);
	m_dlg_fightInfo->show();
}

void fight_fight::on_checkBox_concise_clicked(void)
{ 
	if (ui.checkBox_concise->isChecked())
	{
		ui.display_Spoils->setVisible(true);
		ui.display_Fighting->setVisible(false);

		bCheckConcise = true;
	}
	else
	{
		ui.display_Spoils->setVisible(false);
		ui.display_Fighting->setVisible(true);

		bCheckConcise = false;
	}
}

void fight_fight::on_filter_level_textEdited(const QString & text)
{
	if (!text.isEmpty())
	{
		FilterLvl = text.toUInt();	
	}
	else
	{
		FilterLvl = 0;
		ui.filter_level->setText(QString::number(FilterLvl));
	}
}
void fight_fight::pickFilterChange(int index)
{
	FilterAdd = index;
}

void fight_fight::InitUI()
{
	bCheckAuto = false;

	ui.progressBar_role_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_role_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");

	ui.progressBar_monster_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_monster_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");

	ui.edit_hp->setText(QString::number(limit_rhp));
	ui.edit_mp->setText(QString::number(limit_rmp));
 	ui.checkBox_concise->setChecked(bCheckConcise);
	ui.checkBox_boss->setChecked(bCheckFindBoss);
	ui.filter_add->setCurrentIndex(FilterAdd);
	ui.filter_level->setText(QString::number(FilterLvl));

	ui.display_Fighting->document()->setMaximumBlockCount(500);
	ui.display_Spoils->document()->setMaximumBlockCount(50);
	on_checkBox_concise_clicked();

	QLabel *buffs_player[MaxBuffCount] = { ui.lbl_role_buff_0, ui.lbl_role_buff_1, ui.lbl_role_buff_2, ui.lbl_role_buff_3 };
	player->bingWidget(ui.edit_role_name, ui.edit_role_level, ui.label_role_head, buffs_player, MaxBuffCount, ui.progressBar_role_hp, ui.progressBar_role_mp);
	
	QLabel *buffs_pet[MaxBuffCount] = { ui.lbl_pet_buff_0, ui.lbl_pet_buff_1, ui.lbl_pet_buff_2, ui.lbl_pet_buff_3 };
	pet.bingWidget(ui.edit_pet_name, ui.edit_pet_level, ui.label_pet_head, buffs_pet, MaxBuffCount, ui.progressBar_pet_hp, ui.progressBar_pet_mp);
	
	QLabel *buffs_mon[MaxBuffCount] = { ui.lbl_monster_buff_0, ui.lbl_monster_buff_1, ui.lbl_monster_buff_2, ui.lbl_monster_buff_3 };
	monster.bingWidget(ui.edit_monster_name, ui.edit_monster_level, ui.label_monster_head, buffs_mon, MaxBuffCount, ui.progressBar_monster_hp, ui.progressBar_monster_mp);

	ui.progressBar_pet_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_pet_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");
}

void fight_fight::DisplayRoleinfo()
{
	ui.edit_role_name->setText(player->get_name());

	QString VocImg = QString(":/mirror/Resources/ui/f_0_%1.png").arg(player->get_voc() + 1);
	ui.lbl_role_vocation->setPixmap(QPixmap(VocImg));

	qint32 headNo = ((player->get_voc() - 1) * 2 + player->get_gender());
	QString headImg = QString(":/mirror/Resources/head/%1.png").arg(headNo);
	ui.label_role_head->setPixmap(QPixmap(headImg));

	//�����������б��е�����ȡ���һ����ܣ��Խ�Լ�������õ�Ч��	
	MapRoleSkill *m_skill = player->get_skill();
	int32_t nTmp = 1;
	for (int32_t i = 0; i < m_skill->size(); i++)
	{
		for (auto iterRole = m_skill->constBegin(); iterRole != m_skill->constEnd(); iterRole++)
		{
			if (iterRole->usdIndex == nTmp)
			{
				fightingSkill.append(skill_fight(g_SkillBasic.value(iterRole->id), iterRole->level));
				nTmp++;
			}
		}
	}
	if (fightingSkill.size() == 0)
	{
		fightingSkill.append(skill_fight(g_SkillBasic.first(),1));
	}
}

const Info_Item* fight_fight::FindItem(itemID id)
{
	if (g_ItemList.contains(id))
		return &g_ItemList[id];
	else
		return nullptr;
}
const Info_Item* fight_fight::FindItem(const QString &name)
{
	foreach(const Info_Item &item, g_ItemList)
	{
		if (item.name == name)
		{
			return &item;
		}
	}
	return nullptr;
}

void fight_fight::InitDrug_hp()
{
	ui.comboBox_hp->clear();
	QString strTmp;
	for (auto iter = m_bag_item->constBegin(); iter != m_bag_item->constEnd(); iter++)
	{
		const Info_Item *itemItem = FindItem(iter.key());
		if (itemItem != nullptr && itemItem->level <= player->get_lv())
		{
			if (itemItem->type == et_immediate_hp)
			{
				strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("Ѫ"), itemItem->value, iter.value());
				ui.comboBox_hp->addItem(strTmp);
			}
		}
	}
	ui.comboBox_hp->setCurrentIndex(ui.comboBox_hp->count() - 1);
}
void fight_fight::InitDrug_mp()
{
	ui.comboBox_mp->clear();
	QString strTmp;
	for (auto iter = m_bag_item->constBegin(); iter != m_bag_item->constEnd(); iter++)
	{
		const Info_Item *itemItem = FindItem(iter.key());
		if (itemItem != nullptr && itemItem->level <= player->get_lv())
		{
			if (itemItem->type == et_immediate_mp)
			{
				strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("��"), itemItem->value, iter.value());;
				ui.comboBox_mp->addItem(strTmp);
			}
		}
	}
	ui.comboBox_mp->setCurrentIndex(ui.comboBox_mp->count() - 1);
}

bool fight_fight::AssignMonster(QMap<monsterID, MonsterInfo> monsters, QMap<mapID, Info_Distribute> Distribute)
{
	quint32 c;
	memset(monster_normal_assign, 0, Max_monster * sizeof(quint32));
	memset(monster_boss_assign, 0, Max_monster * sizeof(quint32));

	const Info_Distribute &dis = Distribute[m_mapID];

	c = 0;
	foreach(quint32 n, dis.normal)
	{
		monster_normal_assign[c++] = n;
	}
	monster_normal_count = c;

	c = 0;
	foreach(quint32 n, dis.boss)
	{
		monster_boss_assign[c++] = n;
	}
	if (monster_boss_assign[0] == 0)
	{
		monster_boss_count = 0;			//��Щ��ͼ��ˢ��BOSS
	}
	else
	{
		monster_boss_count = c;
	}

	return true;
}

void fight_fight::Display_CurrentMonsterInfo()
{
	//�����塢ħ���ֵ��
	ui.progressBar_monster_hp->setMaximum(monster.get_hp_max());
	ui.progressBar_monster_mp->setMaximum(monster.get_mp_max());
	//��ʾ��ǰ�塢ħ
	ui.progressBar_monster_hp->setValue(monster.get_hp_c());
	ui.progressBar_monster_mp->setValue(monster.get_mp_c());
	
	//����ͷ��
	ui.label_monster_head->setPixmap(QPixmap::fromImage(monster.get_head()));

	//������������
	ui.edit_monster_name->setText(monster.get_name());
	ui.edit_monster_level->setText(QStringLiteral("Lv:%1").arg(monster.get_lv()));
}

inline QString fight_fight::Generate_ItemComboBox_Text(const QString &name, const QString &type, quint32 value, quint32 count)
{
	QString strSplit = QStringLiteral("%1 %2:%3 ʣ:%4").arg(name).arg(type).arg(value).arg(count);
	return strSplit;
}
inline QString fight_fight::Generate_Display_LineText(const QString &str1, const QString &skill, const QString &str2, bool bLuck, bool bep, QList<qint32> listDamage)
{
	QString strTmp = QStringLiteral("%1ʹ��<font color=gray>%2</font>��").arg(str1).arg(skill);
	if (bLuck)
		strTmp += QStringLiteral("���ս��ף��, ");

	strTmp += QStringLiteral("��") + str2;
	
	if (bep)
		strTmp += QStringLiteral("���<font color = red>����</font>�˺�:<font color = magenta>");
	else
		strTmp += QStringLiteral("����˺�:<font color = magenta>");

	if (listDamage.size() == 0)
	{
		strTmp += "0";
	}
	for (qint32 i = 0; i < listDamage.size(); i++)
	{
		strTmp += QString::number(listDamage.at(i)) + " ";
	}

	strTmp += QStringLiteral("</font>");
	return strTmp;
}

QString fight_fight::Generate_Display_buffInfo(bool bLuck, const QString &SkillName, const realBuff &real)
{
	QString strTmp = QStringLiteral("<font color=DarkCyan>��</font>ʹ��:<font color=gray>%1</font>").arg(SkillName);
	if (bLuck)
		strTmp += QStringLiteral("�������Ů��͸�,");

	strTmp += QStringLiteral("  Ч������<font color=magenta>") + QString::number(real.time) + QStringLiteral("</font>�غ� ");
#ifdef _DEBUG
	strTmp += QString::number(real.rhp) + " " + QString::number(real.damage) + " " + QString::number(real.defense);
#endif // _DEBUG

	return strTmp;
}

void fight_fight::Step_role_UsingItem_hp(void)
{
	quint32 nTmp1;
	QString strTmp;

	const Info_Item *itemItem = FindItem(SelectDrug_hp);
	if (itemItem != nullptr)
	{
		//������Ӧ����������1
		m_bag_item->insert(SelectDrug_hp, m_bag_item->value(SelectDrug_hp) - 1); 
		strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("Ѫ"), itemItem->value, m_bag_item->value(SelectDrug_hp));
		ui.comboBox_hp->setItemText(ui.comboBox_hp->currentIndex(), strTmp);

		//���Ľ�ɫ״̬
		player->set_hp_c(player->get_hp_c() + itemItem->value);
		strTmp = QStringLiteral("<font color=green>��ʹ���ˣ�%1 </font>").arg(itemItem->name);
		ui.display_Fighting->append(strTmp);

		//��������Ѿ����꣬��ɾ����ǰ����,�����³�ʼ���б�
		if (m_bag_item->value(SelectDrug_hp) <= 0)
		{
			m_bag_item->remove(SelectDrug_hp);
			InitDrug_hp();
		}
	}
	else
	{	//�Ҳ�����Ӧ���ߣ�����Զ���Ѫ��ѡ��
		ui.checkBox_hp->setCheckState(Qt::Unchecked);
		bCheckHp = false;
	}
}
void fight_fight::Step_role_UsingItem_mp(void)
{
	quint32 nTmp1;
	QString strTmp;

	const Info_Item *itemItem = FindItem(SelectDrug_mp);
	if (itemItem != nullptr)
	{
		//������Ӧ����������1
		SelectDrug_mp = itemItem->ID;
		m_bag_item->insert(SelectDrug_mp, m_bag_item->value(SelectDrug_mp) - 1);
		strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("��"), itemItem->value, m_bag_item->value(SelectDrug_mp));
		ui.comboBox_mp->setItemText(ui.comboBox_mp->currentIndex(), strTmp);

		//���Ľ�ɫ״̬
		player->set_mp_c(player->get_mp_c() + itemItem->value);
		strTmp = QStringLiteral("<font color=green>��ʹ���ˣ�%1 </font>").arg(itemItem->name);
		ui.display_Fighting->append(strTmp);

		//��������Ѿ����꣬��ɾ����ǰ���ߣ������³�ʼ���б�
		if (m_bag_item->value(SelectDrug_mp) <= 0)
		{
			m_bag_item->remove(SelectDrug_mp);
			InitDrug_mp();
		}
	}
	else
	{	//�Ҳ�����Ӧ���ߣ������ѡ��
		ui.checkBox_mp->setCheckState(Qt::Unchecked);
		bCheckMp = false;
	}
}

void fight_fight::Step_role_Skill(void)
{
	bool bUsedSkill = false;
	qint32 nTmp;

	for (qint32 i = 0; i < fightingSkill.size(); i++)
	{
		nTmp = nSkillIndex;
		const skill_fight &skill = fightingSkill.at(nSkillIndex++);
		if (nSkillIndex >= fightingSkill.size())
		{
			nSkillIndex = 0;
		}

		if (player->get_mp_c() <= 0)
		{
			QString strTmp = QStringLiteral("<font color=red>�������㣬�޷�ʩ�ż���.</font>");
			ui.display_Fighting->append(strTmp);
			return;
		}	

		if (skill.cd_c <= 0)
		{
			switch (skill.type)
			{
			case 1: bUsedSkill = MStep_role_Attack(skill); break;
			case 2: bUsedSkill = MStep_role_Buff(skill); break;
			case 3: bUsedSkill = MStet_role_Debuff(skill); break;
			case 4: bUsedSkill = SummonPet(skill); break;
			default: bUsedSkill = true; break;
			}
		}
		if (bUsedSkill)
		{
			fightingSkill[nTmp].cd_c = fightingSkill[nTmp].cd;
			player->set_mp_c(player->get_mp_c() - skill.spell);
			break;
		}
	}
	if (!bUsedSkill)
	{
		ui.display_Fighting->append(QStringLiteral("�޿��ü���"));
	}
}
bool fight_fight::MStep_role_Buff(const skill_fight &skill)
{
	quint32 nTmp, nTmp1;
	realBuff real;
	bool bLuck = false;

	if (player->HasBuff(skill.no) && pet.HasBuff(skill.no))
	{
		return false;	//no need to used skill
	}
	
	if (!Init_realBuff(skill, bLuck, real))
	{
		//has error, can't use the skill
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_para);
		return false;
	}

	if (real.rhp > 0 && 
		0.8 < 1.0 * player->get_hp_c() / player->get_hp_max() &&
		(pet.wasDead() || 0.8 < 1.0 * pet.get_hp_c() / pet.get_hp_max()))
	{
		return false;				//�������Լ�����Ѫ������80%����ʹ�ûָ���buff��
	}
	player->appendBuff(real);
	pet.appendBuff(real);

	ui.display_Fighting->append(Generate_Display_buffInfo(bLuck, skill.name, real));
	return true;
}

bool fight_fight::MStet_role_Debuff(const skill_fight &skill)
{
	quint32 nTmp, nTmp1;
	realBuff real;
	bool bLuck = false;

	if (monster.HasBuff(skill.no))
	{
		return false;	//no need to used skill
	}

	if (!Init_realBuff(skill, bLuck, real))
	{
		//has error, can't use the skill
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_para);
		return false;
	}

	monster.appendBuff(real);

	ui.display_Fighting->append(Generate_Display_buffInfo(bLuck, skill.name, real));
	return true;
}

bool fight_fight::Init_realBuff(const skill_fight &skill, bool &bLuck, realBuff &real)
{
	bool res = false;
	qint32 nTmp;

	int32_t flag;
	if (skill.type == 2) {
		flag = 1;
	} else {
		flag = -1;
	}

	const Info_SkillBuff &sb = g_SkillBuff.value(skill.no);

	nTmp = player->GetAttack(player->get_voc(), bLuck);

	real.id = skill.no;
	real.name = skill.name;
	real.icon = skill.icon;
	real.time = nTmp * sb.time * skill.level / 100;
	real.rhp = flag * nTmp * sb.rhp / 100;
	real.damage = flag * nTmp * sb.damage / 100;
	real.defense = flag * nTmp * sb.defense / 100;

	real.speed = player->get_intervel() * sb.speed / 100;

	res = true;

	return res;
}

bool fight_fight::MStep_role_Attack(const skill_fight &skill)
{
	qint32 nDamage, nTmp, nTmp1, nTmp2, m_ac,m_mac;
	bool bTmp, bep = false, bLuck = false;
	QList<qint32> ListDamage;

	player->attack(&monster, skill.id, skill.level, bLuck, &ListDamage);
	ui.display_Fighting->append(Generate_Display_LineText(
		QStringLiteral("<font color=DarkCyan>��</font>"), 
		skill.name, 
		QStringLiteral("<font color=DarkRed>%1</font>").arg(monster.get_name()), 
		bLuck, bep, ListDamage));

	return true;
}

void fight_fight::CalcDropItemsAndDisplay(monsterID id)
{
	if (!g_mapDropSet.contains(id))	{
		return;
	}

	Info_Equip DropEquip;
	double dTmp1, dTmp2;
	quint32 nTmp;
	const ListDrop &LD = g_mapDropSet.value(id);
	QStringList List_Pick, List_Drop;
	QString strTmp;
	foreach(const Rational &rRat, LD)
	{
		dTmp1 = 1.0 * qrand() / RAND_MAX;
		dTmp2 = 1.0 * (rRat.den - 1) / rRat.den;
		if (dTmp1 > dTmp2)
		{
			if (rRat.ID > g_itemID_start_equip && rRat.ID <= g_itemID_stop_equip)
			{
				//����װ��,����ʰȡ������ʰȡ����ȡ������
				CreateEquip(rRat.ID, DropEquip);
				const Info_basic_equip *equip = Item_Base::GetEquipBasicInfo(DropEquip.ID);
				List_Drop.append(equip->name);

				if (m_bag_equip->size() < g_bag_maxSize && (DropEquip.extraAmount >= FilterAdd) && (equip->lv >= FilterLvl))
				{
					List_Pick.append(equip->name);
					m_bag_equip->append(DropEquip);
				}
			}
			else
			{
				//�������
				const Info_Item *item = Item_Base::FindItem_Item(rRat.ID);
				List_Drop.append(item->name);

				List_Pick.append(item->name);
				m_bag_item->insert(rRat.ID, m_bag_item->value(rRat.ID) + 1);
			}
		}
	}

	if (List_Drop.size() > 0)
	{
		strTmp.clear();
		foreach(const QString &s, List_Drop)
		{
			strTmp += s + ", ";
		}
		ui.display_Fighting->append(QStringLiteral("<font color=white>������������ɢ���ڵ��ϣ���ϸһ������%1 �ö�öడ��</font>").arg(strTmp));
	}

	if (List_Pick.size() > 0)
	{
		strTmp.clear();
		foreach(const QString &s, List_Pick)
		{
			strTmp += s + ", ";
		}
		ui.display_Spoils->append(QStringLiteral("<font color=white>��� %1 ʵ����ǿ�˼��֡�</font>").arg(strTmp));
	}
}

void fight_fight::CreateEquip(itemID id, Info_Equip &DropEquip)
{
	//װ��Ʒ�ʣ���ͨ��������ϡ�С�׿Խ����������ͨ�ֵ�����ͨ��������ϡ�С�BOSS�ֵ���ϡ�С�������
	int32_t nTmp = 0;
	int32_t level = g_EquipList.value(id).lv;
	
	double dTmp = 1.0 * qrand() / RAND_MAX;
	if (dTmp > 0.95) {
		nTmp = 3;
	} else if (dTmp > 0.85) {
		nTmp = 2;
	} else if (dTmp > 0.65) {
		nTmp = 1;
	} else {
		nTmp = 0;
	}
	if (monster.isBoss())
	{
		nTmp += 1;
	}
	
	//��ʼ��
	DropEquip = { 0 };
	DropEquip.ID = id;
	DropEquip.lvUp = 0;
	DropEquip.extraAmount = nTmp;
	
	QVector<EquipExtraType> eet_attack = { eet_fixed_dc, eet_fixed_mc, eet_fixed_sc, eet_percent_dc, eet_percent_mc, eet_percent_sc };
	QVector<EquipExtraType> eet_defense = { eet_fixed_ac, eet_fixed_mac, eet_percent_ac, eet_percent_mac };
	int nArr[20];

	//��ȡװ�������
	int Type = (id - g_itemID_start_equip) / 1000;
	switch (Type)
	{
	case g_equipType_weapon: 
		eet_attack.append(eet_fixed_spd);
		eet_attack.append(eet_fixed_luck);
		break;

	case g_equipType_necklace:
	case  g_equipType_ring:
		eet_attack.append(eet_fixed_luck);
		break;

	case  g_equipType_bracelet:
		eet_attack.append(eet_fixed_spd);
		break;

	case g_equipType_clothes_m:
	case g_equipType_clothes_f:
	case g_equipType_belt:
		eet_defense.append(eet_fixed_hp);
		eet_defense.append(eet_fixed_hpr);
		eet_defense.append(eet_percent_hp);
		eet_defense.append(eet_percent_hpr);
		break;

	case g_equipType_helmet:
		eet_defense.append(eet_fixed_mp);
		eet_defense.append(eet_fixed_mpr);
		eet_defense.append(eet_percent_mp);
		eet_defense.append(eet_percent_mpr);
		break;
	default:
		//nothing
		break;
	}
	
	if (Type == g_equipType_weapon || Type == g_equipType_necklace || Type == g_equipType_bracelet ||
		Type == g_equipType_ring || Type == g_equipType_medal || Type == g_equipType_gem) {
		GetRandomSequence(nArr, eet_attack.size());
	} else {
		GetRandomSequence(nArr, eet_defense.size());
	}

	for (int i = 0; i < DropEquip.extraAmount; i++)
	{
		EquipExtra2 *extra = &DropEquip.extra[i];
		if (Type == g_equipType_weapon || Type == g_equipType_necklace || Type == g_equipType_bracelet ||
			Type == g_equipType_ring || Type == g_equipType_medal || Type == g_equipType_gem) {
			extra->eet = eet_attack[nArr[i]];
		} else {
			extra->eet = eet_defense[nArr[i]];
		}
		
		switch (extra->eet)
		{
		case eet_fixed_hp:
		case eet_fixed_mp:
			dTmp = 4 * level * level * qrand() / RAND_MAX;
			break;

		case eet_fixed_hpr:
		case eet_fixed_mpr:
			dTmp = 2 * level * qrand() / RAND_MAX;
			break;

		case eet_fixed_dc:
		case eet_fixed_mc:
		case eet_fixed_sc:
			dTmp = 3.5 * level * qrand() / RAND_MAX;
			break;

		case eet_fixed_ac:
		case eet_fixed_mac:
			dTmp = 1.8 * level * qrand() / RAND_MAX;
			break;

		case eet_fixed_spd:
			dTmp = 10 * level * qrand() / RAND_MAX;
			break;
		case eet_fixed_luck:
			dTmp = 0.5 * level * qrand() / RAND_MAX;
			break;

		case eet_percent_hpr:
		case eet_percent_mpr:
			dTmp = 3 * qrand() / RAND_MAX;
			break;

		default:
			dTmp = level * qrand() / RAND_MAX;
			break;
		}

		if (dTmp <= 0.5)
		{
			extra->value = 1;
		}
		else
		{
			extra->value = dTmp + 0.55;
		}
	}
}

void fight_fight::Action_role(void)
{
	quint32 nTmp1, nTmp_rhp, nTmp_rmp;

	player->update_beforeAction();

	//ʹ�õ��ߵ�����
	nTmp_rhp = player->get_hp_max() * ui.edit_hp->text().toInt() / 100;
	nTmp_rmp = player->get_mp_max() * ui.edit_mp->text().toInt() / 100;

	//�����ѡ���Զ�ʹ�õ���
	if (bCheckHp && player->get_hp_c() < nTmp_rhp)
	{
		Step_role_UsingItem_hp();
	}
	if (bCheckMp && player->get_mp_c() < nTmp_rmp)
	{
		Step_role_UsingItem_mp();
	}

	Step_role_Skill();
	if (monster.wasDead())
	{
		MonsterDead();
	}
}
void fight_fight::Action_monster(void)
{	
	monster.update_beforeAction();

	bool bLuck, bAttackPlayer;
	QList<int32_t> ListDamage;
	QString strTmp;

	if (pet.wasDead()) 	{
		bAttackPlayer = true;
	} 	else {
		bAttackPlayer = 0.65 < (1.0 * qrand() / RAND_MAX);
	}
	
	if (bAttackPlayer)
	{
		monster.attack(player, bLuck, &ListDamage);
		ui.display_Fighting->append(Generate_Display_LineText(
			QStringLiteral("<font color = darkRed>%1</font>").arg(monster.get_name()),
			monster.getVirtualSkillName(),
			QStringLiteral("<font color=DarkCyan>%1</font>").arg(player->get_name()),
			bLuck, false, ListDamage));


		if (player->wasDead())
		{
			bFighting = false;
			killTimer(nFightTimer);
			++nCount_fail;

			//��ɫ��������ʧ����30%�����20%
			int32_t nDropExp = player->get_exp() * 0.3;
			int32_t nDropCoin = player->get_coin() * 0.2;
			player->sub_exp(nDropExp);
			player->sub_coin(nDropCoin);

			ui.progressBar_role_exp->setValue(player->get_exp());
			ui.display_Fighting->append(QStringLiteral("<font color=white>ս��ʧ��!</font>"));
			ui.display_Fighting->append(QStringLiteral("��ʧ���飺") + QString::number(nDropExp));
			ui.display_Fighting->append(QStringLiteral("��ʧ��ң�") + QString::number(nDropCoin));
		}
	}
	else
	{
		monster.attack(&pet, bLuck, &ListDamage);
		ui.display_Fighting->append(Generate_Display_LineText(
			QStringLiteral("<font color = darkRed>%1</font>").arg(monster.get_name()),
			monster.getVirtualSkillName(),
			QStringLiteral("<font color=DarkCyan>%1</font>").arg(pet.get_name()),
			bLuck, false, ListDamage));

		if (pet.wasDead())
		{
			PetDead();
		}
	}
}

void fight_fight::Action_pet(void)
{
	pet.update_beforeAction();

	bool bLuck, bAttackPlayer;
	QList<int32_t> ListDamage;
	QString strTmp;

	pet.M_attack(&monster, bLuck, &ListDamage);
	ui.display_Fighting->append(Generate_Display_LineText(
		QStringLiteral("<font color=DarkCyan>%1</font>").arg(pet.get_name()),
		pet.get_skill().name,
		QStringLiteral("<font color=DarkRed>%1</font>").arg(monster.get_name()),
		bLuck, false, ListDamage));


	if (monster.wasDead())
	{
		MonsterDead();

		pet.add_exp(1);
		if (pet.get_exp() > pet.get_LvExp())
		{
			pet.LevelUp();
			UpdatePetParameter();
		}
	}
}

void fight_fight::GenerateMonster()
{
	bool bBoss = false;
	QString strTmp = "";
	if (bEnableBoss && monster_boss_count > 0)
	{
		++nElapse_pre_boss;
		if (nElapse_pre_boss > 100) {
			bBoss = true;
		} else 	{
			bBoss = (1.0 * qrand() / RAND_MAX) > g_fight_boss_probability;
		}
	}
	if (bBoss)
	{
		nElapse_pre_boss = 0;
		qint32 n = qrand() % monster_boss_count;
		monster.ReplaceSoul(g_MonsterInfo.value(monster_boss_assign[n]), bBoss);

		strTmp = QStringLiteral("ǿ���<font color=darkRed>") + monster.get_name()
			+ QStringLiteral("</font>��Ϯ,�¸ҵ�<font color = red>ս</font>�ɣ�");
	}
	else
	{
		qint32 n = qrand() % monster_normal_count;
		monster.ReplaceSoul(g_MonsterInfo.value(monster_normal_assign[n]), bBoss);

		strTmp = QStringLiteral("<font color= white>���� %1</font>").arg(monster.get_name());
	}

	monster.reset_live(time_remain);
	monster.ClearBuff();
	Display_CurrentMonsterInfo();
	ui.display_Fighting->setText(strTmp);
}

void fight_fight::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == nXSpeedTimer)
	{
		//����Ƿ����
		if (xSpeedTime.elapsed() - nXSpeedInvterval < 0)
		{
			++nXSpeedCount;
			if (nXSpeedCount > 20)	{
				LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_XSpeed);
				exit(0);
			}
		} else	{
			--nXSpeedCount;
			if (nXSpeedCount < 0)	{
				nXSpeedCount = 0;
			}
		}
		xSpeedTime.restart();
	}
	else if (event->timerId() == nFightTimer)
	{
		//ÿһ��timerEventΪһ���غϡ� 
		//��ǰδ����ս��״̬������ʱ��ʾ��һ�ε�ս����Ϣ����ʱ���������һ�����
		if (!bFighting)
		{
			--nShowStatusRound;
			if (nShowStatusRound >= 0) 	{
				return;
			}
		
			nShowStatusRound = 10;

			time_remain = 0;
			//����һ���������ʾ������Ϣ��
			GenerateMonster();			
			player->reset_live(time_remain);
			pet.reset_live(time_remain);
			bFighting = true;
		}

		//�غ�ʱ�������꣬�ж�ս����ʱ��
		if (time_remain > nTimeOutTime)
		{
			++nCount_timeout;
			ui.display_Fighting->append(QStringLiteral("<font color=white>ս����ʱ������Ѱ�ҹ��</font>"));
			bFighting = false;
			return;
		}
	
		//���غ�ʱ����ڽ�ɫʱ�䣬���ɫ�һ�غϡ����жϣ����غ�ʱ��С�ڹ���ʱ�䣬�����һ�غϡ�
		if (!pet.wasDead() && time_remain > pet.get_live())
		{
			Action_pet();
		}
		else if (time_remain > player->get_live())
		{
			Action_role();
			updateSkillCD();
		}
		else if (time_remain > monster.get_live())
		{
			Action_monster();
		}
	
		//ս����ʱ
		time_remain += nFightInterval;
	}
}

void fight_fight::updateSkillCD()
{
	for (int i = 0; i < fightingSkill.size(); i++)
	{
		--fightingSkill[i].cd_c;
	}
}

void fight_fight::DisplayRoleParameter(void)
{
	ui.edit_role_level->setText(QStringLiteral("Lv:") + QString::number(player->get_lv()));

	ui.progressBar_role_exp->setMaximum(g_JobAddSet[player->get_lv()].exp);
	ui.progressBar_role_exp->setValue(player->get_exp());	
	ui.progressBar_role_hp->setMaximum(player->get_hp_max());
	ui.progressBar_role_hp->setValue(player->get_hp_c());
	ui.progressBar_role_mp->setMaximum(player->get_mp_max());
	ui.progressBar_role_mp->setValue(player->get_mp_c());
}

void fight_fight::setPetVisible(bool Visible)
{
	ui.lbl_pet_info->setVisible(Visible);
	ui.edit_pet_name->setVisible(Visible);
	ui.label_pet_head->setVisible(Visible);
	ui.progressBar_pet_hp->setVisible(Visible);
	ui.progressBar_pet_mp->setVisible(Visible);
	ui.edit_pet_level->setVisible(Visible);
	ui.lbl_pet_vocation->setVisible(Visible);

	ui.lbl_pet_buff_0->setVisible(Visible);
	ui.lbl_pet_buff_1->setVisible(Visible);
	ui.lbl_pet_buff_2->setVisible(Visible);
	ui.lbl_pet_buff_3->setVisible(Visible);
}

void fight_fight::UpdatePetParameter()
{
	ui.edit_pet_level->setText(QStringLiteral("Lv:") + QString::number(pet.get_lv()));

	ui.progressBar_pet_hp->setMaximum(pet.get_hp_max());
	ui.progressBar_pet_mp->setMaximum(pet.get_mp_max());
}

bool fight_fight::SummonPet(const skill_fight &skill)
{
	if (!pet.wasDead())
	{
		return false;
	}

	bool bLuck;
	qint32 nDamage = (player->get_sc1() + player->get_sc2()) / 2;
	pet.ReplaceSoul(skill.no, skill.level, player->get_lv(), nDamage);
	pet.reset_live(time_remain);

	ui.edit_pet_name->setText(pet.get_name());
	ui.label_pet_head->setPixmap(QPixmap::fromImage(pet.get_head()));	

	UpdatePetParameter();

	setPetVisible(true);
	return true;
}

void fight_fight::PetDead()
{
	setPetVisible(false);	
}

void fight_fight::MonsterDead()
{
	double dTmp;
	quint32 nTmp, nDropExp, nDropCoin, nRoleLevel, nDropRep = 0;
	QString strTmp;

	bFighting = false;

	//������������ɫ���Ӿ��鼰��ҡ�����BOSS��������������
	//�����ȳ�1.0ת��Ϊdouble������ȼ�������㽫������uint��δӶ��õ�һ�������
	dTmp = atan(0.3 * (1.0 * monster.get_lv() - player->get_lv()));
	nTmp = monster.get_exp() * ((dTmp + 1.58) / 2);

	nDropExp = nTmp;
	if (player->get_lv() < MaxLevel)	{
		player->add_exp(nDropExp);
	}

	nDropCoin = monster.get_exp() * 0.11;
	player->add_coin(nDropCoin);

	if (monster.isBoss())
	{
		nDropRep = nTmp * 0.01;
		player->add_rep(nDropRep);

		player->add_soul(monster.get_lv());
	}

	if (monster.isBoss())	{
		++nCount_boss;
	} else	{
		++nCount_normalMonster;

		if ((nCount_normalMonster > g_boss_after_monster) && bCheckFindBoss) {
			bEnableBoss = true;
		} else {
			bEnableBoss = false;
		}
	}

	nCount_exp += nDropExp;
	nCount_coin += nDropCoin;
	nCount_rep += nDropRep;
	DisplayDropBasic(monster.get_name(), nDropExp, nDropCoin, nDropRep);
	CalcDropItemsAndDisplay(monster.get_id());

	quint64 lvExp = g_JobAddSet[player->get_lv()].exp;
	if (player->get_exp() > lvExp)
	{
		player->levelUp();
		DisplayRoleParameter();
		ui.display_Fighting->append(QStringLiteral("<font color=white>������. </font>"));
	}
	ui.progressBar_role_exp->setValue(player->get_exp());
}

inline void fight_fight::DisplayDropBasic(const QString &MonsterName, quint32 nDropExp, quint32 nDropCoin, quint32 nDropRep)
{
	ui.display_Spoils->append("");

	QString strTmp;
	strTmp = QStringLiteral("<font color=DarkCyan>��</font>������<font color=DarkRed>%1</font>").arg(MonsterName);
	strTmp += QStringLiteral("�����\t����: <font color=green>%1</font>, ���: <font color=#808000>%2</font>").arg(nDropExp).arg(nDropCoin);
	if (monster.isBoss())
	{
		strTmp += QStringLiteral("������: <font color=DarkMagenta>%1</font>").arg(nDropRep);
	}
	ui.display_Spoils->append(strTmp);
}