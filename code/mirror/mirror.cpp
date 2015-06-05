#include "mirror.h"
#include <QMessageBox>
#include <QFile>
#include "def_item_equip.h"
#include "def_takInfo.h"
#include "about.h"

QWidget *g_widget;
QVector<Info_skill> g_skillList;				//�����趨
vecBuff g_buffList;								//buff�趨
QVector<Info_Item> g_ItemList;					//��Ϸ�����б�
QVector<Info_basic_equip> g_EquipList;			//��Ϸװ���б�
QMap<itemID, Info_StateEquip> g_StateEquip;				//��ɫ����װ�����
QMap<mapID, Info_Distribute> g_MonsterDistribute;	//����ֲ��б�
QVector<MonsterInfo> g_MonsterNormal_List;		//��ͨ�����б�
QVector<MonsterInfo> g_MonsterBoss_list;		//BOSS���б�
QVector<info_task> g_task_main_list;			//���������б�
mapDrop	g_mapDropSet;							//���ﱩ���趨
QVector<Info_jobAdd> g_JobAddSet;			//ְҵ�ӳ��趨
roleAddition g_roleAddition;					//��ɫ���Ӳ���

mirror::mirror(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

	g_widget = this;
	bFirstMinimum = false;

	QString strTitle = QStringLiteral("mirror����_beta_0.1.11");
	
	this->setWindowTitle(strTitle);

	if (!LoadRole() || !LoadJobSet())
	{
		QString message = QStringLiteral("����ְҵ�趨ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, QStringLiteral("������"), message);
		exit(0);
	}

	if (!LoadSkill() || !LoadBuff() || !LoadItemList()  || !LoadEquipList() || !LoadStateEquip())
	{
		QString message = QStringLiteral("���ؼ��ܡ����߻�װ��ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, QStringLiteral("������"), message);
		exit(0);
	}
	if (!LoadMonster() || !LoadBoss() || !LoadDistribute() || !LoadDropSet())
	{
		QString message = QStringLiteral("���ع���ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, QStringLiteral("������"), message);
		exit(0);
	}
	if (!LoadTaskSet())
	{
		QString message = QStringLiteral("��������ϵͳʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, QStringLiteral("������"), message);
		exit(0);
	}

//	GiveSomeItem();
	m_tab_fight = new fight(&roleInfo, &m_bag_item, &m_bag_equip);
	ui.stackedWidget_main->addWidget(m_tab_fight);

	m_tab_role = new role(&roleInfo, &m_skill_study, &m_bag_item, &m_storage_item, &m_bag_equip, &m_storage_equip);
	ui.stackedWidget_main->addWidget(m_tab_role);

	m_tab_city = new city(&roleInfo, &m_bag_item);
	ui.stackedWidget_main->addWidget(m_tab_city);
	ui.stackedWidget_main->setCurrentIndex(1);

	bgAudioList = nullptr;
	bgAudio = nullptr;
	if (QFile::exists("./sound/b-2.mp3"))
	{
		bgAudioList = new QMediaPlaylist;
		bgAudioList->setPlaybackMode(QMediaPlaylist::Loop);
		bgAudio = new QMediaPlayer;
		bgAudio->setPlaylist(bgAudioList);
		bgAudio->setVolume(80);
		bgAudioList->addMedia(QUrl::fromLocalFile("./sound/b-2.mp3"));
		bgAudioList->setCurrentIndex(0);
	}

	QIcon mainIcon = QIcon(":/mirror/Resources/mainIcon.png");
	setWindowIcon(mainIcon);
	//����֪ͨ����ͼ��
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(mainIcon);
	trayIcon->setToolTip(roleInfo.name + QStringLiteral("  Level:") + QString::number(roleInfo.level));
//	trayIcon->show();
	
	//����֪ͨ����ͼ�����Ӧ�¼���������
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	QObject::connect(m_tab_role, &role::mirrorSave, this, &mirror::on_mirror_save);
	QObject::connect(m_tab_role, &role::autoSave, this, &mirror::enable_autoSave);
	QObject::connect(m_tab_role, &role::bkSound, this, &mirror::enable_bkSound);
}

mirror::~mirror()
{
	delete m_tab_fight;
	delete m_tab_role;
	delete m_tab_city;
	delete trayIcon;

	if (bgAudio != nullptr)
	{
		bgAudio->stop();
	}
	
	delete bgAudio;
	delete bgAudioList;
}

void mirror::closeEvent(QCloseEvent *event)
{
	QString title = QStringLiteral("�˳�ȷ��");
	QString message = QStringLiteral("�Ƿ���Ҫ������Ϸ��");
	QMessageBox msgBox(QMessageBox::Question, title, message);
	QPushButton *YsBtn = msgBox.addButton(QStringLiteral(" ���沢�˳� "), QMessageBox::AcceptRole);
	QPushButton *NoBtn = msgBox.addButton(QStringLiteral(" ֱ���˳� "), QMessageBox::RejectRole);
	msgBox.exec();
	if (msgBox.clickedButton() == YsBtn)
	{
		silentSave();
	}
}

void mirror::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Trigger:
		setWindowState(Qt::WindowActive);
		activateWindow();
		showNormal();
		trayIcon->hide();
		break;

	default:
		break;
	}
}

void mirror::changeEvent(QEvent *e)
{
	if ((e->type() == QEvent::WindowStateChange) && this->isMinimized())
	{
		hide();

		trayIcon->show();
		trayIcon->setToolTip(roleInfo.name + QStringLiteral("  Level:") + QString::number(roleInfo.level));
		if (!bFirstMinimum)
		{
			QString strMsg = roleInfo.name + QStringLiteral("  Lv:") + QString::number(roleInfo.level);
			trayIcon->showMessage(QStringLiteral("mirror����"), strMsg, QSystemTrayIcon::Information, 500);
			bFirstMinimum = true;
		}
	}
}

void mirror::on_btn_fight_clicked(void)
{
	ui.stackedWidget_main->setCurrentIndex(0);
}
void mirror::on_btn_role_clicked(void)
{
	ui.stackedWidget_main->setCurrentIndex(1);
	m_tab_role->updateRoleInfo();
}
void mirror::on_btn_city_clicked(void)
{
	ui.stackedWidget_main->setCurrentIndex(2);
}
void mirror::on_btn_help_clicked(void)
{
	QString message = QStringLiteral("��ϸ������鿴�ļ����ڵġ�����ָ����");
	QMessageBox::information(this, QStringLiteral("����"), message);
}
void mirror::on_btn_about_clicked(void)
{
	about *Dlg = new about();
	Dlg->exec();
}

bool mirror::LoadJobSet()
{
	QFile file("./db/jobSet.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	//��Ϊ�������⣬�˴���Ҫ�ر���
	qint32 Len_jobAdd = sizeof(Info_jobAdd) - sizeof(qint32);

	Info_jobAdd job;
	quint32 count;

	quint32 nSkipBytes;
	QDataStream out(file.readAll());
//	while (!out.atEnd())
	{	//ֻ���ر�ְҵ�����ݿ���Ϣ
		out >> count;
		nSkipBytes = count * Len_jobAdd * (roleInfo.vocation-1) + (roleInfo.vocation-1) * sizeof(count);
		if (nSkipBytes > 0)
		{
			out.skipRawData(nSkipBytes);
		}

		while (count--)
		{
			out >> job.level >> job.exp >> job.hp >> job.mp >> job.dc1 >> job.dc2 >> job.mc1 >> job.mc2
				>> job.sc1 >> job.sc2 >> job.ac1 >> job.ac2 >> job.mac1 >> job.mac2;
	
			g_JobAddSet.append(job);
		}
	}

	file.close();
	return true;
}
bool mirror::LoadSkill()
{
	QFile file("./db/skill.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	Info_skill skill;
	QImage img;
	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> skill.ID >> skill.name >> img >> skill.level >> skill.spell[0] >> skill.spell[1] >> skill.spell[2] >> skill.cd;
		out >> skill.times >> skill.damage[0] >> skill.damage[1] >> skill.damage[2] >> skill.buff >> skill.descr;

		skill.icon = QPixmap::fromImage(img);
		g_skillList.append(skill);
	}

	file.close();
	return true;
}
bool mirror::LoadBuff()
{
	QFile file("./db/buff.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	info_buff buff;
	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> buff.ID >> buff.name >> buff.time >> buff.rhp >> buff.ac >> buff.mac;

		g_buffList.append(buff);
	}

	file.close();
	return true;
}
bool mirror::LoadItemList()
{
	QFile file("./db/item_item.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	Info_Item item;
	quint32 type;
	QImage img;

	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> item.ID >> item.name >> img >> item.vocation >> item.level >> item.sale >> item.coin >> item.gold;
		out >> type >> item.value >> item.descr;
		
		item.icon = QPixmap::fromImage(img);
		item.type = static_cast<EffectType>(type);

		g_ItemList.append(item);
	}

	file.close();
	return true;
}
bool mirror::LoadEquipList()
{
	QFile file("./db/item_equip.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	Info_basic_equip equip;
	QImage img;
	QDataStream out(file.readAll());
	while (!out.atEnd())
	{		
		out >> equip.ID >> equip.name >> img >> equip.lv >> equip.luck >> equip.acc >> equip.ag >> equip.spd >> equip.md;
		out >> equip.ac1 >> equip.ac2 >> equip.mac1 >> equip.mac2 >> equip.dc1 >> equip.dc2 >> equip.mc1 >> equip.mc2 >> equip.sc1 >> equip.sc2;
		out >> equip.need >> equip.needLvl >> equip.price;

		equip.icon = QPixmap::fromImage(img);
		g_EquipList.append(equip);
	}

	file.close();
	return true;
}

bool mirror::LoadStateEquip()
{
	QFile file("./db/StateEquip.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	itemID id;
	QImage img;
	Info_StateEquip equip;
	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> id >> img >> equip.offset_x >> equip.offset_y;

		equip.img = QPixmap::fromImage(img);
		g_StateEquip.insert(id, equip);
	}

	file.close();
	return true;
}

void mirror::GiveSomeItem()
{
	QUuid uuid;
	Info_Equip equip = { 0 };
	for (quint32 i = 0; i < 13; i++)
	{
		equip.ID = 304001 + i;
		m_bag_equip.append(equip);
	}

	m_bag_item[201001] = 10;
	m_bag_item[201004] = 10;
	m_bag_item[201011] = 10;
	m_bag_item[201020] = 10;
	m_bag_item[202001] = 10;
	m_bag_item[202010] = 10;
	m_bag_item[203002] = 10;
	m_bag_item[203018] = 10;
}

bool mirror::LoadDistribute()
{
	QFile file("./db/distribute.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	quint32 id;
	QDataStream out(file.readAll());
	QVector<quint32> vec1, vec2;
	Info_Distribute dis;
	QImage img;
	while (!out.atEnd())
	{
		out >> dis.ID >> dis.name >> img >> dis.need_lv >> dis.expend_rep >> dis.expend_item >> dis.normal >> dis.boss;

		dis.img = QIcon(QPixmap::fromImage(img));
		g_MonsterDistribute.insert(dis.ID, dis);
	}

	file.close();
	return true;
}

bool mirror::LoadMonster()
{
	QFile file("./db/Monster_normal1.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		false;
	}

	MonsterInfo mon;
	quint32 hit;
	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> mon.ID >> mon.name >> mon.Head >> mon.level >> mon.exp >> mon.hp >> mon.mp;
		out >> mon.DC1 >> mon.DC2 >> mon.MC1 >> mon.MC2 >> mon.AC >> mon.MAC >> hit >> mon.interval;
		g_MonsterNormal_List.append(mon);
	}

	file.close();
	return true;
}

bool mirror::LoadBoss()
{
	QFile file("./db/Monster_boss1.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	MonsterInfo mon;
	quint32 hit;
	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> mon.ID >> mon.name >> mon.Head >> mon.level >> mon.exp >> mon.hp >> mon.mp;
		out >> mon.DC1 >> mon.DC2 >> mon.MC1 >> mon.MC2 >> mon.AC >> mon.MAC >> hit >> mon.interval;
		g_MonsterBoss_list.append(mon);		
	}

	file.close();
	return true;
}

bool mirror::LoadDropSet()
{
	QFile file("./db/drop.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	quint32 monsterID, ListSize;
	Rational rRat;
	QList<Rational> listDrop;

	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> monsterID >> ListSize;

		listDrop.clear();
		while (ListSize--)
		{
			out >> rRat.ID >> rRat.den;
			listDrop.append(rRat);
		}
		g_mapDropSet[monsterID] = listDrop;
	}

	file.close();
	return true;
}

bool mirror::LoadTaskSet()
{
	QFile file("./db/task.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	info_task task;
	QDataStream out(file.readAll());
	while (!out.atEnd())
	{
		out >> task.requireItem >> task.requireCount >> task.giveItem >> task.giveCount >> task.msg;
		g_task_main_list.append(task);
	}
	return true;
}

bool mirror::LoadRole()
{
	QFile file(SaveFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	qint32 ver;
	quint32 nTmp, nItemID, nItemCount;
	Info_Equip equip;
	roleSkill skill;
	QDataStream out(file.readAll());
	out >> ver;
	if (ver != SaveFileVer)
	{
		file.close();
		if (ver == 3)
		{
			//�浵ת��
			QString message = QStringLiteral("��⵽��ǰ�浵�ļ��汾���ɣ��Ƿ�ת�������°汾��\n��ע�⣬��ת�������棡���ȱ��ݴ浵Ȼ��YES��");
			if (QMessageBox::Yes == QMessageBox::question(this, QStringLiteral("ת���浵"), message))
			{
				if(!updateSaveFileVersion())
				{
					QString message = QStringLiteral("�浵ת��ʧ�ܡ�");
					QMessageBox::critical(this, QStringLiteral("ת���浵"), message);
				}
				else
				{
					QString message = QStringLiteral("�浵ת���ɹ�,������������Ϸ��");
					QMessageBox::information(this, QStringLiteral("ת���浵"), message);
				}
			}
		}
		else
		{
			//�浵̫�ϣ�����ת��
			QString message = QStringLiteral("��ǰ�浵�ļ�̫���ϣ�ϵͳ�޷�ʶ��");
			QMessageBox::critical(this, QStringLiteral("ת���浵"), message);
		}
		exit(0);
	}

	out >> roleInfo.name >> roleInfo.vocation >> roleInfo.gender;
	out >> roleInfo.coin >> roleInfo.gold >> roleInfo.reputation >> roleInfo.exp >> roleInfo.level;

	out.readRawData((char *)&g_roleAddition, sizeof(roleAddition));

	//����ս���еļ���
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> skill.id >> skill.level;
		roleInfo.skill.append(skill);
	}

	//���ص��߱�����Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> nItemID >> nItemCount;
		m_bag_item.insert(nItemID, nItemCount);
	}

	//���ص��ֿ߲���Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> nItemID >> nItemCount;
		m_storage_item.insert(nItemCount, nItemCount);
	}
	
	//����װ��������Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out.readRawData((char *)&equip, sizeof(Info_Equip));
		m_bag_equip.append(equip);
	}

	//����װ���ֿ���Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out.readRawData((char *)&equip, sizeof(Info_Equip));
		m_storage_equip.append(equip);
	}

	//���ؼ���
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> skill.id >> skill.level;
		m_skill_study.append(skill);
	}

	file.close();
	return true;
}

bool mirror::updateSaveFileVersion()
{
	QFile file(SaveFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	qint32 ver;
	quint32 nTmp, nItemID, nItemCount;
	Info_Equip equip;
	roleSkill skill;
	QDataStream out(file.readAll());
	out >> ver;
	out >> roleInfo.name >> roleInfo.vocation >> roleInfo.gender;
	out >> roleInfo.coin >> roleInfo.gold >> roleInfo.reputation >> roleInfo.exp >> roleInfo.level;

	equip = { 0 };
	g_roleAddition = { 0 };
	out >> g_roleAddition.strength >> g_roleAddition.wisdom >> g_roleAddition.spirit >> g_roleAddition.life >> g_roleAddition.agility >> g_roleAddition.potential;

	//����װ��
	for (qint32 i = 0; i < MaxEquipCountForRole; i++)
	{
		out >> equip.ID;
		g_roleAddition.vecEquip[i] = equip;
	}

	//ս���еļ���
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> skill.id >> skill.level;
		roleInfo.skill.append(skill);
	}

	//���ص��߱�����Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> nItemID >> nItemCount;
		m_bag_item.insert(nItemID, nItemCount);
	}

	//���ص��ֿ߲���Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> nItemID >> nItemCount;
		m_storage_item.insert(nItemCount, nItemCount);
	}

	//����װ��������Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> equip.ID;
		m_bag_equip.append(equip);
	}

	//����װ���ֿ���Ϣ
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> equip.ID;
		m_storage_equip.append(equip);
	}

	//���ؼ���
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		out >> skill.id >> skill.level;
		m_skill_study.append(skill);
	}

	file.close();

	silentSave();
	return true;
}


bool mirror::silentSave()
{
	qint32 nTmp;

	QFile file(SaveFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		return false;
	}

	QDataStream out(&file);
	out << SaveFileVer;

	//���������Ϣ
	out << roleInfo.name << roleInfo.vocation << roleInfo.gender;
	out << roleInfo.coin << roleInfo.gold << roleInfo.reputation << roleInfo.exp << roleInfo.level;

	//���渽����Ϣ�����Ե㡢����װ����������ȵ�
	out.writeRawData((char *)&g_roleAddition, sizeof(roleAddition));

	//��������趨�Ĺһ������б�
	nTmp = roleInfo.skill.size();
	out << nTmp;
	for (VecRoleSkill::const_iterator iter = roleInfo.skill.begin(); iter != roleInfo.skill.end(); iter++)
	{
		out << iter->id << iter->level;
	}

	//������߱�����Ϣ
	nTmp = m_bag_item.size();
	out << nTmp;
	for (MapItem::iterator iter = m_bag_item.begin(); iter != m_bag_item.end(); iter++)
	{
		out << iter.key() << iter.value();
	}

	//������ֿ߲���Ϣ
	nTmp = m_storage_item.size();
	out << nTmp;
	for (MapItem::iterator iter = m_storage_item.begin(); iter != m_storage_item.end(); iter++)
	{
		out << iter.key() << iter.value();
	}

	//����װ��������Ϣ
	nTmp = m_bag_equip.size();
	out << nTmp;
	for (ListEquip::iterator iter = m_bag_equip.begin(); iter != m_bag_equip.end(); iter++)
	{
		out.writeRawData((char *)&*iter, sizeof(Info_Equip));
	}

	//����װ���ֿ���Ϣ
	nTmp = m_storage_equip.size();
	out << nTmp;
	for (ListEquip::iterator iter = m_storage_equip.begin(); iter != m_storage_equip.end(); iter++)
	{
		out.writeRawData((char *)&*iter, sizeof(Info_Equip));
	}

	//���������ѧ��ļ���
	nTmp = m_skill_study.size();
	out << nTmp;
	for (VecRoleSkill::const_iterator iter = m_skill_study.begin(); iter != m_skill_study.end(); iter++)
	{
		out << iter->id << iter->level;
	}

	file.close();
	return true;
}

void mirror::on_mirror_save()
{
	if (silentSave())
	{
		QString message = QStringLiteral("��Ϸ�ѱ��档");
		QMessageBox::information(this, QStringLiteral("������Ϸ"), message);
	}
	else
	{
		QString message = QStringLiteral("�޷����棬�浵�������𻵻򲻴��ڡ�");
		QMessageBox::critical(this, QStringLiteral("������Ϸ"), message);
	}
}

void mirror::enable_bkSound(bool bEnable)
{
	if (bgAudio != nullptr)
	{
		if (bEnable)
		{
			bgAudio->play();
		}
		else
		{
			bgAudio->stop();
		}
	}
}

void mirror::enable_autoSave(bool bEnable)
{
	if (bEnable)
	{
		nSaveTimer = startTimer(600 * 1000);
	}
	else
	{
		killTimer(nSaveTimer);
	}
}

void mirror::timerEvent(QTimerEvent *event)
{
	if (!silentSave())
	{
		QString message = QStringLiteral("�޷����棬�浵�������𻵻򲻴��ڡ�");
		QMessageBox::critical(this, QStringLiteral("������Ϸ"), message);
	}
}