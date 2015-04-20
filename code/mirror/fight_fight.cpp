#include "fight_fight.h"
#include <QFile>
#include <QMessageBox>
#include <QThread>

const int interval = 100;

extern QVector<Info_Item> g_ItemList;
extern QVector<Info_Distribute> g_MonsterDistribute;
extern QVector<MonsterInfo> g_MonsterNormal_List;
extern QVector<MonsterInfo> g_MonsterBoss_list;

fight_fight::fight_fight(QWidget* parent, qint32 id, RoleInfo *info, MapItem *bag_item)
: QDialog(parent), m_mapID(id), myRole(info), m_bag_item(bag_item)
{
	ui.setupUi(this);

	ui.edit_display->setText(QString::number(m_mapID));

	Cacl_Display_Role_Value();
	LoadItem();

	AssignMonster(g_MonsterNormal_List, g_MonsterBoss_list, g_MonsterDistribute);
	monster_cur = &g_MonsterNormal_List[monster_normal_assign[0]];
	Display_CurrentMonsterInfo();

	bKeepFight = bFighting = false;	

	ui.progressBar_monster_hp->setStyleSheet("QProgressBar::chunk { background-color: rgb(255, 0, 0) }");
	ui.progressBar_monster_mp->setStyleSheet("QProgressBar::chunk { background-color: rgb(0, 0, 255) }");
	ui.edit_monster_sc->setText("0 - 0");
}

fight_fight::~fight_fight()
{
	
}

void fight_fight::on_btn_quit_clicked(void)
{
	if (!bFighting)
	{
		this->close();
	}
	else
	{
		QString title = QString::fromLocal8Bit("��ʾ");
		QString message = QString::fromLocal8Bit("��ǰ����ս���У����ܽ���ʧ50%������30%��ҡ�");
		QMessageBox msgBox(QMessageBox::Question, title, message);
		QPushButton *YsBtn = msgBox.addButton(QString::fromLocal8Bit(" �� "), QMessageBox::AcceptRole);
		QPushButton *NoBtn = msgBox.addButton(QString::fromLocal8Bit(" �� "), QMessageBox::RejectRole);
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
	nCount_attack = nCount_parry= nCount_item = nRound = 0;
	nShowStatusRound = 5;

	//����һ���������ʾ������Ϣ��
	bBoss = false;	
	if (ui.checkBox_boss->isChecked() && monster_boss_count > 0)
	{
		bBoss = (qrand() % 100) > 80;
	}
	if (bBoss)
	{
		qint32 n = qrand() % monster_boss_count;
		monster_cur = &g_MonsterBoss_list[monster_boss_assign[n]];

		QString strTmp = QString::fromLocal8Bit("ǿ���<font size = 4 color=blue>") + monster_cur->name
			+ QString::fromLocal8Bit("</font>��Ϯ,�¸ҵ�<font size = 5 color = red>ս</font>�ɣ�");
			ui.edit_display->setText(strTmp);
	}
	else
	{
		qint32 n = qrand() % monster_normal_count;
		monster_cur = &g_MonsterNormal_List[monster_normal_assign[n]];

		ui.edit_display->setText("");
	}
	Display_CurrentMonsterInfo();

	ui.edit_display->append(QString::fromLocal8Bit("ս����ʼ"));

	nFightTimer = startTimer(interval);
	bFighting = true;
	ui.btn_start->setEnabled(false);
}

void fight_fight::Cacl_Display_Role_Value()
{
	ui.edit_role_name->setText(myRole->name);
	ui.edit_role_level->setText(QString::number(myRole->level));

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
}

Info_Item* fight_fight::FindItem(quint32 ID)
{
	for (QVector<Info_Item>::iterator iter = g_ItemList.begin(); iter != g_ItemList.end(); iter++)
	{
		if (iter->ID == ID)
		{
			return &*iter;
		}
	}
	return NULL;
}
Info_Item* fight_fight::FindItem(const QString &name)
{
	for (QVector<Info_Item>::iterator iter = g_ItemList.begin(); iter != g_ItemList.end(); iter++)
	{
		if (iter->name == name)
		{
			return &*iter;
		}
	}
	return NULL;
}

void fight_fight::LoadItem()
{
	QString strTmp;
	Info_Item *itemItem;
	for (MapItem::iterator iter = m_bag_item->begin(); iter != m_bag_item->end(); iter++)
	{
		itemItem = FindItem(iter.key());
		if (itemItem != NULL)
		{
			if (itemItem->type == et_immediate_hp)
			{
				strTmp = Generate_ItemComboBox_Text(itemItem->name, QString::fromLocal8Bit("Ѫ"), itemItem->value, iter.value());
				ui.comboBox_hp->addItem(strTmp);
			}
			else if (itemItem->type == et_immediate_mp)
			{
				strTmp = Generate_ItemComboBox_Text(itemItem->name, QString::fromLocal8Bit("ħ"), itemItem->value, iter.value());;
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
		if (Distribute[i].mapID == m_mapID)
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
	ui.edit_monster_level->setText(QString::number(monster_cur->level));
	ui.edit_monster_dc->setText(QString::number(monster_cur->DC1) + " - " + QString::number(monster_cur->DC2));
	ui.edit_monster_mc->setText(QString::number(monster_cur->MC1) + " - " + QString::number(monster_cur->MC2));
	ui.edit_monster_ac->setText(QString::number(monster_cur->AC));
	ui.edit_monster_mac->setText(QString::number(monster_cur->MAC));
	ui.edit_monster_interval->setText(QString::number(monster_cur->interval));
}

inline QString fight_fight::Generate_ItemComboBox_Text(const QString &name, const QString &type, quint32 value, quint32 count)
{
	QString strSplit = QString::fromLocal8Bit(" ");
	QString strTmp = name;
	strTmp += strSplit + type + QString::fromLocal8Bit(":") + QString::number(value);
	strTmp += strSplit + QString::fromLocal8Bit("ʣ:") + QString::number(count);
	return strTmp;
}
inline QString fight_fight::Generate_Display_LineText(const QString &str1, const QString &skill, const QString &str2, quint32 damage)
{
	QString strTmp = QString::fromLocal8Bit("<font color=blue>") + str1
		+ QString::fromLocal8Bit("</font>ʹ��<font color=darkRed>") + skill
		+ QString::fromLocal8Bit("</font>����<font color = blue>") + str2
		+ QString::fromLocal8Bit("</font>����˺�:<font color = magenta>") + QString::number(damage)
		+ QString::fromLocal8Bit("</font>");
	return strTmp;
}

void fight_fight::Step_role_UsingItem_hp(void)
{
	++nCount_item;

	quint32 ID;
	bool bHasNotItem = true;

	QString strTmp = ui.comboBox_hp->currentText();
	QStringList strList = strTmp.split(" ");

	Info_Item *itemItem = FindItem(strList.at(0));
	if (itemItem != NULL)
	{
		ID = itemItem->ID;
		//������Ӧ����������1
		m_bag_item->insert(ID, m_bag_item->value(ID) - 1); 
		strTmp = Generate_ItemComboBox_Text(itemItem->name, QString::fromLocal8Bit("Ѫ"), itemItem->value, m_bag_item->value(ID));
		ui.comboBox_hp->setItemText(ui.comboBox_hp->currentIndex(), strTmp);

		//���Ľ�ɫ״̬
		role_hp_c += itemItem->value;
		if (role_hp_c >= myRole->hp)
		{
			role_hp_c = myRole->hp;
		}
		ui.progressBar_role_hp->setValue(role_hp_c);
		if (!ui.checkBox_concise->isChecked())
		{
			strTmp = QString::fromLocal8Bit("��ʹ���ˣ�") + itemItem->name;
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
			}
			m_bag_item->remove(ID);
		}
	}
	else
	{	//�Ҳ�����Ӧ���ߣ�����Զ���Ѫ��ѡ��
		ui.checkBox_hp->setCheckState(Qt::Unchecked);
	}
}
void fight_fight::Step_role_UsingItem_mp(void)
{
	++nCount_item;
}

void fight_fight::Step_role_NormalAttack(void)
{
	++nCount_attack;
	//��ɫ��ͨ�������˺�ֵ = (��ɫ������-��������� + (��ɫħ�������� + ��ɫ���������� - ����ħ����
	qint32 nDamage, role_dc, role_mc, role_sc;
	role_dc = myRole->dc1 + qrand() % (myRole->dc2 - myRole->dc1 + 1);
	role_mc = myRole->mc1 + qrand() % (myRole->mc2 - myRole->mc1 + 1);
	role_sc = myRole->sc1 + qrand() % (myRole->sc2 - myRole->sc1 + 1);
	qint32 damage_dc = (role_dc - monster_cur->AC);
	qint32 damage_mc = (role_mc + role_sc - monster_cur->MAC);
	nDamage = (damage_dc > 0 ? damage_dc : 0) + (damage_mc > 0 ? damage_mc : 0);
	monster_cur_hp -= nDamage;
	if (monster_cur_hp <= 0)
	{
		monster_cur_hp = 0;
	}
	ui.progressBar_monster_hp->setValue(monster_cur_hp);

	if (!ui.checkBox_concise->isChecked())
	{
		ui.edit_display->append(
			Generate_Display_LineText(QString::fromLocal8Bit("��"), QString::fromLocal8Bit("��������"), monster_cur->name, nDamage)
			);
	}
}
void fight_fight::Step_role_SkillAttack(void)
{
	++nCount_attack;
}

void fight_fight::Action_role(void)
{
	//���ٽ�ɫ��ʣ��ʱ�䡣
	time_remain_role += myRole->intervel;

	//ʹ�õ��ߵ�����
	qint32 limit_rhp = myRole->hp * ui.edit_hp->text().toInt() / 100;
	qint32 limit_rmp = myRole->mp * ui.edit_mp->text().toInt() / 100;

	//�����ѡ���Զ�ʹ�õ���
	if (ui.checkBox_hp->isChecked() && role_hp_c < limit_rhp)
	{
		Step_role_UsingItem_hp();
	}
	else if (ui.checkBox_mp->isChecked() && role_mp_c < limit_rmp)
	{
		Step_role_UsingItem_mp();
	}
	else if (false /*�����б�*/)
	{
		Step_role_SkillAttack();
	}
	else
	{
		Step_role_NormalAttack();
	}

	quint32 nDropExp, nDropCoin, nDropRep;

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

		ui.edit_display->append(QString::fromLocal8Bit("ս��ʤ��!"));
		if (ui.checkBox_concise->isChecked())
		{
			QString strTmp;
			strTmp = QString::fromLocal8Bit("������") + QString::number(nCount_attack) + QString::fromLocal8Bit("��");
			ui.edit_display->append(strTmp);
			strTmp = QString::fromLocal8Bit("�񵲣�") + QString::number(nCount_parry) + QString::fromLocal8Bit("��");
			ui.edit_display->append(strTmp);
			strTmp = QString::fromLocal8Bit("ʹ�õ��ߣ�") + QString::number(nCount_item) + QString::fromLocal8Bit("��");
			ui.edit_display->append(strTmp);
		}
		ui.edit_display->append(QString::fromLocal8Bit("��þ���:") + QString::number(nDropExp));
		ui.edit_display->append(QString::fromLocal8Bit("��ý��:") + QString::number(nDropCoin));
		if (bBoss)
		{
			ui.edit_display->append(QString::fromLocal8Bit("�������:") + QString::number(nDropRep));
		}
	}
}
void fight_fight::Action_monster(void)
{
	//���ٹ����ʣ��ʱ�䡣
	time_remain_monster += monster_cur->interval;

	//�����һ��
	++nCount_parry;

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
	if (!ui.checkBox_concise->isChecked())
	{
		ui.edit_display->append(
			Generate_Display_LineText(monster_cur->name, QString::fromLocal8Bit("�չ�"), QString::fromLocal8Bit("��"), nTmp)
			);
	}

	if (role_hp_c <= 0)
	{
		//����ս��״̬Ϊ��ս�������ҽ�ɫ�����󲻿��ٴ�ս����
		bFighting = false;
		ui.btn_start->setEnabled(false);
		ui.checkBox_auto->setChecked(false);

		//��ɫ��������ʧ���顢��������Ҹ�10%
		quint32 nExp = myRole->exp * 0.1;
		quint32 nCoin = myRole->coin * 0.1;
		quint32 nRep = myRole->reputation * 0.1;
		myRole->exp -= nExp;
		myRole->coin -= nCoin;
		myRole->reputation -= nRep;

		ui.progressBar_role_exp->setValue(myRole->exp);
		ui.edit_display->append(QString::fromLocal8Bit("ս��ʧ��!"));
		ui.edit_display->append(QString::fromLocal8Bit("��ʧ���飺") + QString::number(nExp));
		ui.edit_display->append(QString::fromLocal8Bit("��ʧ��ң�") + QString::number(nCoin));
		ui.edit_display->append(QString::fromLocal8Bit("��ʧ������") + QString::number(nRep));
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

//	++nRound;
//	if (!ui.checkBox_concise->isChecked())
//	{
//		ui.edit_display->append(QString::fromLocal8Bit("��") + QString::number(nRound)
//			+ QString::fromLocal8Bit("�غ�"));
//	}

	//�غ�ʱ�������꣬�ж�ս����ʱ����ֹͣ����ս���������Զ�ս����
	if (time_remain >= 300000)
	{
		ui.edit_display->append(QString::fromLocal8Bit("ս����ʱ��"));
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
