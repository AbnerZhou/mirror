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

//���岢��ʼ����̬���ݳ�Ա��
bool fight_fight::bCheckHp = false;
bool fight_fight::bCheckMp = false;
bool fight_fight::bCheckConcise = false;
bool fight_fight::bCheckFindBoss = false;
qint32 fight_fight::FilterAdd = 0;
qint32 fight_fight::FilterLvl = 0;
qint32 fight_fight::limit_rhp = 50;
qint32 fight_fight::limit_rmp = 50;

extern vecBuff g_buffList;
extern QVector<Info_jobAdd> g_JobAddSet;
extern QMap<skillID, Info_skill> g_skillList;
extern QMap<itemID, Info_Item> g_ItemList;
extern QMap<mapID, Info_Distribute> g_MonsterDistribute;
extern QVector<MonsterInfo> g_MonsterNormal_List;
extern QVector<MonsterInfo> g_MonsterBoss_list;
extern mapDrop	g_mapDropSet;

fight_fight::fight_fight(QWidget* parent, qint32 id, CPlayer *const w_player)
	: QDialog(parent), m_MainFrame(parent), m_mapID(id), player(w_player)
{
	ui.setupUi(this);
	InitUI();

	m_bag_item = player->get_bag_item();
	m_bag_equip = player->get_bag_equip();

	if (player->get_hp_c() < 1)
	{
		player->set_hp_c(1);
	}
	DisplayRoleinfo();
	DisplayRoleParameter();
	LoadItem();

	AssignMonster(g_MonsterNormal_List, g_MonsterBoss_list, g_MonsterDistribute);

	bFighting = false;
	bCheckHp = bCheckMp = true;
	nShowStatusRound = 0;
	nSkillIndex = 0;
	m_dlg_fightInfo = nullptr;

	if (m_mapID > 2000) {
		nTimeOutTime = 999 * 60 * 1000;
	} else {
		nTimeOutTime = 5 * 60 * 1000;
	}
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
	player->set_buff_rhp(0);
	player->set_buff_ac(0, 0);
	player->set_buff_mac(0, 0);
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

void fight_fight::on_btn_statistics_clicked(void)
{
	if (m_dlg_fightInfo == nullptr)
	{
		m_dlg_fightInfo = new fight_info(this);
	}
	QPoint pos = QPoint(730, 370);// mapFromGlobal(cursor().pos()) + QPoint(20, 0);
	m_dlg_fightInfo->move(pos);

	qint32 time = ct_start.secsTo(QDateTime::currentDateTime()) / 60;
	m_dlg_fightInfo->updateInfo(time, nCount_fail, nCount_timeout, nCount_normalMonster, nCount_boss, nCount_exp, nCount_coin, nCount_rep);
	m_dlg_fightInfo->show();
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
	FilterAdd = index * 2 - 1;
	if (FilterAdd < 0)
	{
		FilterAdd = 0;
	}
}

void fight_fight::InitUI()
{
	bCheckAuto = false;

	ui.progressBar_monster_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_monster_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");

	ui.edit_hp->setText(QString::number(limit_rhp));
	ui.edit_mp->setText(QString::number(limit_rmp));
 	ui.checkBox_concise->setChecked(bCheckConcise);
	ui.checkBox_boss->setChecked(bCheckFindBoss);
	ui.filter_add->setCurrentIndex((FilterAdd + 1) / 2);
	ui.filter_level->setText(QString::number(FilterLvl));

	buffDisp_Role[0] = ui.lbl_role_buff_0;
	buffDisp_Role[1] = ui.lbl_role_buff_1;
	buffDisp_Role[2] = ui.lbl_role_buff_2;
	buffDisp_Role[3] = ui.lbl_role_buff_3;
	buffDisp_Mon[0] = ui.lbl_monster_buff_0;
	buffDisp_Mon[1] = ui.lbl_monster_buff_1;
	buffDisp_Mon[2] = ui.lbl_monster_buff_2;
	buffDisp_Mon[3] = ui.lbl_monster_buff_3;
	buffDisp_pet[0] = ui.lbl_pet_buff_0;
	buffDisp_pet[1] = ui.lbl_pet_buff_1;
	buffDisp_pet[2] = ui.lbl_pet_buff_2;
	buffDisp_pet[3] = ui.lbl_pet_buff_3;

	for (qint32 i = 0; i < MaxBuffCount; i++)
	{
		buffDisp_Role[i]->setText("");
		buffDisp_Mon[i]->setText("");
	}

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
				fightingSkill.append(skill_fight(g_skillList.value(iterRole->id), iterRole->level));
				nTmp++;
			}
		}
	}
	if (fightingSkill.size() == 0)
	{
		fightingSkill.append(skill_fight(g_skillList.first(),1));
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

void fight_fight::LoadItem()
{
	QString strTmp;
	for (MapItem::iterator iter = m_bag_item->begin(); iter != m_bag_item->end(); iter++)
	{
		const Info_Item *itemItem = FindItem(iter.key());
		if (itemItem != nullptr && itemItem->level <= player->get_lv())
		{
			if (itemItem->type == et_immediate_hp)
			{
				strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("Ѫ"), itemItem->value, iter.value());
				ui.comboBox_hp->addItem(strTmp);
			}
			else if (itemItem->type == et_immediate_mp)
			{
				strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("ħ"), itemItem->value, iter.value());;
				ui.comboBox_mp->addItem(strTmp);
			}
		}
	}
	ui.comboBox_hp->setCurrentIndex(ui.comboBox_hp->count() - 1);
	ui.comboBox_mp->setCurrentIndex(ui.comboBox_mp->count() - 1);
}

bool fight_fight::AssignMonster(QVector<MonsterInfo> normalList, QVector<MonsterInfo> bossList, QMap<mapID, Info_Distribute> Distribute)
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

	//������IDת��Ϊ�����ܹ����б��е�������ţ��Է���������ء�
	c = 0;
	for (quint32 i = 0; i < normalList.size() && c < monster_normal_count; i++)
	{
		if (monster_normal_assign[c] == normalList[i].ID)
		{
			monster_normal_assign[c++] = i;
		}
	}
	c = 0;
	for (quint32 i = 0; i < bossList.size() && c < monster_boss_count; i++)
	{
		if (monster_boss_assign[c] == bossList[i].ID)
		{
			monster_boss_assign[c++] = i;
		}
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
	
	//��ͨ��ͼ�Ĺ��л�Ѫ���ܡ�	
	if (m_mapID < 2000) {	
		monster.set_rhp(monster.get_hp_max() >> 7);
	}

	//����ͷ��
	ui.label_monster_head->setPixmap(QPixmap::fromImage(monster.get_head()));

	//������������
	ui.edit_monster_name->setText(monster.get_name());
	ui.edit_monster_level->setText(QStringLiteral("Lv:") + QString::number(monster.get_lv()));
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

void fight_fight::Step_role_UsingItem_hp(void)
{
	quint32 ID, nTmp1;

	QString strTmp = ui.comboBox_hp->currentText();
	QStringList strList = strTmp.split(" ");

	const Info_Item *itemItem = FindItem(strList.at(0));
	if (itemItem != nullptr)
	{
		ID = itemItem->ID;
		//������Ӧ����������1
		m_bag_item->insert(ID, m_bag_item->value(ID) - 1); 
		strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("Ѫ"), itemItem->value, m_bag_item->value(ID));
		ui.comboBox_hp->setItemText(ui.comboBox_hp->currentIndex(), strTmp);

		//���Ľ�ɫ״̬
		player->set_hp_c(player->get_hp_c() + itemItem->value);
		ui.progressBar_role_hp->setValue(player->get_hp_c());
		if (!bCheckConcise)
		{
			strTmp = QStringLiteral("<font color=green>��ʹ���ˣ�") + itemItem->name + QStringLiteral("</font>");
			ui.edit_display->append(strTmp);
		}

		//��������Ѿ����꣬��ɾ����ǰ����.������е��ߣ����л���0�ŵ��ߣ���������Զ���Ѫ��ѡ��
		if (m_bag_item->value(ID) <= 0)
		{
			m_bag_item->remove(ID);
			ui.comboBox_hp->removeItem(ui.comboBox_hp->currentIndex());
			if (ui.comboBox_hp->count() > 0)
			{
				ui.comboBox_hp->setCurrentIndex(0);
			}
			else
			{
				ui.checkBox_hp->setChecked(false);
				bCheckHp = false;
			}
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
	quint32 ID, nTmp1;
	QString strTmp = ui.comboBox_mp->currentText();
	QStringList strList = strTmp.split(" ");

	const Info_Item *itemItem = FindItem(strList.at(0));
	if (itemItem != NULL)
	{
		ID = itemItem->ID;
		//������Ӧ����������1
		m_bag_item->insert(ID, m_bag_item->value(ID) - 1);
		strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("ħ"), itemItem->value, m_bag_item->value(ID));
		ui.comboBox_mp->setItemText(ui.comboBox_mp->currentIndex(), strTmp);

		//���Ľ�ɫ״̬
		player->set_mp_c(player->get_mp_c() + itemItem->value);
		ui.progressBar_role_mp->setValue(player->get_mp_c());
		if (!bCheckConcise)
		{
			strTmp = QStringLiteral("<font color=green>��ʹ���ˣ�") + itemItem->name + QStringLiteral("</font>");
			ui.edit_display->append(strTmp);
		}
		//��������Ѿ����꣬��ɾ����ǰ����.������е��ߣ����л���0�ŵ��ߣ���������Զ���Ѫ��ѡ��
		if (m_bag_item->value(ID) <= 0)
		{
			m_bag_item->remove(ID);
			ui.comboBox_mp->removeItem(ui.comboBox_mp->currentIndex());
			if (ui.comboBox_mp->count() > 0)
			{
				ui.comboBox_mp->setCurrentIndex(0);
			}
			else
			{
				ui.checkBox_mp->setChecked(false);
				bCheckMp = false;
			}
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
	qint32 spell, nTmp;

	for (qint32 i = 0; i < fightingSkill.size(); i++)
	{
		nTmp = nSkillIndex;
		const skill_fight &skill = fightingSkill.at(nSkillIndex++);
		if (nSkillIndex >= fightingSkill.size())
		{
			nSkillIndex = 0;
		}

		spell = skill.spell;
		if (player->get_mp_c() < spell)
		{
			QString strTmp = QStringLiteral("<font color=red>ħ�����㣬�޷�ʩ�ż���.</font>");
			ui.edit_display->append(strTmp);
			return;
		}	

		if (skill.cd_c <= 0)
		{
			if (skill.buff == 0 && skill.times == 0 && pet.wasDead())
			{
				SummonPet(skill);
				bUsedSkill = true;
			}
			if (skill.buff > 0)
			{
				bUsedSkill = MStep_role_Buff(skill);
			}
			if (skill.times > 0)
			{
				bUsedSkill = MStep_role_Attack(skill);
			}
		}
		if (bUsedSkill)
		{
			fightingSkill[nTmp].cd_c = fightingSkill[nTmp].cd;
			player->set_mp_c(player->get_mp_c() - spell);
			ui.progressBar_role_mp->setValue(player->get_mp_c());
			break;
		}
	}
	if (!bUsedSkill)
	{
		ui.edit_display->append(QStringLiteral("�޿��ü���"));
	}
}
bool fight_fight::MStep_role_Buff(const skill_fight &skill)
{
	if (m_mapID > 1000 && m_mapID < 2000 && skill.buff > 100)
	{
		QString strTmp = QStringLiteral("<font color=red>����ӵ��ħ�����.%1��Ч</font>").arg(skill.name);
		ui.edit_display->append(strTmp);
		return true;
	}

	quint32 nTmp, nTmp1;
	bool bLuck = false;
	if (skill.buff > 100)
	{
		foreach(const realBuff &real, buffInMonster)
		{
			if (real.id == skill.id)
			{//�����������д�buff�������ٴ�ʩ�ż��ܡ�
				return false;
			}
		}
	}
	else
	{ 
		foreach(const realBuff &real, buffInRole)
		{
			if (real.id == skill.id)
			{//��ɫ�������д�buff�������ٴ�ʩ�ż��ܡ�
				return false;
			}
		}
	}

	info_buff *buff = nullptr;
	for (quint32 i = 0; i < g_buffList.size(); i++)
	{
		if (g_buffList[i].ID == skill.buff)
		{
			buff = &g_buffList[i];
			break;
		}
	}

	if (buff == nullptr)
	{
		return false;
	}
	quint32 nA = player->GetAttack(skill.type, bLuck);
	realBuff real;
	real.id = skill.id;
	real.name = skill.name;
	real.icon = skill.icon;
	real.time = nA * buff->time / 100 + 2;
	real.rhp = nA * buff->rhp * skill.level / 100;
	real.ac = nA * buff->ac * skill.level / 100;
	real.mac = nA * buff->mac * skill.level / 100;

	if (skill.buff < 100)
	{//��������buff
		if (real.rhp > 0 && 
			0.8 < 1.0 * player->get_hp_c() / player->get_hp_max() &&
			0.8 < 1.0 * pet.get_hp_c() / pet.get_hp_max())
		{
			return false;				//�������Լ�����Ѫ������80%����ʹ�ûָ���buff��
		}
		buffInRole.append(real);
		buffDisp_Role[buffInRole.size() -1 ]->setPixmap(real.icon);
	}
	else
	{//�Է�����buff
		buffInMonster.append(real);
		buffDisp_Mon[buffInMonster.size() - 1]->setPixmap(real.icon);
	}

	if (!bCheckConcise)
	{
		QString strTmp = QStringLiteral("<font color=DarkCyan>��</font>ʹ��:<font color=gray>%1</font>").arg(skill.name);
		if (bLuck)
			strTmp += QStringLiteral("�������Ů��͸�,");

		strTmp += QStringLiteral("  Ч������<font color=magenta>") + QString::number(real.time) + QStringLiteral("</font>�غ� ");
#ifdef _DEBUG
		strTmp += QString::number(real.rhp) + " " + QString::number(real.ac) + " " + QString::number(real.mac);
#endif // _DEBUG
		
		ui.edit_display->append(strTmp);
	}
	return true;
}

bool fight_fight::MStep_role_Attack(const skill_fight &skill)
{
	qint32 nDamage, nTmp, nTmp1, nTmp2, m_ac,m_mac;
	bool bTmp, bep = false, bLuck = false;
	QList<qint32> ListDamage;

	player->attack(&monster, skill, bLuck, &ListDamage);
	ui.progressBar_monster_hp->setValue(monster.get_hp_c());
	time_remain_monster += skill.stiff;
	if (!bCheckConcise)
	{
		ui.edit_display->append(Generate_Display_LineText(
			QStringLiteral("<font color=DarkCyan>��</font>"), 
			skill.name, 
			QStringLiteral("<font color=DarkRed>%1</font>").arg(monster.get_name()), 
			bLuck, bep, ListDamage));
	}
	return true;
}
inline void fight_fight::DisplayDropBasic(quint32 nDropExp, quint32 nDropCoin, quint32 nDropRep)
{
	QString strTmp = QStringLiteral("<font color=white>���\t����: %1, ���: %2</font>").arg(nDropExp).arg(nDropCoin);	
	if (monster.isBoss())
	{
		strTmp += QStringLiteral("<font color=white>, ����: %1 </font>").arg(nDropRep);
	}
	ui.edit_display->append(strTmp);
}

void fight_fight::CreateEquip(itemID id, Info_Equip &DropEquip)
{
	//��Ʒ0--8����ֵĸ���
	double probability[9] = { 0, 0.4096, 0.2048, 0.0512, 0.0128, 0.0032, 0.0016, 0.004, 0.0004 };
	double dTmp = 1.0 * qrand() / RAND_MAX;
	qint32 extraAmount = 0;
	for (int i = 8; i > 0; i--)
	{
		if (dTmp > 1 - probability[i])
		{
			extraAmount = i;
			break;
		}
	}

	EquipExtra extra = { 0 };
	quint32 *p, extraPara = sizeof(EquipExtra) / sizeof(quint32);
	quint32 index, nCount, type;

	//�˴�ǿ��ת����Ϊ�������ʵ�ּ�Ʒװ�������Ե�λ�ü���С�����������ء�
	p = (quint32 *)&extra;

	//���������������������档
	while (extraAmount > 0)
	{
		index = qrand() % extraPara;
		nCount = qrand() % g_specialEquip_MaxExtra;

		p[index] = (extraAmount < nCount) ? extraAmount : nCount;
		extraAmount -= p[index];
	}
	//��ʼ��
	DropEquip = { 0 };
	DropEquip.ID = id;
	DropEquip.lvUp = 0;
	DropEquip.extra = extra;

	type = (DropEquip.ID - g_itemID_start_equip) / 1000;
	//������Ʒ�Բ�������׼ȷ�ӳ�
	DropEquip.extra.acc = 0;
	//ֻ�����������˼ӳ�,�������˷�Χֻ��0-3��
	if (DropEquip.extra.luck > 0)
	{	//fix ��ʱ��д�����Ժ���������ݿ������á�
		if (type == g_equipType_necklace && (DropEquip.ID == 305006 || DropEquip.ID == 305007 || DropEquip.ID == 305016))
			DropEquip.extra.luck = (DropEquip.extra.luck + 1) / 3;
		else
			DropEquip.extra.luck = 0;
	}
	if (type == g_equipType_weapon || type == g_equipType_necklace || type == g_equipType_ring)
	{
		//��������������ָ�������з�����ħ��
		DropEquip.extra.ac = 0;
		DropEquip.extra.mac = 0;
	}
	//ͳ�Ƽ�Ʒ������
	nCount = DropEquip.extra.luck + DropEquip.extra.ac + DropEquip.extra.mac + DropEquip.extra.dc + DropEquip.extra.mc + DropEquip.extra.sc;
	DropEquip.extraAmount = nCount;
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

	if (monster.isBoss())
	{
		//boss������������һЩ���ߣ�һƿ��죬һƿ������1����Ԫ��
		itemID nArr[5] = { 201003, 201013, 203007 };
		for (quint32 i = 0; i < 3; i++)
		{
			const Info_Item *item = Item_Base::FindItem_Item(nArr[i]);
			List_Drop.append(item->name);
			List_Pick.append(item->name);
			m_bag_item->insert(nArr[i], m_bag_item->value(nArr[i]) + 1);
		}
	}

	if (List_Drop.size() > 0)
	{
		strTmp.clear();
		foreach(const QString &s, List_Drop)
		{
			strTmp += s + ", ";
		}
		ui.edit_display->append(QStringLiteral("<font color=white>������������ɢ���ϵ��ϣ���ϸһ������%1 �ö�öడ��</font>").arg(strTmp));
	}

	if (List_Pick.size() > 0)
	{
		strTmp.clear();
		foreach(const QString &s, List_Pick)
		{
			strTmp += s + ", ";
		}
		ui.edit_display->append(QStringLiteral("<font color=white>ʰȡ %1 %2���������ޡ�</font>").arg(strTmp).arg(player->get_name()));
	}
}

void fight_fight::Action_role(void)
{
	quint32 nTmp1, nTmp_rhp, nTmp_rmp;

	time_remain_role += player->get_intervel();	//�ۼӽ�ɫ�ʱ�䡣
	//���Ľ�ɫ״̬
	player->set_hp_c(player->get_hp_c() + player->get_rhp());
	player->set_mp_c(player->get_mp_c() + player->get_rmp());

	ui.progressBar_role_hp->setValue(player->get_hp_c());
	ui.progressBar_role_mp->setValue(player->get_mp_c());

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
	time_remain_monster += monster.get_intervel();	//�ۼӹ���Ļʱ�䡣	
	//�����Ѫ
	monster.set_hp_c(monster.get_hp_c() + monster.get_rhp());
	ui.progressBar_monster_hp->setValue(monster.get_hp_c());

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
		monster.M_attack(player, bLuck, &ListDamage);
		ui.progressBar_role_hp->setValue(player->get_hp_c());
	}
	else
	{
		monster.M_attack(&pet, bLuck, &ListDamage);
		ui.progressBar_pet_hp->setValue(pet.get_hp_c());

		if (pet.wasDead())
		{
			PetDead();
		}
	}
	
	//�ǡ����ģʽ������ʾ�˺���Ϣ��
	if (!bCheckConcise)
	{
		if (bAttackPlayer) 	{
			strTmp = player->get_name();
		} else{
			strTmp = pet.get_name();
		}
		ui.edit_display->append(Generate_Display_LineText(
			QStringLiteral("<font color = darkRed>%1</font>").arg(monster.get_name()), 
			monster.get_skill()->name, 
			QStringLiteral("<font color=DarkCyan>%1</font>").arg(strTmp),
			bLuck, false, ListDamage));
	}

	if (player->wasDead())
	{
		//��ɫ����
		bFighting = false;
		killTimer(nFightTimer);
		++nCount_fail;

		//��ɫ��������ʧ����30%�����20%
		int32_t nDropExp = player->get_exp() * 0.3;
		int32_t nDropCoin = player->get_coin() * 0.2;
		player->sub_exp(nDropExp);
		player->sub_coin(nDropCoin);

		ui.progressBar_role_exp->setValue(player->get_exp());
		ui.edit_display->append(QStringLiteral("<font color=white>ս��ʧ��!</font>"));
		ui.edit_display->append(QStringLiteral("��ʧ���飺") + QString::number(nDropExp));
		ui.edit_display->append(QStringLiteral("��ʧ��ң�") + QString::number(nDropCoin));
	}
}

void fight_fight::Action_pet(void)
{
	time_remain_pet += pet.get_intervel();	//�ۼӳ���Ļʱ�䡣	
	pet.set_hp_c(pet.get_hp_c() + pet.get_rhp());
	ui.progressBar_pet_hp->setValue(pet.get_hp_c());


	bool bLuck, bAttackPlayer;
	QList<int32_t> ListDamage;
	QString strTmp;

	pet.M_attack(&monster, bLuck, &ListDamage);
	ui.progressBar_monster_hp->setValue(monster.get_hp_c());
	//�ǡ����ģʽ������ʾ�˺���Ϣ��
	if (!bCheckConcise)
	{
		ui.edit_display->append(Generate_Display_LineText(
			QStringLiteral("<font color=DarkCyan>%1</font>").arg(pet.get_name()),
			pet.get_skill()->name,
			QStringLiteral("<font color=DarkRed>%1</font>").arg(monster.get_name()),
			bLuck, false, ListDamage));
	}

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
	if (bCheckFindBoss && monster_boss_count > 0)
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
		monster.ReplaceSoul(g_MonsterBoss_list[monster_boss_assign[n]], bBoss);

		strTmp = QStringLiteral("ǿ���<font color=darkRed>") + monster.get_name()
			+ QStringLiteral("</font>��Ϯ,�¸ҵ�<font color = red>ս</font>�ɣ�");
	}
	else
	{
		qint32 n = qrand() % monster_normal_count;
		monster.ReplaceSoul(g_MonsterNormal_List[monster_normal_assign[n]], bBoss);

		strTmp = QStringLiteral("<font color= white>���� %1</font>").arg(monster.get_name());
	}
	if (!bCheckConcise)
	{
		ui.edit_display->setText(strTmp);
	}
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
			//����һ���������ʾ������Ϣ��
			GenerateMonster();
			Display_CurrentMonsterInfo();
			bFighting = true;
			time_remain = time_remain_role  = time_remain_pet = time_remain_monster = 0;
		}

		//�غ�ʱ�������꣬�ж�ս����ʱ��
		if (time_remain > nTimeOutTime)
		{
			++nCount_timeout;
			ui.edit_display->append(QStringLiteral("<font color=white>ս����ʱ������Ѱ�ҹ��</font>"));
			bFighting = false;
			return;
		}
	
		//���غ�ʱ����ڽ�ɫʱ�䣬���ɫ�һ�غϡ����жϣ����غ�ʱ��С�ڹ���ʱ�䣬�����һ�غϡ�
		if (time_remain > time_remain_pet && !pet.wasDead())
		{
			Action_pet();
		} else if (time_remain > time_remain_role)
		{
			Action_role();
			updateRoleBuffInfo();
			updateSkillCD();
		}
		else if (time_remain > time_remain_monster)
		{
			Action_monster();
			updateMonsterBuffInfo();
		}
	
		//ս����ʱ
		time_remain += nFightInterval;
	}
}

void fight_fight::updateRoleBuffInfo(void)
{
	qint32 i, b_rhp, b_ac, b_mac;

	b_rhp = b_ac = b_mac = 0;

	for (auto iter = buffInRole.begin(); iter != buffInRole.end(); )
	{
		--iter->time;
		if (iter->time <= 0)
		{
			iter = buffInRole.erase(iter);
		}
		else
		{
			b_rhp += iter->rhp;
			b_ac += iter->ac;
			b_mac += iter->mac;

			iter++;
		}
	}

	//��ʱ�ó��������˹���buff.
	i = 0;
	int32_t nTmp = (buffInRole.size() < MaxBuffCount) ? buffInRole.size() : MaxBuffCount;
	for (; i < nTmp; i++)
	{
		buffDisp_Role[i]->setPixmap(buffInRole[i].icon);
		buffDisp_pet[i]->setPixmap(buffInRole[i].icon);
	}
	for (; i < MaxBuffCount; i++)
	{
		buffDisp_Role[i]->setPixmap(QPixmap(""));
		buffDisp_pet[i]->setPixmap(QPixmap(""));
	}
	
	player->set_buff_rhp(b_rhp);
	player->set_buff_ac(b_ac, b_ac);
	player->set_buff_mac(b_mac, b_mac);

	pet.set_buff_rhp(b_rhp);
	pet.set_buff_ac(b_ac, b_ac);
	pet.set_buff_mac(b_mac, b_mac);
}

void fight_fight::updateMonsterBuffInfo(void)
{
	qint32 i,b_rhp, b_ac, b_mac;
	
	b_rhp = b_ac = b_mac = 0;

	for (auto iter = buffInMonster.begin(); iter != buffInMonster.end(); )
	{
		--iter->time;
		if (iter->time <= 0)
		{
			iter = buffInMonster.erase(iter);
		}
		else
		{
			b_rhp -= iter->rhp;
			b_ac -= iter->ac;
			b_mac -= iter->mac;

			iter++;
		}
	}

	i = 0;
	for (; i < buffInMonster.size(); i++)
	{
		buffDisp_Mon[i]->setPixmap(buffInMonster[i].icon);
	}
	for (; i < MaxBuffCount; i++)
	{
		buffDisp_Mon[i]->setPixmap(QPixmap(""));
	}
	
	monster.set_buff_rhp(b_rhp);
	monster.set_buff_ac(b_ac, b_ac);
	monster.set_buff_mac(b_mac, b_mac);
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
	QString strTmp;
	quint32 nTmp1, nTmp2;
	quint64 role_exp;

	ui.edit_role_level->setText(QStringLiteral("Lv:") + QString::number(player->get_lv()));

	ui.progressBar_role_exp->setMaximum(g_JobAddSet[player->get_lv()].exp);
	if (player->get_exp() >= ui.progressBar_role_exp->maximum())
		ui.progressBar_role_exp->setValue(ui.progressBar_role_exp->maximum());
	else
		ui.progressBar_role_exp->setValue(player->get_exp());

	ui.progressBar_role_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_role_hp->setMaximum(player->get_hp_max());
	ui.progressBar_role_hp->setValue(player->get_hp_c());

	ui.progressBar_role_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");
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

	for (int32_t i = 0; i < MaxBuffCount;i++)
	{
		buffDisp_pet[i]->setVisible(Visible);
	}
	
}

void fight_fight::UpdatePetParameter()
{
	ui.edit_pet_level->setText(QStringLiteral("Lv:") + QString::number(pet.get_lv()));

	ui.progressBar_pet_hp->setMaximum(pet.get_hp_max());
	ui.progressBar_pet_hp->setValue(pet.get_hp_c());

	ui.progressBar_pet_mp->setMaximum(pet.get_mp_max());
	ui.progressBar_pet_mp->setValue(pet.get_mp_c());
}

void fight_fight::SummonPet(const skill_fight &skill)
{
	//�ٻ�
	pet.ReplaceSoul(skill.damage, skill.level, skill.basic, player->get_lv());

	ui.edit_pet_name->setText(pet.get_name());
	ui.label_pet_head->setPixmap(QPixmap::fromImage(pet.get_head()));	

	UpdatePetParameter();

	time_remain_pet = time_remain;
	setPetVisible(true);
}

void fight_fight::PetDead()
{
	time_remain_pet = nTimeOutTime + 1;

	setPetVisible(false);	
}

void fight_fight::MonsterDead()
{
	double dTmp;
	quint32 nTmp, nDropExp, nDropCoin, nRoleLevel, nDropRep = 0;
	QString strTmp;

	bFighting = false;
	buffInMonster.clear();
	for (int i = 0; i < MaxBuffCount; i++)
	{
		buffDisp_Mon[i]->setPixmap(QPixmap(""));
	}

	//������������ɫ���Ӿ��鼰��ҡ�����BOSS��������������
	//�����ȳ�1.0ת��Ϊdouble������ȼ�������㽫������uint��δӶ��õ�һ�������
	dTmp = atan(0.3 * (1.0 * monster.get_lv() - player->get_lv()));
	nTmp = monster.get_exp() * ((dTmp + 1.58) / 2);

	//�ȼ�ÿ��99ʱ�������ȡֻ��1��
	if (99 == (player->get_lv() % 100))	{
		nDropExp = 1;
	}
	else {
		nDropExp = nTmp;
	}
	player->add_exp(nDropExp);

	nDropCoin = monster.get_exp() * 0.11;
	player->add_coin(nDropCoin);

	if (monster.isBoss())
	{
		nDropRep = nTmp * 0.01;
		player->add_rep(nDropRep);
	}

	if (bCheckConcise)
		ui.edit_display->setText(QStringLiteral("<font color=white>������� %1 </font>").arg(monster.get_name()));
	else
		ui.edit_display->append(QStringLiteral("<font color=white>������� %1 </font>").arg(monster.get_name()));

	if (monster.isBoss())	{
		++nCount_boss;
	}
	else	{
		++nCount_normalMonster;
	}

	nCount_exp += nDropExp;
	nCount_coin += nDropCoin;
	nCount_rep += nDropRep;

	ui.edit_display->append("");
	DisplayDropBasic(nDropExp, nDropCoin, nDropRep);
	CalcDropItemsAndDisplay(monster.get_id());

	quint64 lvExp = g_JobAddSet[player->get_lv()].exp;
	if (player->get_exp() > lvExp)
	{
		player->levelUp();
		DisplayRoleParameter();
		ui.edit_display->append(QStringLiteral("<font color=white>������. </font>"));
	}
	ui.progressBar_role_exp->setValue(player->get_exp());
}