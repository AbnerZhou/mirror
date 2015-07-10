#include "mirror.h"
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include "cryptography.h"
#include "def_item_equip.h"
#include "def_takInfo.h"
#include "about.h"
#include "mirrorlog.h"
#include "MirrorVersion.h"
#include "role_skill.h"
#include "task.h"

RoleInfo_False g_falseRole;

QWidget *g_widget;
QVector<Info_skill> g_skillList;					//�����趨
vecBuff g_buffList;									//buff�趨
QMap<itemID, Info_Item> g_ItemList;					//��Ϸ�����б�
QVector<Info_basic_equip> g_EquipList;				//��Ϸװ���б�
QMap<itemID, Info_StateEquip> g_StateEquip;			//��ɫ����װ�����
QMap<mapID, Info_Distribute> g_MonsterDistribute;	//����ֲ��б�
QVector<MonsterInfo> g_MonsterNormal_List;			//��ͨ�����б�
QVector<MonsterInfo> g_MonsterBoss_list;			//BOSS���б�
QVector<info_task> g_task_main_list;				//���������б�
mapDrop	g_mapDropSet;								//���ﱩ���趨
QVector<Info_jobAdd> g_JobAddSet;					//ְҵ�ӳ��趨
roleAddition g_roleAddition;						//��ɫ���Ӳ���
QMap<itemID, info_formula> g_formula;				//װ���ϳɹ�ʽ

mirror::mirror(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initUi();
#ifdef _DEBUG
	LogIns.init(LEVEL_INFO);
#else
	LogIns.init(LEVEL_ERROR);
#endif	
	
	g_widget = this;
	bFirstMinimum = false;

	QString msgTitle = QStringLiteral("������");
	if (!LoadRole() || !LoadJobSet())
	{
		QString message = QStringLiteral("����ְҵ�趨ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}

	if (!LoadSkill() || !LoadBuff())
	{
		QString message = QStringLiteral("���ؼ���ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}
	if (!LoadItemList() || !LoadEquipList() || !LoadStateEquip())
	{
		QString message = QStringLiteral("���ص��߻�װ��ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}
	if (!LoadMonster() || !LoadBoss() || !LoadDistribute() || !LoadDropSet())
	{
		QString message = QStringLiteral("���ع���ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}
	if (!LoadTaskSet())
	{
		QString message = QStringLiteral("��������ϵͳʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}
	if (!LoadFormula())
	{
		QString message = QStringLiteral("���ض���ϵͳʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}

//	GiveSomeItem();	//_test
	m_tab_fight = new fight(&roleInfo, &m_bag_item, &m_bag_equip);
	ui.stackedWidget_main->addWidget(m_tab_fight);

	m_tab_role = new role(&roleInfo, &m_skill_study, &m_bag_item, &m_storage_item, &m_bag_equip, &m_storage_equip);
	ui.stackedWidget_main->addWidget(m_tab_role);

	m_tab_city = new city(&roleInfo, &m_bag_item, &m_bag_equip);
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
	trayIcon->setToolTip(roleInfo.name + QStringLiteral("  Level:") + QString::number((roleInfo.level >> 1) - 1));

	nSaveTimer = startTimer(5 * 60 * 1000);		//�Զ��浵
	
	//����֪ͨ����ͼ�����Ӧ�¼���������
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	QObject::connect(m_tab_role, &role::mirrorSave, this, &mirror::on_mirror_save);
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

void mirror::initUi()
{
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

	QString strTitle = QStringLiteral("mirror����_beta_%1.%2.%3_����01(50������)").arg(version_major).arg(version_minor).arg(version_build);
	setWindowTitle(strTitle);

	popMenu = new QMenu();
	action_limit = new QAction(QStringLiteral("�ɾ�"), this);
	action_about = new QAction(QStringLiteral("����"), this);
	action_help = new QAction(QStringLiteral("����"), this);
	popMenu->addAction(action_limit);
	popMenu->addSeparator();
	popMenu->addAction(action_about);
	popMenu->addSeparator();
	popMenu->addAction(action_help);
	ui.btn_system->setMenu(popMenu);

	connect(action_limit, SIGNAL(triggered(bool)), this, SLOT(on_action_limit(bool)));
	connect(action_about, SIGNAL(triggered(bool)), this, SLOT(on_action_about(bool)));
	connect(action_help, SIGNAL(triggered(bool)), this, SLOT(on_action_help(bool)));
}

void mirror::changeEvent(QEvent *e)
{
	if ((e->type() == QEvent::WindowStateChange) && this->isMinimized())
	{
		hide();

		quint32 level = (roleInfo.level >> 1) - 1;
		trayIcon->show();
		trayIcon->setToolTip(roleInfo.name + QStringLiteral("  Level:") + QString::number(level));
		if (!bFirstMinimum)
		{
			QString strMsg = roleInfo.name + QStringLiteral("  Lv:") + QString::number(level);
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
	m_tab_city->hideAllDlg();
	ui.stackedWidget_main->setCurrentIndex(2);
}
void mirror::on_action_help(bool checked)
{
	QString message = QStringLiteral("��ϸ������鿴�ļ����ڵġ�����ָ����");
	QMessageBox::information(this, QStringLiteral("����"), message);
}
void mirror::on_action_about(bool checked)
{
	about *Dlg = new about();
	Dlg->exec();
}
void mirror::on_action_limit(bool checked)
{
	QString message = QStringLiteral("��δ���ţ������ڴ�");
	QMessageBox::information(this, QStringLiteral("�ɾ�"), message);
}

bool mirror::LoadJobSet()
{
	char MD5[] = "562cfed093c8b982ecf1c17b9f6cfbfc";
	QFile file("./db/jobSet.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);

	//��Ϊ�������⣬�˴���Ҫ�ر���
	qint32 Len_jobAdd = sizeof(Info_jobAdd) - sizeof(qint32);

	Info_jobAdd job;
	quint32 count;

	quint32 nSkipBytes;

	//ֻ���ر�ְҵ�����ݿ���Ϣ
	out >> count;
	nSkipBytes = count * Len_jobAdd * (roleInfo.vocation - 1) + (roleInfo.vocation - 1) * sizeof(count);
	if (nSkipBytes > 0)
	{
		if (nSkipBytes != out.skipRawData(nSkipBytes))
			return false;
	}

	while (count--)
	{
		out >> job.level >> job.exp >> job.hp >> job.mp >> job.dc1 >> job.dc2 >> job.mc1 >> job.mc2
			>> job.sc1 >> job.sc2 >> job.ac1 >> job.ac2 >> job.mac1 >> job.mac2;

		g_JobAddSet.append(job);
	}


	if (g_JobAddSet[g_JobAddSet.size()-1].level != 1350)
	{
		return false;
	}
	else
	{
		return true;
	}	
}
bool mirror::LoadSkill()
{
	char MD5[] = "04dc649af0a1e2caf29cea33df3b9d41";
	QFile file("./db/skill.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	
	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);
	
	Info_skill skill;
	QImage img;

	while (!out.atEnd())
	{
		out >> skill.ID >> skill.name >> img >> skill.type >> skill.level >> skill.spell[0] >> skill.spell[1] >> skill.spell[2] >> skill.cd;
		out >> skill.times >> skill.damage[0] >> skill.damage[1] >> skill.damage[2] >> skill.buff >> skill.descr;

		skill.icon = QPixmap::fromImage(img);
		g_skillList.append(skill);
	}
	return true;
}
bool mirror::LoadBuff()
{
	char MD5[] = "5d45bfb8e7e3b395623a58f34259bdf0";
	QFile file("./db/buff.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	info_buff buff;
	
	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);

	while (!out.atEnd())
	{
		out >> buff.ID >> buff.name >> buff.time >> buff.rhp >> buff.ac >> buff.mac;

		g_buffList.append(buff);
	}

	return true;
}
bool mirror::LoadItemList()
{
	char MD5[] = "945d7379a8a26f7a949cbd0c92c8e202";
	QFile file("./db/item_item.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);

	Info_Item item;
	quint32 type;
	QImage img;

	while (!out.atEnd())
	{
		out >> item.ID >> item.name >> img >> item.vocation >> item.level >> item.sale >> item.coin;
		out >> type >> item.value >> item.descr;
		
		item.icon = QPixmap::fromImage(img);
		item.type = static_cast<EffectType>(type);

		g_ItemList[item.ID] = item;
	}

	return true;
}
bool mirror::LoadEquipList()
{
	char MD5[] = "51b7635970db10c824c3bca1673233c2";

	QFile file("./db/item_equip.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);
	Info_basic_equip equip;
	QImage img;

	while (!out.atEnd())
	{		
		out >> equip.ID >> equip.name >> img >> equip.lv;
		out >> equip.luck >> equip.acc >> equip.sacred >> equip.ag >> equip.spd >> equip.md >> equip.ep >> equip.ed;
		out >> equip.ac1 >> equip.ac2 >> equip.mac1 >> equip.mac2 >> equip.dc1 >> equip.dc2 >> equip.mc1 >> equip.mc2 >> equip.sc1 >> equip.sc2;
		out >> equip.need >> equip.needLvl >> equip.price;

		equip.icon = QPixmap::fromImage(img);
		g_EquipList.append(equip);
	}

	return true;
}

bool mirror::LoadStateEquip()
{
	char MD5[] = "9a459dbe2fe165e943f26cb36c7fca45";
	QFile file("./db/StateEquip.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	itemID id;
	QImage img;
	Info_StateEquip equip;
	
	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}
	
	QDataStream out(documentContent);
	while (!out.atEnd())
	{
		out >> id >> img >> equip.offset_x >> equip.offset_y;

		equip.img = QPixmap::fromImage(img);
		g_StateEquip.insert(id, equip);
	}

	return true;
}

void mirror::GiveSomeItem()
{
	Info_Equip equip = { 0 };
// 	for (int i = 313001; i <= 313005; i++)
// 	{
// 		equip.ID = i;
// 		m_bag_equip.append(equip);
// 	}
// 	for (int i = 302001; i <= 302029; i++)
// 	{
// 		equip.ID = i;
// 		m_bag_equip.append(equip);
// 	}
// 	for (int i = 303001; i <= 303029; i++)
// 	{
// 		equip.ID = i;
// 		m_bag_equip.append(equip);
// 	}
// 	for (int i = 304014; i <= 304035; i++)
// 	{
// 		equip.ID = i;
// 		m_bag_equip.append(equip);
// 	}
}

bool mirror::LoadDistribute()
{
	char MD5[] = "bee1f42f3a532131e2f0b5b93a18f661";
	QFile file("./db/distribute.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);

	quint32 id;
	QVector<quint32> vec1, vec2;
	Info_Distribute dis;
	QImage img;

	while (!out.atEnd())
	{
		out >> dis.ID >> dis.name >> img >> dis.need_lv >> dis.expend_rep >> dis.expend_item >> dis.normal >> dis.boss;

		dis.img = QIcon(QPixmap::fromImage(img));
		g_MonsterDistribute.insert(dis.ID, dis);
	}

	return true;
}

bool mirror::LoadMonster()
{
	char MD5[] = "5c8a6b0de39d0b4176ce2d959f40ebb4";

	QFile file("./db/Monster_normal1.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);

	MonsterInfo mon;
	quint32 hit;

	while (!out.atEnd())
	{
		out >> mon.ID >> mon.name >> mon.Head >> mon.level >> mon.exp >> mon.hp >> mon.mp;
		out >> mon.DC1 >> mon.DC2 >> mon.MC1 >> mon.MC2 >> mon.AC >> mon.MAC >> hit >> mon.interval;
		g_MonsterNormal_List.append(mon);
	}

	return true;
}

bool mirror::LoadBoss()
{
	char MD5[] = "10f25851ae276bcb21642151c9ae27ef";
	QFile file("./db/Monster_boss1.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}
	QDataStream out(documentContent);

	MonsterInfo mon;
	quint32 hit;


	while (!out.atEnd())
	{
		out >> mon.ID >> mon.name >> mon.Head >> mon.level >> mon.exp >> mon.hp >> mon.mp;
		out >> mon.DC1 >> mon.DC2 >> mon.MC1 >> mon.MC2 >> mon.AC >> mon.MAC >> hit >> mon.interval;
		g_MonsterBoss_list.append(mon);		
	}

	return true;
}

bool mirror::LoadDropSet()
{
	char MD5[] = "b5e07363b59a1f559fcb94e1f14c4d50";
	QFile file("./db/drop.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);

	quint32 monsterID, ListSize;
	Rational rRat;
	QList<Rational> listDrop;

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

	return true;
}

bool mirror::LoadTaskSet()
{
	char MD5[] = "9a459dbe2fe165e943f26cb36c7fca45";
	QFile file("./db/task.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

// 	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
// 	{
// 		return false;
// 	}

	QDataStream out(documentContent);
	info_task task;

	while (!out.atEnd())
	{
		out >> task.requireItem >> task.requireCount >> task.giveItem >> task.giveCount >> task.msg;
		g_task_main_list.append(task);
	}
	return true;
}

bool mirror::LoadFormula()
{
	char MD5[] = "ca21603b2c2d396ca7201e2d4bb721e6";
	QFile file("./db/formula.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QByteArray documentContent = file.readAll();
	file.close();

	if (!cryptography::verifyDB_MD5(MD5, documentContent))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(documentContent);

	info_formula f;

	while (!out.atEnd())
	{
		out >> f.ID >> f.npc >> f.m_m >> f.m1_t >> f.m1_c >> f.m2_t >> f.m2_c >> f.m3_t >> f.m3_c >> f.m4_t >> f.m4_c;

		g_formula[f.ID] = f;
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

	qint32 ver_file, ver_major, ver_minor, ver_build;
	quint32 nTmp, nItemID, nItemCount;
	Info_Equip equip;
	roleSkill skill;
	QByteArray md5Arr_s, cryptData, validData;

	cryptData = file.read(2000);
	validData = file.readAll();
	file.close();

	cryptography::Decrypt(md5Arr_s, cryptData);
	if (!cryptography::verifyDB_MD5(md5Arr_s.data(), validData))
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_MD5);
		return false;
	}

	QDataStream out(validData);
	out >> ver_major >> ver_minor >> ver_build >> ver_file;
	out.readRawData(roleInfo.name, 128);

	out >> g_falseRole.vocation >> g_falseRole.gender;
	out >> g_falseRole.coin >> g_falseRole.gold >> g_falseRole.reputation >> g_falseRole.exp >> g_falseRole.level;

	roleInfo.vocation = g_falseRole.vocation;
	roleInfo.gender = g_falseRole.gender;
	roleInfo.coin = (g_falseRole.coin + 1) << 1;
	roleInfo.gold = (g_falseRole.gold + 1) << 1;
	roleInfo.reputation = (g_falseRole.reputation + 1) << 1;
	roleInfo.exp = (g_falseRole.exp + 1) << 1;
	roleInfo.level = (g_falseRole.level + 1) << 1;

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
		m_skill_study[skill.id] = skill.level;
	}

	initMarkByte();
	return true;
}

bool mirror::updateSaveFileVersion()
{
	return false;
}

bool mirror::verifyRoleInfo()
{
	qint32 nTmp, level;
// 	nTmp = g_roleAddition.strength + g_roleAddition.wisdom + g_roleAddition.spirit + g_roleAddition.life + g_roleAddition.agility + g_roleAddition.potential;
// 	level = (roleInfo.level >> 1) - 1;
// 	if (nTmp != (level - 1) * 5)
// 	{
// 		return false;
// 	}

	bool bTest = true;
	bTest &= roleInfo.coin == (g_falseRole.coin + 1) << 1;
	bTest &= roleInfo.gold == (g_falseRole.gold + 1) << 1;
	bTest &= roleInfo.reputation == (g_falseRole.reputation + 1) << 1;
	bTest &= roleInfo.exp == (g_falseRole.exp + 1) << 1;
	bTest &= roleInfo.level == (g_falseRole.level + 1) << 1;
	if (!bTest)
	{
		return false;
	}


	quint8 *p = &roleInfo.mark_1;
	for (quint8 i = 0; i < markCount; i++)
	{
		if (*(p + i * 2) != 0)
		{
			return false;
		}
	}
	return true;
}
bool mirror::verifyXSpeed(QDateTime time_c)
{
	//�ȶԵ�ǰʱ����ļ�����ȡʱ�䡣����ǰʱ�����(����)�ļ�����޸�ʱ�䣬��˵��������ϵͳ�����١�
	//ע���˷���ֻ��win7������ϵͳ�Ͽ��á���Ϊwin7������ϵͳʹ�ü��٣�Ҳ�����޸�ϵͳʱ�䣬ֻ��hook��ǰ����
	//��ʱ����غ�����
	QString tmpFile = "./mirror.tmp";
	QFile file(tmpFile);
	file.open(QIODevice::WriteOnly);
	file.write("mirror");
	file.close();

	QFileInfo fi(tmpFile);
	QDateTime time_f = fi.lastModified();

	if (time_c > time_f)
	{
		return false;
	}

	return true;
}

bool mirror::silentSave()
{
	if (!verifyRoleInfo())
	{
		LogIns.append(LEVEL_FATAL, __FUNCTION__, mirErr_Modify);
		exit(0);
	}
	QDateTime time_c = QDateTime::currentDateTime();
	if (!verifyXSpeed(time_c))
	{
		LogIns.append(LEVEL_FATAL, __FUNCTION__, mirErr_XSpeed);
		exit(0);
	}

	qint32 nTmp;
	quint64 nTmp64Arr[4];
	QByteArray save_plain, save_cryptograph;

	QDataStream out(&save_plain, QIODevice::WriteOnly);
	out << version_major << version_minor << version_build << SaveFileVer;

	//���������Ϣ
	out.writeRawData(roleInfo.name, 128);
	out << roleInfo.vocation << roleInfo.gender;

	nTmp64Arr[0] = (roleInfo.coin >> 1) - 1;
	nTmp64Arr[1] = (roleInfo.gold >> 1) - 1;
	nTmp64Arr[2] = (roleInfo.reputation >> 1) - 1;
	nTmp64Arr[3] = (roleInfo.exp >> 1) - 1;
	nTmp = (roleInfo.level >> 1) - 1;
	out << nTmp64Arr[0] << nTmp64Arr[1] << nTmp64Arr[2] << nTmp64Arr[3] << nTmp;

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
	for (MapRoleSkill::const_iterator iter = m_skill_study.begin(); iter != m_skill_study.end(); iter++)
	{
		out << iter.key() << iter.value();
	}

	if (!cryptography::Encrypt(save_cryptograph, save_plain))
	{
		return false;
	}

	QFile file(SaveFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		return false;
	}
	QDataStream dfs(&file);
	dfs.writeRawData(save_cryptograph.data(), save_cryptograph.size());
	dfs.writeRawData(save_plain.data(), save_plain.size());
	file.close();
	return true;
}

void mirror::on_mirror_save()
{
	if (silentSave())
	{
		QString message = QStringLiteral("��Ϸ�ѱ��档");
		QMessageBox::information(this, QStringLiteral("�ֶ�����"), message);
	}
	else
	{
		QString message = QStringLiteral("�޷����棬�浵�ļ��޷����ʡ�");
		QMessageBox::critical(this, QStringLiteral("�ֶ�����"), message);
	}
}

void mirror::on_btn_skill_clicked()
{
	role_skill *dlg_skill = new role_skill(this, &m_skill_study, &roleInfo.skill);
	dlg_skill->setWindowFlags(Qt::Tool);
	dlg_skill->show();
}

void mirror::on_btn_task_clicked()
{
	task *taskDlg = new task(this);
	taskDlg->exec();
	delete taskDlg;
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

void mirror::timerEvent(QTimerEvent *event)
{	
	if (!silentSave())
	{
		QString message = QStringLiteral("�޷����棬�浵�ļ��޷����ʡ�");
		QMessageBox::critical(this, QStringLiteral("�Զ�����"), message);
	}
}
void mirror::initMarkByte()
{
	quint8 *p = &roleInfo.mark_1;

	for (quint8 i = 0; i < markCount; i++)
	{
		*(p + i * 2) = 0;
	}
}