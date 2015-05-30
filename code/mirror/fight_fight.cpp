#include "fight_fight.h"
#include <QFile>
#include <QMessageBox>
#include <QThread>
#include <time.h>
#include <QKeyEvent>

#include "Item_Base.h"
#include "def_System_para.h"

const int interval = 100;

//���岢��ʼ����̬���ݳ�Ա��
bool fight_fight::bCheckHp = false;
bool fight_fight::bCheckMp = false;
bool fight_fight::bCheckConcise = false;
bool fight_fight::bCheckFindBoss = false;
qint32 fight_fight::pickFilter = 0;

extern vecBuff g_buffList;
extern QVector<Info_skill> g_skillList;
extern QVector<Info_Item> g_ItemList;
extern QVector<Info_basic_equip> g_EquipList;
extern QVector<Info_Distribute> g_MonsterDistribute;
extern QVector<MonsterInfo> g_MonsterNormal_List;
extern QVector<MonsterInfo> g_MonsterBoss_list;
extern mapDrop	g_mapDropSet;

fight_fight::fight_fight(QWidget* parent, qint32 id, RoleInfo *info, MapItem *bag_item, ListEquip *bag_equip)
	: QDialog(parent), m_MainFrame(parent), m_mapID(id), myRole(info), m_bag_item(bag_item), m_bag_equip(bag_equip)
{
	ui.setupUi(this);
	InitUI();

	Cacl_Display_Role_Value();
	LoadItem();

	AssignMonster(g_MonsterNormal_List, g_MonsterBoss_list, g_MonsterDistribute);
	monster_cur = &g_MonsterNormal_List[monster_normal_assign[0]];
	Display_CurrentMonsterInfo();

	bKeepFight = bFighting = bTimeOut = false;	
	time_remain = time_remain_role = time_remain_monster = 0;
	nShowStatusRound = 0;
	nSkillIndex = 0;
	m_dlg_fightInfo = nullptr;

	connect(ui.comboBox_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(pickFilterChange(int)));
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
	if (!bFighting || bTimeOut)
	{
		this->close();
	}
	else
	{
		QString title = QStringLiteral("��ʾ");
		QString message = QStringLiteral("��ǰ����ս���У����ܽ���ʧ50%������30%��ҡ�");
		QMessageBox msgBox(QMessageBox::Question, title, message);
		QPushButton *YsBtn = msgBox.addButton(QStringLiteral(" �� "), QMessageBox::AcceptRole);
		QPushButton *NoBtn = msgBox.addButton(QStringLiteral(" �� "), QMessageBox::RejectRole);
		msgBox.exec();
		if (msgBox.clickedButton() == YsBtn)
		{
			myRole->coin -= myRole->coin * 0.3;
			myRole->reputation -= myRole->reputation * 0.5;
			this->close();
		}
	}
}

void fight_fight::on_btn_start_clicked(void)
{	
	nFightTimer = startTimer(interval);
	ui.btn_start->setEnabled(false);
}
void fight_fight::on_btn_statistics_clicked(void)
{
	time_t t_Cur, t_cost;
	time(&t_Cur);
	t_cost = (t_Cur - t_Count_start) / 60;
	if (m_dlg_fightInfo != nullptr)
	{
		delete m_dlg_fightInfo;
	}
	m_dlg_fightInfo = new fight_info(this);
	m_dlg_fightInfo->updateInfo(t_cost, nCount_normalMonster, nCount_boss, nCount_exp, nCount_coin, nCount_rep);
	m_dlg_fightInfo->show();
}
void fight_fight::on_checkBox_auto_clicked(void)
{
	if (ui.checkBox_auto->isChecked())
	{
		nCount_normalMonster = nCount_boss = nCount_exp = nCount_coin = nCount_rep = 0;
		time(&t_Count_start);
		bCheckAuto = true;
	}
	else
	{
		bCheckAuto = false;
	}
	
}
void fight_fight::pickFilterChange(int index)
{
	pickFilter = index;
}

void fight_fight::InitUI()
{
	bCheckAuto = false;

	ui.progressBar_monster_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_monster_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");
	ui.edit_monster_sc->setText("0 - 0");
	ui.edit_monster_rmp->setText("0");

 	ui.checkBox_hp->setChecked(bCheckHp);
 	ui.checkBox_mp->setChecked(bCheckMp);
 	ui.checkBox_concise->setChecked(bCheckConcise);
	ui.checkBox_boss->setChecked(bCheckFindBoss);
	ui.comboBox_filter->setCurrentIndex(pickFilter);

	ui.label_role_head->setAttribute(Qt::WA_TranslucentBackground, true);
	ui.edit_role_name->setAttribute(Qt::WA_TranslucentBackground, true);
	ui.edit_role_level->setAttribute(Qt::WA_TranslucentBackground, true);
	ui.edit_role_vocation->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_3->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_4->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_5->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_6->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_7->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_8->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_9->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_10->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_11->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_12->setAttribute(Qt::WA_TranslucentBackground, true);

	ui.label_monster_head->setAttribute(Qt::WA_TranslucentBackground, true);
	ui.edit_monster_name->setAttribute(Qt::WA_TranslucentBackground, true);
	ui.edit_monster_level->setAttribute(Qt::WA_TranslucentBackground, true);
	ui.edit_monster_vocation->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_53->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_54->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_55->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_56->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_57->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_58->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_59->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_60->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_61->setAttribute(Qt::WA_TranslucentBackground, true);
// 	ui.label_62->setAttribute(Qt::WA_TranslucentBackground, true);

// 	ui.checkBox_hp->setStyleSheet("QCheckBox{ background:transparent} ");
// 	ui.checkBox_mp->setStyleSheet("QCheckBox{ background:transparent} ");
// 	ui.checkBox_auto->setStyleSheet("QCheckBox{ background:transparent} ");
// 	ui.checkBox_concise->setStyleSheet("QCheckBox{ background:transparent} ");
// 	ui.checkBox_boss->setStyleSheet("QCheckBox{ background:transparent} ");

//	ui.groupBox->setStyleSheet("QGroupBox{ background:transparent} ");

//	ui.edit_hp->setStyleSheet("QLineEdit{ background:transparent} ");
//	ui.edit_mp->setStyleSheet("QLineEdit{ background:transparent} ");
//	ui.edit_display->setStyleSheet("QTextEdit{ background:transparent} ");

//	ui.btn_start->setStyleSheet("QPushButton{ background:rgb(170, 170, 170)} ");
//	ui.btn_quit->setStyleSheet("QPushButton{ background:rgb(170, 170, 170)} ");
//	ui.btn_statistics->setStyleSheet("QPushButton{ background:rgb(170, 170, 170)} ");

	buffDisp_Role[0] = ui.lbl_role_buff_0;
	buffDisp_Role[1] = ui.lbl_role_buff_1;
	buffDisp_Role[2] = ui.lbl_role_buff_2;
	buffDisp_Mon[0] = ui.lbl_monster_buff_0;
	buffDisp_Mon[1] = ui.lbl_monster_buff_1;
	buffDisp_Mon[2] = ui.lbl_monster_buff_2;

	for (qint32 i = 0; i < MaxBuffCount; i++)
	{
		buffDisp_Role[i]->setText("");
		buffDisp_Mon[i]->setText("");
	}
}

void fight_fight::Cacl_Display_Role_Value()
{
	ui.edit_role_name->setText(myRole->name);
	ui.edit_role_level->setText(QStringLiteral("Lv:") + QString::number(myRole->level));

	QString def_vocation[] = { QStringLiteral("��"), QStringLiteral("ս"), QStringLiteral("��"), QStringLiteral("��") };
	ui.edit_role_vocation->setText(def_vocation[myRole->vocation]);

	ui.progressBar_role_exp->setMaximum(myRole->lvExp);
	if (myRole->exp >= ui.progressBar_role_exp->maximum())
		ui.progressBar_role_exp->setValue(ui.progressBar_role_exp->maximum());
	else
		ui.progressBar_role_exp->setValue(myRole->exp);

	ui.edit_role_interval->setText(QString::number(myRole->intervel));
	ui.edit_role_dc->setText(QString::number(myRole->dc1) + "-" + QString::number(myRole->dc2));
	ui.edit_role_mc->setText(QString::number(myRole->mc1) + "-" + QString::number(myRole->mc2));
	ui.edit_role_sc->setText(QString::number(myRole->sc1) + "-" + QString::number(myRole->sc2));
	ui.edit_role_ac->setText(QString::number(myRole->ac1) + "-" + QString::number(myRole->ac2));
	ui.edit_role_mac->setText(QString::number(myRole->mac1) + "-" + QString::number(myRole->mac2));

	role_ac1 = myRole->ac1;
	role_ac2 = myRole->ac2;
	role_mac1 = myRole->mac1;
	role_mac2 = myRole->ac2;

	ui.progressBar_role_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_role_hp->setMaximum(myRole->hp);
	ui.progressBar_role_hp->setValue(myRole->hp);
	role_hp_c = myRole->hp;

	ui.progressBar_role_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");
	ui.progressBar_role_mp->setMaximum(myRole->mp);
	ui.progressBar_role_mp->setValue(myRole->mp);
	role_mp_c = myRole->mp;

	role_rhp = 0;
	ui.edit_role_rhp->setText(QString::number(role_rhp));

	role_rmp = 0;
	ui.edit_role_rmp->setText(QString::number(role_rmp));

	qint32 headNo = ((myRole->vocation - 1) * 2 + myRole->gender) * 10;
	QString headImg = (":/role/Resources/role/") + QString::number(headNo) + ".png";
	ui.label_role_head->setPixmap(QPixmap(headImg));

	//�����������б��е�����ȡ���һ����ܣ��Խ�Լ�������õ�Ч��
	skill_fight skill;
	for (VecRoleSkill::const_iterator iterRole = myRole->skill.begin(); iterRole != myRole->skill.end(); iterRole++)
	{
		for (QVector<Info_skill>::iterator iterSkill = g_skillList.begin(); iterSkill != g_skillList.end(); iterSkill++)
		{
			if (iterRole->id == iterSkill->ID)
			{
				skill.id = iterSkill->ID;
				skill.name = iterSkill->name;
				skill.icon = iterSkill->icon;
				skill.level = iterRole->level;
				skill.spell = iterSkill->spell[skill.level - 1];
				skill.cd = iterSkill->cd;
				skill.cd_c = 0;
				skill.times = iterSkill->times;
				skill.damage = iterSkill->damage[skill.level - 1];
				skill.buff = iterSkill->buff;
				fightingSkill.append(skill);
			}
		}
	}
}

const Info_Item* fight_fight::FindItem(quint32 ID)
{
	foreach(const Info_Item &item, g_ItemList)
	{
		if (item.ID == ID)
		{
			return &item;
		}
	}
	return NULL;
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
	return NULL;
}

void fight_fight::LoadItem()
{
	QString strTmp;
	for (MapItem::iterator iter = m_bag_item->begin(); iter != m_bag_item->end(); iter++)
	{
		const Info_Item *itemItem = FindItem(iter.key());
		if (itemItem != nullptr && itemItem->level <= myRole->level)
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
}

bool fight_fight::AssignMonster(QVector<MonsterInfo> normalList, QVector<MonsterInfo> bossList, QVector<Info_Distribute> Distribute)
{
	quint32 c;
	memset(monster_normal_assign, 0, Max_monster * sizeof(quint32));
	memset(monster_boss_assign, 0, Max_monster * sizeof(quint32));

	//���г�����ͼ��ˢ�¹����ID��
	for (quint32 i = 0; i < Distribute.size(); i++)
	{
		if (Distribute[i].ID == m_mapID)
		{
			c = 0;
			foreach(quint32 n, Distribute[i].normal)
			{
				monster_normal_assign[c++] = n;
			}
			monster_normal_count = c;

			c = 0;
			foreach(quint32 n, Distribute[i].boss)
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
			break;
		}
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
	ui.progressBar_monster_hp->setMaximum(monster_cur->hp);
	ui.progressBar_monster_mp->setMaximum(monster_cur->mp);
	//��ʾ��ǰ�塢ħ
	monster_cur_hp = monster_cur->hp;
//	monster_cur_mp = monster_cur->mp;
	ui.progressBar_monster_hp->setValue(monster_cur_hp);
	ui.progressBar_monster_mp->setValue(monster_cur->mp);
	
	//�ظ��塢ħΪ���ֵ	
	monster_cur_rhp = monster_cur_hp >> 7;	
	ui.edit_monster_rhp->setText(QString::number(monster_cur_rhp));

	//����ͷ��
	ui.label_monster_head->setPixmap(QPixmap::fromImage(monster_cur->Head));

	//������������
	monster_cur_ac = monster_cur->AC;
	monster_cur_mac = monster_cur->MAC;
	ui.edit_monster_name->setText(monster_cur->name);
	ui.edit_monster_level->setText(QStringLiteral("Lv:") + QString::number(monster_cur->level));
	ui.edit_monster_dc->setText(QString::number(monster_cur->DC1) + " - " + QString::number(monster_cur->DC2));
	ui.edit_monster_mc->setText(QString::number(monster_cur->MC1) + " - " + QString::number(monster_cur->MC2));
	ui.edit_monster_ac->setText(QString::number(monster_cur_ac));
	ui.edit_monster_mac->setText(QString::number(monster_cur_mac));
	ui.edit_monster_interval->setText(QString::number(monster_cur->interval));


}

inline QString fight_fight::Generate_ItemComboBox_Text(const QString &name, const QString &type, quint32 value, quint32 count)
{
	QString strSplit = QStringLiteral(" ");
	QString strTmp = name;
	strTmp += strSplit + type + QStringLiteral(":") + QString::number(value);
	strTmp += strSplit + QStringLiteral("ʣ:") + QString::number(count);
	return strTmp;
}
inline QString fight_fight::Generate_Display_LineText(const QString &str1, const QString &skill, const QString &str2, QList<qint32> listDamage)
{
	QString strTmp = QStringLiteral("<font color=blue>") + str1
		+ QStringLiteral("</font>ʹ��<font color=darkRed>") + skill
		+ QStringLiteral("</font>����<font color = blue>") + str2
		+ QStringLiteral("</font>����˺�:<font color = magenta>");

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
	quint32 ID;

	QString strTmp = ui.comboBox_hp->currentText();
	QStringList strList = strTmp.split(" ");

	const Info_Item *itemItem = FindItem(strList.at(0));
	if (itemItem != NULL)
	{
		ID = itemItem->ID;
		//������Ӧ����������1
		m_bag_item->insert(ID, m_bag_item->value(ID) - 1); 
		strTmp = Generate_ItemComboBox_Text(itemItem->name, QStringLiteral("Ѫ"), itemItem->value, m_bag_item->value(ID));
		ui.comboBox_hp->setItemText(ui.comboBox_hp->currentIndex(), strTmp);

		//���Ľ�ɫ״̬
		role_hp_c += itemItem->value;
		if (role_hp_c >= myRole->hp)
		{
			role_hp_c = myRole->hp;
		}
		ui.progressBar_role_hp->setValue(role_hp_c);
		if (!bCheckConcise)
		{
			strTmp = QStringLiteral("<font color=black>��ʹ���ˣ�") + itemItem->name + QStringLiteral("</font>");
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
	quint32 ID;
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
		role_mp_c += itemItem->value;
		if (role_mp_c >= myRole->mp)
		{
			role_mp_c = myRole->mp;
		}
		ui.progressBar_role_mp->setValue(role_mp_c);
		if (!bCheckConcise)
		{
			strTmp = QStringLiteral("<font color=black>��ʹ���ˣ�") + itemItem->name + QStringLiteral("</font>");
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

inline quint32 fight_fight::GetRoleATK()
{
	quint32 nA;
	if (myRole->vocation == 1)
	{
		nA = myRole->dc1 + qrand() % (myRole->dc2 - myRole->dc1 + 1);
	}
	else if (myRole->vocation == 2)
	{
		nA = myRole->mc1 + qrand() % (myRole->mc2 - myRole->mc1 + 1);
	}
	else
	{
		nA = myRole->sc1 + qrand() % (myRole->sc2 - myRole->sc1 + 1);
	}
	return nA;
}

void fight_fight::Step_role_Skill(void)
{
	++nCount_attack;
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

		if (role_mp_c < skill.spell)
		{
			QString strTmp = QStringLiteral("<font color=red>ħ�����㣬�޷�ʩ�ż���.</font>");
			ui.edit_display->append(strTmp);
			return;
		}
		spell = skill.spell;

		if (skill.cd_c <= 0)
		{
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
			role_mp_c -= spell;
			ui.progressBar_role_mp->setValue(role_mp_c);
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

	if (buff != nullptr)
	{
		quint32 nA = GetRoleATK();
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
			if (real.rhp > 0 && 0.8 < 1.0 * role_hp_c / myRole->hp)
			{
				return false;				//������Ѫ������80%����ʹ�ûָ���buff��
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
			QString strTmp = QStringLiteral("<font color=blue>��</font>ʹ��:<font color=darkRed>") + skill.name
				+ QStringLiteral("</font>  Ч������<font color=cyan>") + QString::number(real.time) + QStringLiteral("</font>�غ� ");
#ifdef _DEBUG
			strTmp += QString::number(real.rhp) + " " + QString::number(real.ac) + " " + QString::number(real.mac);
#endif // _DEBUG
		
			ui.edit_display->append(strTmp);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool fight_fight::MStep_role_Attack(const skill_fight &skill)
{
	qint32 nDamage, nTmp;
	QList<qint32> ListDamage;
	for (qint32 i = 0; i < skill.times; i++)
	{
		quint32 nA = GetRoleATK();
		if (skill.id == g_skillList.at(0).ID)
		{
			nTmp = myRole->dc1 + qrand() % (myRole->dc2 - myRole->dc1 + 1);
			nDamage = (nTmp - monster_cur_ac);
		}
		else if (myRole->vocation == 1)
		{
			nTmp = nA * skill.damage / 100;
			nDamage = (nTmp - monster_cur_ac);
		}
		else
		{
			nTmp = nA * skill.damage / 100;
			nDamage = (nTmp - monster_cur_mac);
		}
		nDamage = (nDamage < 1 ? 1 : nDamage);
		ListDamage.append(nDamage);

		nTmp = monster_cur_hp - nDamage;
		monster_cur_hp = nTmp < 0 ? 0 : nTmp;
		ui.progressBar_monster_hp->setValue(monster_cur_hp);	
	}
	if (!bCheckConcise && skill.times != 0)
	{
		ui.edit_display->append(Generate_Display_LineText(QStringLiteral("��"), skill.name, monster_cur->name, ListDamage));
	}
	//���Ľ�ɫ״̬
	nTmp = role_hp_c + role_rhp;
	role_hp_c = nTmp > myRole->hp ? myRole->hp : nTmp;
	ui.progressBar_role_hp->setValue(role_hp_c);
	return true;
}
inline void fight_fight::DisplayDropBasic(quint32 nDropExp, quint32 nDropCoin, quint32 nDropRep)
{
	ui.edit_display->append(QStringLiteral("<font color=black>��þ���:") + QString::number(nDropExp) + QStringLiteral("</font>"));
	ui.edit_display->append(QStringLiteral("<font color=black>��ý��:") + QString::number(nDropCoin) + QStringLiteral("</font>"));
	if (bBoss)
	{
		ui.edit_display->append(QStringLiteral("<font color=black>�������:") + QString::number(nDropRep) + QStringLiteral("</font>"));
	}
}

void fight_fight::CreateEquip(itemID id, Info_Equip &DropEquip)
{
	//��Ʒ0--8����ֵĸ���
	double probability[9] = { 0, 0.4096, 0.2048, 0.0512, 0.0128, 0.0064, 0.0032, 0.0016, 0.0008 };
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

	const Info_basic_equip *EquipBasicInfo = Item_Base::GetEquipBasicInfo(id);
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
	//����װ���Բ����������˼ӳ�,�������˷�Χֻ��0-3��
	if (DropEquip.extra.luck > 0)
	{
		if (type == g_equipType_necklace)
			DropEquip.extra.luck = 0;
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
	ui.edit_display->append(" ");
	Info_Equip DropEquip;
	double dTmp1, dTmp2;
	const ListDrop &LD = g_mapDropSet[id];
	foreach(const Rational &rRat, LD)
	{
		dTmp1 = 1.0 * qrand() / RAND_MAX;
		dTmp2 = 1.0 * (rRat.den - 1) / rRat.den;
		if (dTmp1 > dTmp2)
		{
			if (rRat.ID > g_itemID_start_equip && rRat.ID <= g_itemID_stop_equip)
			{
				//����װ��,����ʰȡ���˻�Ʒ��ʰȡ����ȡ���ۡ�
				CreateEquip(rRat.ID, DropEquip);
				const Info_basic_equip *equip = Item_Base::GetEquipBasicInfo(DropEquip.ID);
				ui.edit_display->append(QStringLiteral("<font color=black>���:") + equip->name + QStringLiteral("</font>"));
				if (m_bag_equip->size() >= g_bag_maxSize)
				{
					myRole->coin += equip->price >> 1;
					ui.edit_display->append(QStringLiteral("<font color=black>��������������:") + equip->name
						+ QStringLiteral(" ��ý��:") + QString::number(equip->price >> 2) + equip->name + QStringLiteral("</font>"));
				}
				else if (equip->lv > pickFilter || DropEquip.extraAmount > 0)
				{
					m_bag_equip->append(DropEquip);
				}
				else
				{
					myRole->coin += equip->price >> 1;
					ui.edit_display->append(QStringLiteral("<font color=black>����:") + equip->name 
						+ QStringLiteral(" ��ý��:") + QString::number(equip->price >> 2) + equip->name + QStringLiteral("</font>"));
				}
			}
			else
			{
				//��������
				const Info_Item *item = Item_Base::FindItem_Item(rRat.ID);
				ui.edit_display->append(QStringLiteral("<font color=black>���:") + item->name + QStringLiteral("</font>"));
				m_bag_item->insert(rRat.ID, m_bag_item->value(rRat.ID) + 1);
			}
		}
	}

	if (bBoss)
	{
		//boss������������һЩ���ߣ���ƿ��죬��ƿ������1����Ԫ��
		itemID nArr[5] = { 201003, 201003, 201013, 201013, 203007 };
		for (quint32 i = 0; i < 5; i++)
		{
			const Info_Item *item = Item_Base::FindItem_Item(nArr[i]);
			ui.edit_display->append(QStringLiteral("<font color=black>���:") + item->name + QStringLiteral("</font>"));
			m_bag_item->insert(nArr[i], m_bag_item->value(nArr[i]) + 1);
		}
	}
}

void fight_fight::Action_role(void)
{
	time_remain_role += myRole->intervel;	//�ۼӽ�ɫ�ʱ�䡣

	//ʹ�õ��ߵ�����
	qint32 limit_rhp = myRole->hp * ui.edit_hp->text().toInt() / 100;
	qint32 limit_rmp = myRole->mp * ui.edit_mp->text().toInt() / 100;

	//�����ѡ���Զ�ʹ�õ���
	if (bCheckHp && role_hp_c < limit_rhp)
	{
		Step_role_UsingItem_hp();
	}
	else if (bCheckMp && role_mp_c < limit_rmp)
	{
		Step_role_UsingItem_mp();
	}
	else
	{
		Step_role_Skill();
	}
	double dTmp;
	quint32 nTmp, nDropExp, nDropCoin, nDropRep = 0;
	QString strTmp;

	if (monster_cur_hp <= 0)
	{
		bFighting = false;
		buffInMonster.clear();
		for (int  i = 0; i < MaxBuffCount; i++)
		{
			buffDisp_Mon[i]->setPixmap(QPixmap(""));
		}
		
		//������������ɫ���Ӿ��鼰��ҡ�����BOSS��������������
		//�����ȳ�1.0ת��Ϊdouble������ȼ���������������uint��δӶ��õ�һ�������
		dTmp = atan(0.3 * (1.0 * monster_cur->level - myRole->level));
		nDropExp = monster_cur->exp * ((dTmp + 1.58) / 2);
		nDropCoin = nDropExp * 0.1;
		myRole->exp += nDropExp;
		myRole->coin += nDropCoin;

		if (bBoss)
		{
			nDropRep = nDropExp * 0.01;
			myRole->reputation += nDropRep;
		}

		if (myRole->exp > ui.progressBar_role_exp->maximum())
			ui.progressBar_role_exp->setValue(ui.progressBar_role_exp->maximum());
		else
			ui.progressBar_role_exp->setValue(myRole->exp);

		ui.edit_display->append(QStringLiteral("<font color=black>ս��ʤ��!</font>"));
		if (bCheckConcise)
		{
			strTmp = QStringLiteral("<font color=black>������") + QString::number(nCount_attack) + QStringLiteral("��</font>");
			ui.edit_display->append(strTmp);
			strTmp = QStringLiteral("<font color=black>�񵲣�") + QString::number(nCount_parry) + QStringLiteral("��</font>");
			ui.edit_display->append(strTmp);
		}

		if (bBoss)	{
			++nCount_boss;
		}	else	{
			++nCount_normalMonster;
		}
		
		nCount_exp += nDropExp;
		nCount_coin += nDropCoin;
		nCount_rep += nDropRep;
		
		DisplayDropBasic(nDropExp, nDropCoin, nDropRep);
		CalcDropItemsAndDisplay(monster_cur->ID);

		//�����ɫʤ������δ��ѡ�Զ�ս�������������ٴε������ʼս����
		if (!bCheckAuto)
		{
			killTimer(nFightTimer);
			ui.btn_start->setEnabled(true);
		}
	}
}
void fight_fight::Action_monster(void)
{	
	time_remain_monster += monster_cur->interval;	//���ٹ����ʣ��ʱ�䡣	
	++nCount_parry;									//�����һ��

	//���￳��ɫһ�����˺�ֵ = (������������-��ɫ����������� + (����ħ�������� - ��ɫħ���������� + 2 (ǿ���˺�)
	qint32 monster_dc = monster_cur->DC1 + qrand() % (monster_cur->DC2 - monster_cur->DC1 + 1);
	qint32 monster_mc = monster_cur->MC1 + qrand() % (monster_cur->MC2 - monster_cur->MC1 + 1);
	qint32 role_ac = role_ac1 + qrand() % (role_ac2 - role_ac1 + 1);
	qint32 role_mac = role_mac1 + qrand() % (role_mac2 - role_mac1 + 1);
	qint32 damage_dc = (monster_dc - role_ac);
	qint32 damage_mc = (monster_mc - role_mac);
	qint32 nTmp = (damage_dc > 1 ? damage_dc : 1) + (damage_mc > 1 ? damage_mc : 1);
	
	role_hp_c -= nTmp;
	if (role_hp_c < 0)
	{
		role_hp_c = 0;
	}
	ui.progressBar_role_hp->setValue(role_hp_c);

	//�����Ѫ
	monster_cur_hp += monster_cur_rhp;
	if (monster_cur_hp > monster_cur->hp)
		monster_cur_hp = monster_cur->hp;
	if (monster_cur_hp < 0)
		monster_cur_hp = 0;
	ui.progressBar_monster_hp->setValue(monster_cur_hp);

	//�ǡ����ģʽ������ʾ�˺���Ϣ��
	if (!bCheckConcise)
	{
		QList<qint32> list;
		list.append(nTmp);
		ui.edit_display->append(Generate_Display_LineText(monster_cur->name, QStringLiteral("�չ�"), QStringLiteral("��"), list));
	}

	if (role_hp_c <= 0)
	{
		//����ս��״̬Ϊ��ս�������ҽ�ɫ�����󲻿��ٴ�ս����
		bFighting = false;
		killTimer(nFightTimer);
		ui.checkBox_auto->setChecked(false);

		//��ɫ��������ʧ����30%�����20%
		quint32 nExp = myRole->exp * 0.3;
		quint32 nCoin = myRole->coin * 0.2;
		myRole->exp -= nExp;
		myRole->coin -= nCoin;

		ui.progressBar_role_exp->setValue(myRole->exp);
		ui.edit_display->append(QStringLiteral("<font color=black>ս��ʧ��!</font>"));
		ui.edit_display->append(QStringLiteral("��ʧ���飺") + QString::number(nExp));
		ui.edit_display->append(QStringLiteral("��ʧ��ң�") + QString::number(nCoin));
	}
}

void fight_fight::GenerateMonster()
{
	bBoss = false;
	if (bCheckFindBoss && monster_boss_count > 0)
	{
		bBoss = (1.0 * qrand() / RAND_MAX) > g_fight_boss_probability;
	}
	if (bBoss)
	{
		qint32 n = qrand() % monster_boss_count;
		monster_cur = &g_MonsterBoss_list[monster_boss_assign[n]];

		QString strTmp = QStringLiteral("ǿ���<font size = 4 color=blue>") + monster_cur->name
			+ QStringLiteral("</font>��Ϯ,�¸ҵ�<font size = 5 color = red>ս</font>�ɣ�");
		ui.edit_display->setText(strTmp);
	}
	else
	{
		qint32 n = qrand() % monster_normal_count;
		monster_cur = &g_MonsterNormal_List[monster_normal_assign[n]];
		ui.edit_display->setText("");
	}
}

void fight_fight::timerEvent(QTimerEvent *event)
{
	//ÿһ��timerEventΪһ���غϡ� 
	//��ǰδ����ս��״̬������ʱ��ʾ��һ�ε�ս����Ϣ����ʱ���������һ�����
	if (!bFighting)
	{
		--nShowStatusRound;
		if (nShowStatusRound >= 0)
		{
			return;
		}
		
		nShowStatusRound = 10;
		//����һ���������ʾ������Ϣ��
		GenerateMonster();
		Display_CurrentMonsterInfo();
		bFighting = true;
		time_remain = time_remain_role = time_remain_monster = 0;
		nCount_attack = nCount_parry = 0;
		ui.edit_display->append(QStringLiteral("<font color=black>ս����ʼ</font>"));
	}

	//�غ�ʱ�������꣬�ж�ս����ʱ����ֹͣ����ս���������Զ�ս����
	if (time_remain >= 600000)
	{
		ui.edit_display->append(QStringLiteral("ս����ʱ��"));
		bTimeOut = true;
		killTimer(nFightTimer);
		ui.checkBox_auto->setCheckState(Qt::Unchecked);
		return;
	}
	
	//���غ�ʱ����ڽ�ɫʱ�䣬���ɫ�һ�غϡ����жϣ����غ�ʱ��С�ڹ���ʱ�䣬�����һ�غϡ�
	if (time_remain > time_remain_role)
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
	time_remain += interval;
}

void fight_fight::updateRoleBuffInfo(void)
{
	qint32 i;
	role_rhp = 0;
	role_ac1 = myRole->ac1;
	role_ac2 = myRole->ac2;
	role_mac1 = myRole->mac1;
	role_mac2 = myRole->mac2;

	for (i = 0; i < MaxBuffCount && i < buffInRole.size(); i++)
	{
		--buffInRole[i].time;
		if (buffInRole[i].time <= 0)
		{
			buffDisp_Role[i]->setPixmap(QPixmap(""));
			buffInRole.remove(i);
		}
		else
		{
			role_rhp += buffInRole[i].rhp;
			role_ac1 += buffInRole[i].ac;
			role_ac2 += buffInRole[i].ac;
			role_mac1 += buffInRole[i].mac;
			role_mac2 += buffInRole[i].mac;
		}
	}

	i = 0;
	for (; i < buffInRole.size(); i++)
	{
		buffDisp_Role[i]->setPixmap(buffInRole[i].icon);
	}
	for (; i < MaxBuffCount; i++)
	{
		buffDisp_Role[i]->setPixmap(QPixmap(""));
	}
	
	QString strTmp;
	ui.edit_role_rhp->setText(QString::number(role_rhp));
	strTmp = QString::number(role_ac1) + "-" + QString::number(role_ac2);
	ui.edit_role_ac->setText(strTmp);
	strTmp = QString::number(role_mac1) + "-" + QString::number(role_mac2);
	ui.edit_role_mac->setText(strTmp);
}
void fight_fight::updateMonsterBuffInfo(void)
{
	qint32 i,nTmp;
	nTmp = 0;
	monster_cur_ac = monster_cur->AC;
	monster_cur_mac = monster_cur->MAC;

	for (i = 0; i < MaxBuffCount && i < buffInMonster.size(); i++)
	{
		--buffInMonster[i].time;
		if (buffInMonster[i].time <= 0)
		{
			buffDisp_Mon[i]->setPixmap(QPixmap(""));
			buffInMonster.remove(i);
		}
		else
		{
			nTmp -= buffInMonster[i].rhp;
			monster_cur_ac -= buffInMonster[i].ac;
			monster_cur_mac -= buffInMonster[i].mac;
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

	//���BOSSû�м���buff,��ָ���ԭ���Ļ�Ѫ���á�
	if (nTmp >= 0)
		monster_cur_rhp = monster_cur->hp >> 7;
	else
		monster_cur_rhp = nTmp;

	if (monster_cur_ac < 0)
	{
		monster_cur_ac = 0;
	}
	if (monster_cur_mac < 0)
	{
		monster_cur_mac = 0;
	}
	ui.edit_monster_rhp->setText(QString::number(monster_cur_rhp));
	ui.edit_monster_ac->setText(QString::number(monster_cur_ac));
	ui.edit_monster_mac->setText(QString::number(monster_cur_mac));
}

void fight_fight::updateSkillCD()
{
	for (int i = 0; i < fightingSkill.size(); i++)
	{
		--fightingSkill[i].cd_c;
	}
}