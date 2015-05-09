#include "fight_fight.h"
#include <QFile>
#include <QMessageBox>
#include <QThread>
#include <time.h>

#include "Item_Base.h"
#include "def_System_para.h"

const int interval = 100;

//���岢��ʼ����̬���ݳ�Ա��
bool fight_fight::bCheckHp = false;
bool fight_fight::bCheckMp = false;
bool fight_fight::bCheckQuickFight = false;
bool fight_fight::bCheckConcise = false;
bool fight_fight::bCheckFindBoss = false;
qint32 fight_fight::pickFilter = 0;

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

	bKeepFight = bFighting = false;	
	m_dlg_fightInfo = nullptr;

	connect(ui.comboBox_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(pickFilterChange(int)));
}

fight_fight::~fight_fight()
{
	//�洢checkBox��״̬�仯Ϊstatic����������ٴν���ս������ʱ�Զ���ѡ��Ӧѡ��
	bCheckQuickFight = ui.checkBox_quick->isChecked();
	bCheckFindBoss = ui.checkBox_boss->isChecked();
}

void fight_fight::on_btn_quit_clicked(void)
{
	if (!bFighting)
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
	time_remain = time_remain_role = time_remain_monster = 0;
	nCount_attack = nCount_parry = nRoundCount_role = nRoundCount_monster = 0;
	nShowStatusRound = 5;

	//����һ���������ʾ������Ϣ��
	bBoss = false;	
	if (ui.checkBox_boss->isChecked() && monster_boss_count > 0)
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
	Display_CurrentMonsterInfo();

	ui.edit_display->append(QStringLiteral("ս����ʼ"));

	nFightTimer = startTimer(interval);
	bFighting = true;
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
	}	
}
void fight_fight::pickFilterChange(int index)
{
	pickFilter = index + 1;
}

void fight_fight::InitUI()
{
	ui.checkBox_MultipleDrop->setVisible(false);
	ui.checkBox_MultipleExp->setVisible(false);
	ui.checkBox_MultipleRep->setVisible(false);
	ui.edit_MultipleDrop->setVisible(false);
	ui.edit_MultipleExp->setVisible(false);
	ui.edit_MultipleRep->setVisible(false);

	ui.progressBar_monster_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_monster_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");
	ui.edit_monster_sc->setText("0 - 0");

 	ui.checkBox_hp->setChecked(bCheckHp);
 	ui.checkBox_mp->setChecked(bCheckMp);
 	ui.checkBox_concise->setChecked(bCheckConcise);
 	ui.checkBox_quick->setChecked(bCheckQuickFight);
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
// 	ui.checkBox_quick->setStyleSheet("QCheckBox{ background:transparent} ");
// 	ui.checkBox_boss->setStyleSheet("QCheckBox{ background:transparent} ");

//	ui.groupBox->setStyleSheet("QGroupBox{ background:transparent} ");

//	ui.edit_hp->setStyleSheet("QLineEdit{ background:transparent} ");
//	ui.edit_mp->setStyleSheet("QLineEdit{ background:transparent} ");
//	ui.edit_display->setStyleSheet("QTextEdit{ background:transparent} ");

//	ui.btn_start->setStyleSheet("QPushButton{ background:rgb(170, 170, 170)} ");
//	ui.btn_quit->setStyleSheet("QPushButton{ background:rgb(170, 170, 170)} ");
//	ui.btn_statistics->setStyleSheet("QPushButton{ background:rgb(170, 170, 170)} ");
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

	for (VecRoleSkill::const_iterator iterRole = myRole->skill.begin(); iterRole != myRole->skill.end(); iterRole++)
	{
		for (QVector<Info_skill>::iterator iterSkill = g_skillList.begin(); iterSkill != g_skillList.end(); iterSkill++)
		{
			if (iterRole->id == iterSkill->ID)
			{
				iterSkill->level = iterRole->level;
				fightingSkill.append(*iterSkill);
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
		if (itemItem != NULL)
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
	monster_cur_mp = monster_cur->mp;
	ui.progressBar_monster_hp->setValue(monster_cur_hp);
	ui.progressBar_monster_mp->setValue(monster_cur_mp);
	
	//�ظ��塢ħΪ���ֵ	
	monster_cur_rhp = monster_cur_hp >> 7;	
	monster_cur_rmp = monster_cur_mp >> 7;
	ui.edit_monster_rhp->setText(QString::number(monster_cur_rhp));
	ui.edit_monster_rmp->setText(QString::number(monster_cur_rmp));

	//����ͷ��
	ui.label_monster_head->setPixmap(QPixmap::fromImage(monster_cur->Head));

	//������������
	ui.edit_monster_name->setText(monster_cur->name);
	ui.edit_monster_level->setText(QStringLiteral("Lv:") + QString::number(monster_cur->level));
	ui.edit_monster_dc->setText(QString::number(monster_cur->DC1) + " - " + QString::number(monster_cur->DC2));
	ui.edit_monster_mc->setText(QString::number(monster_cur->MC1) + " - " + QString::number(monster_cur->MC2));
	ui.edit_monster_ac->setText(QString::number(monster_cur->AC));
	ui.edit_monster_mac->setText(QString::number(monster_cur->MAC));
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
	bool bHasNotItem = true;

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
			strTmp = QStringLiteral("��ʹ���ˣ�") + itemItem->name;
			ui.edit_display->append(strTmp);
		}

		//��������Ѿ����꣬��ɾ����ǰ����.������е��ߣ����л���0�ŵ��ߣ���������Զ���Ѫ��ѡ��
		if (m_bag_item->value(ID) <= 0)
		{
			ui.comboBox_hp->removeItem(ui.comboBox_hp->currentIndex());
			if (ui.comboBox_hp->count() > 0)
			{
				bHasNotItem = false;
				ui.comboBox_hp->setCurrentIndex(0);
			}
			else
			{
				ui.checkBox_hp->setChecked(false);
				bCheckHp = false;
			}
			m_bag_item->remove(ID);
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
	bool bHasNotItem = true;

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
			strTmp = QStringLiteral("��ʹ���ˣ�") + itemItem->name;
			ui.edit_display->append(strTmp);
		}

		//��������Ѿ����꣬��ɾ����ǰ����.������е��ߣ����л���0�ŵ��ߣ���������Զ���Ѫ��ѡ��
		if (m_bag_item->value(ID) <= 0)
		{
			ui.comboBox_mp->removeItem(ui.comboBox_mp->currentIndex());
			if (ui.comboBox_mp->count() > 0)
			{
				bHasNotItem = false;
				ui.comboBox_mp->setCurrentIndex(0);
			}
			else
			{
				ui.checkBox_mp->setChecked(false);
				bCheckMp = false;
			}
			m_bag_item->remove(ID);
		}
	}
	else
	{	//�Ҳ�����Ӧ���ߣ������ѡ��
		ui.checkBox_mp->setCheckState(Qt::Unchecked);
		bCheckMp = false;
	}
}

void fight_fight::Step_role_Attack(void)
{
	++nCount_attack;
	qint32 index = (nRoundCount_role-1) % fightingSkill.size();
	const Info_skill &skill = fightingSkill.at(index);
	
	if (role_mp_c < skill.spell[skill.level - 1])	//�ȼ���1��ʼ�������±��0��ʼ
	{
		QString strTmp = QStringLiteral("<font color=red>ħ�����㣬�޷�ʩ�ż���:");
		strTmp += skill.name + QStringLiteral("</font>");
		ui.edit_display->append(strTmp);
		return;
	}

	role_mp_c -= skill.spell[skill.level - 1];
	ui.progressBar_role_mp->setValue(role_mp_c);

	QList<qint32> ListDamage;
	for (qint32 i = 0; i < skill.times; i++)
	{
		//�������˺�ֵ = (��ɫ������-���������
		//ħ�������˺�ֵ =  (��ɫħ�������� + ��ɫ���������� - ����ħ����
		qint32 nDamage, nTmp;
		if (myRole->vocation == 1 || skill.ID == 220000)
		{
			nTmp = myRole->dc1 + qrand() % (myRole->dc2 - myRole->dc1 + 1);
			nTmp = nTmp * skill.damage[skill.level - 1] / 100;
			nDamage = (nTmp - monster_cur->AC);
		}
		else if (myRole->vocation == 2)
		{
			nTmp = myRole->mc1 + qrand() % (myRole->mc2 - myRole->mc1 + 1);
			nTmp = nTmp * skill.damage[skill.level - 1] / 100;
			nDamage = (nTmp - monster_cur->MAC);
		}
		else
		{
			nTmp = myRole->sc1 + qrand() % (myRole->sc2 - myRole->sc1 + 1);
			nTmp = nTmp * skill.damage[skill.level - 1] / 100;
			nDamage = (nTmp - monster_cur->MAC);
		}
		nDamage = (nDamage >= 0 ? nDamage : 0);
		monster_cur_hp -= nDamage;
		if (monster_cur_hp <= 0)
		{
			monster_cur_hp = 0;
		}
		ui.progressBar_monster_hp->setValue(monster_cur_hp);

		ListDamage.append(nDamage);
	}
	if (!bCheckConcise && skill.times != 0)
	{
		ui.edit_display->append(Generate_Display_LineText(QStringLiteral("��"), skill.name, monster_cur->name, ListDamage));
	}

	if (skill.buff != 0)
	{
		QString strTmp = QStringLiteral("��ʹ��:") + skill.name
			+ QStringLiteral(",�������buffer ") + QString::number(skill.buff_time) + QStringLiteral("�غϡ�");
		ui.edit_display->append(strTmp);
		ui.edit_display->append(QStringLiteral("<font color=red>buffer��Ϊ���ԣ�����Ч����</font>"));
	}
}

inline void fight_fight::DisplayDropBasic(quint32 nDropExp, quint32 nDropCoin, quint32 nDropRep)
{
	ui.edit_display->append(QStringLiteral("��þ���:") + QString::number(nDropExp));
	ui.edit_display->append(QStringLiteral("��ý��:") + QString::number(nDropCoin));
	if (bBoss)
	{
		ui.edit_display->append(QStringLiteral("�������:") + QString::number(nDropRep));
	}
}

void fight_fight::CreateEquip(itemID id, Info_Equip &DropEquip)
{
	const Info_basic_equip *EquipBasicInfo = Item_Base::GetEquipBasicInfo(id);
	//�����Ӿ�����Ʒװ���ļ�Ʒ������(���ϵȼ���ʾװ���ȼ�Խ�ߣ�Խ�����׳��ּ�Ʒ)
	qint32 extraAmount = (qint32)(pow((1.0 * qrand() / RAND_MAX), g_specialEquip_probability * EquipBasicInfo->lv) * g_specialEquip_MaxExtra);
	EquipExtra extra = { 0 };
	quint32 *p, extraPara = sizeof(EquipExtra) / sizeof(quint32);
	quint32 index, nCount, type;

	//�˴�ǿ��ת����Ϊ�������ʵ�ּ�Ʒװ�������Ե�λ�ü���С�����������ء�
	p = (quint32 *)&extra;

	//��ʼ��
	DropEquip = { 0 };

	DropEquip.ID = id;
	DropEquip.lvUp = 0;
	DropEquip.extraAmount = extraAmount;

	//���������������������档
	while (extraAmount > 0)
	{
		index = qrand() % extraPara;
		nCount = qrand() % g_specialEquip_MaxExtra;

		p[index] = (extraAmount < nCount) ? extraAmount : nCount;
		extraAmount -= nCount;
	}
	
	//ֻ�����������������������˼ӳɡ�
	type = (EquipBasicInfo->ID - g_itemID_start_equip) / 100;
	if (type != g_equipType_weapon && type != g_equipType_necklace)
	{
		DropEquip.extraAmount -= extra.luck;
		extra.luck = 0;
	}
	DropEquip.extra = extra;
}

void fight_fight::CalcDropItemsAndDisplay(monsterID id)
{
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
				ui.edit_display->append(QStringLiteral("���:") + equip->name);
				if (m_bag_equip->size() >= g_bag_maxSize)
				{
					myRole->coin += equip->price >> 1;
					ui.edit_display->append(QStringLiteral("��������������:") + equip->name
						+ QStringLiteral(" ��ý��:") + QString::number(equip->price >> 1));
				}
				else if (equip->lv >= pickFilter || DropEquip.extraAmount > 0)
				{
					m_bag_equip->append(DropEquip);
				}
				else
				{
					myRole->coin += equip->price >> 1;
					ui.edit_display->append(QStringLiteral("����:") + equip->name 
										   + QStringLiteral(" ��ý��:") + QString::number(equip->price >> 1));
				}
			}
			else
			{
				//��������
				const Info_Item *item = Item_Base::FindItem_Item(rRat.ID);
				ui.edit_display->append(QStringLiteral("���:") + item->name);
				m_bag_item->insert(rRat.ID, m_bag_item->value(rRat.ID) + 1);
			}
		}
	}
}

void fight_fight::Action_role(void)
{
	time_remain_role += myRole->intervel;	//���ٽ�ɫ��ʣ��ʱ�䡣
	++nRoundCount_role;						//��ɫ�غϼ���

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
		Step_role_Attack();
	}

	quint32 nDropExp, nDropCoin, nDropRep = 0;
	QString strTmp;

	if (monster_cur_hp <= 0)
	{
		bFighting = false;

		//������������ɫ���Ӿ��鼰��ҡ�����BOSS��������������
		nDropExp = monster_cur->exp;
		nDropCoin = nDropExp * 0.1;
		myRole->exp += nDropExp;
		myRole->coin += nDropCoin;

		if (bBoss)
		{
			nDropRep = nDropExp * 0.01;
			myRole->reputation += nDropRep;
		}

		if (myRole->exp >= ui.progressBar_role_exp->maximum())
			ui.progressBar_role_exp->setValue(ui.progressBar_role_exp->maximum());
		else
			ui.progressBar_role_exp->setValue(myRole->exp);

		ui.edit_display->append(QStringLiteral("ս��ʤ��!"));
		if (bCheckConcise)
		{
			strTmp = QStringLiteral("������") + QString::number(nCount_attack) + QStringLiteral("��");
			ui.edit_display->append(strTmp);
			strTmp = QStringLiteral("�񵲣�") + QString::number(nCount_parry) + QStringLiteral("��");
			ui.edit_display->append(strTmp);
		}

		if (bBoss)
		{
			++nCount_boss;
		}
		else
		{
			++nCount_normalMonster;
		}
		
		nCount_exp += nDropExp;
		nCount_coin += nDropCoin;
		nCount_rep += nDropRep;
		
		DisplayDropBasic(nDropExp, nDropCoin, nDropRep);
		CalcDropItemsAndDisplay(monster_cur->ID);
	}
}
void fight_fight::Action_monster(void)
{	
	time_remain_monster += monster_cur->interval;	//���ٹ����ʣ��ʱ�䡣	
	++nRoundCount_monster;							//�����غϼ���
	++nCount_parry;									//�����һ��

	//���￳��ɫһ�����˺�ֵ = (������������-��ɫ����������� + (����ħ�������� - ��ɫħ����������
	qint32 monster_dc = monster_cur->DC1 + qrand() % (monster_cur->DC2 - monster_cur->DC1 + 1);
	qint32 monster_mc = monster_cur->MC1 + qrand() % (monster_cur->MC2 - monster_cur->MC1 + 1);
	qint32 role_ac = myRole->ac1 + qrand() % (myRole->ac2 - myRole->ac1 + 1);
	qint32 role_mac = myRole->mac1 + qrand() % (myRole->mac2 - myRole->mac1 + 1);
	qint32 damage_dc = (monster_dc - role_ac);
	qint32 damage_mc = (monster_mc - role_mac);
	qint32 nTmp = (damage_dc > 0 ? damage_dc : 0) + (damage_mc > 0 ? damage_mc : 0);
	
	role_hp_c -= nTmp;
	if (role_hp_c <= 0)
	{
		role_hp_c = 0;
	}
	ui.progressBar_role_hp->setValue(role_hp_c);

	//�����Ѫ������
	monster_cur_hp += monster_cur_rhp;
	if (monster_cur_hp > monster_cur->hp)
	{
		monster_cur_hp = monster_cur->hp;
	}
	ui.progressBar_monster_hp->setValue(monster_cur_hp);
	monster_cur_mp += monster_cur_rmp;	
	if (monster_cur_mp > monster_cur->mp)
	{
		monster_cur_mp = monster_cur->mp;
	}
	ui.progressBar_monster_mp->setValue(monster_cur_mp);

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
		ui.btn_start->setEnabled(false);
		ui.checkBox_auto->setChecked(false);

		//��ɫ��������ʧ���顢��������Ҹ�10%
		quint32 nExp = myRole->exp * 0.3;
		quint32 nCoin = myRole->coin * 0.2;
		quint32 nRep = myRole->reputation * 0.1;
		myRole->exp -= nExp;
		myRole->coin -= nCoin;
		myRole->reputation -= nRep;

		ui.progressBar_role_exp->setValue(myRole->exp);
		ui.edit_display->append(QStringLiteral("ս��ʧ��!"));
		ui.edit_display->append(QStringLiteral("��ʧ���飺") + QString::number(nExp));
		ui.edit_display->append(QStringLiteral("��ʧ��ң�") + QString::number(nCoin));
		ui.edit_display->append(QStringLiteral("��ʧ������") + QString::number(nRep));
	}
}

void fight_fight::timerEvent(QTimerEvent *event)
{
	//ÿһ��timerEventΪһ���غϡ� 
	//��ǰδ����ս��״̬������ʱ��ʾ��һ�ε�ս����Ϣ��ͬʱ����ʱ������ֹͣ��ʱ����������Ƿ�ѡ���Զ�ս�������������ٴ�����ս����
	if (bFighting == false)
	{
		--nShowStatusRound;
		if (nShowStatusRound <= 0)
		{
			killTimer(nFightTimer);
			ui.btn_start->setEnabled(true);
			if (ui.checkBox_auto->isChecked())
			{
				on_btn_start_clicked();
			}
		}	
		return;
	}

// 	if (!bCheckConcise)
// 	{
// 		ui.edit_display->append(QStringLiteral("��") + QString::number(nRoundCount)
// 			+ QStringLiteral("�غ�"));
// 	}

	//�غ�ʱ�������꣬�ж�ս����ʱ����ֹͣ����ս���������Զ�ս����
	if (time_remain >= 600000)
	{
		ui.edit_display->append(QStringLiteral("ս����ʱ��"));
		killTimer(nFightTimer);
		ui.checkBox_auto->setCheckState(Qt::Unchecked);
		return;
	}
	
	//���غ�ʱ����ڽ�ɫʱ�䣬���ɫ�һ�غϡ����жϣ����غ�ʱ��С�ڹ���ʱ�䣬�����һ�غϡ�
	if (time_remain > time_remain_role)
	{
		Action_role();
	}
	else if (time_remain > time_remain_monster)
	{
		Action_monster();
	}
	
	//ս����ʱ
	time_remain += interval;
}
