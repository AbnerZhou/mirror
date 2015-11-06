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

QWidget *g_widget;
QMap<skillID, Info_SkillBasic> g_SkillBasic;		//�����趨-������Ϣ
QMap<qint32, Info_SkillDamage> g_SkillDamage;		//�����趨-�˺��趨
QMap<qint32, Info_SkillBuff> g_SkillBuff;			//�����趨-buff�趨,debuff�趨
QMap<qint32, Info_SkillSummon> g_SkillSummon;		//�����趨-�ٻ����趨
QMap<itemID, Info_Item> g_ItemList;					//��Ϸ�����б�
QMap<itemID, Info_basic_equip> g_EquipList;				//��Ϸװ���б�
QMap<itemID, Info_StateEquip> g_StateEquip;			//��ɫ����װ�����
QMap<mapID, Info_Distribute> g_MonsterDistribute;	//����ֲ��б�
QMap<monsterID, MonsterInfo> g_MonsterInfo;			//������Ϣ��
QVector<info_task> g_task_main_list;				//���������б�
mapDrop	g_mapDropSet;								//���ﱩ���趨
QVector<Info_jobAdd> g_JobAddSet;					//ְҵ�ӳ��趨
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
	QString msgTitle = QStringLiteral("������");

	g_widget = this;
	bFirstMinimum = false;
	player = nullptr;

	if (!LoadVerify())
	{
		QString message = QStringLiteral("������֤��Ϣʧ�ܣ���mirror���桷�Ƿ��Ѿ������У�");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}

	if (!LoadRole() || !LoadJobSet())
	{
		QString message = QStringLiteral("����ְҵ�趨ʧ�ܣ�������������Ϸ��");
		QMessageBox::critical(this, msgTitle, message);
		exit(0);
	}

	if (!LoadSkillBasic() || !LoadSkillDamage()|| !LoadSkillBuff() || !LoadSkillSummon())
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
	if (!LoadMonster() || !LoadDistribute() || !LoadDropSet())
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

	player->bind_skill(&m_skill_study);
	player->bind_bag(&m_bag_item, &m_bag_equip);
	player->bind_storage(&m_storage_item, &m_storage_equip);

#ifdef _DEBUG
	GiveSomeItem();	//_test
#endif

	m_tab_fight = new fight(player);
	ui.stackedWidget_main->addWidget(m_tab_fight);

	m_tab_role = new role(player);
	ui.stackedWidget_main->addWidget(m_tab_role);

	m_tab_city = new city(player);
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
		bgAudio->setVolume(100);
		bgAudioList->addMedia(QUrl::fromLocalFile("./sound/b-2.mp3"));
		bgAudioList->setCurrentIndex(0);
	}

	QIcon mainIcon = QIcon(":/mirror/Resources/mainIcon.png");
	setWindowIcon(mainIcon);
	//����֪ͨ����ͼ��
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(mainIcon);
	trayIcon->setToolTip(player->get_name() + QStringLiteral("  Level:") + QString::number(player->get_lv()));

	nSaveTimer = startTimer(5 * 60 * 1000);		//�Զ��浵
	
	//����֪ͨ����ͼ�����Ӧ�¼���������
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	//ʹ�õ��Բ���������ߡ�
	SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);

	//���ݴ浵
	if (QFile::exists(BackFileName))
	{
		QFile::remove(BackFileName);
	}
	QFile::copy(SaveFileName, BackFileName);

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

	delete player;

	SetThreadExecutionState(ES_CONTINUOUS);
}

void mirror::closeEvent(QCloseEvent *event)
{
// 	QString title = QStringLiteral("�˳�ȷ��");
// 	QString message = QStringLiteral("�Ƿ���Ҫ������Ϸ��");
// 	QMessageBox msgBox(QMessageBox::Question, title, message);
// 	QPushButton *YsBtn = msgBox.addButton(QStringLiteral(" ���沢�˳� "), QMessageBox::AcceptRole);
// 	QPushButton *NoBtn = msgBox.addButton(QStringLiteral(" ֱ���˳� "), QMessageBox::RejectRole);
// 	msgBox.exec();
// 	if (msgBox.clickedButton() == YsBtn)
// 	{
		silentSave();
//	}
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

	QString strTitle = QStringLiteral("mirror����_beta_%1.%2.%3").arg(version_major).arg(version_minor).arg(version_build);
	setWindowTitle(strTitle);

	popMenu = new QMenu();
	action_setting = new QAction(QStringLiteral("����"), this);
	action_limit = new QAction(QStringLiteral("�ɾ�"), this);
	action_about = new QAction(QStringLiteral("����"), this);
	action_help = new QAction(QStringLiteral("����"), this);
	popMenu->addAction(action_setting);
	popMenu->addAction(action_limit);
	popMenu->addSeparator();
	popMenu->addAction(action_about);
	popMenu->addSeparator();
	popMenu->addAction(action_help);
	ui.btn_system->setMenu(popMenu);

	connect(action_setting, SIGNAL(triggered(bool)), this, SLOT(on_action_setting(bool)));
	connect(action_limit, SIGNAL(triggered(bool)), this, SLOT(on_action_limit(bool)));
	connect(action_about, SIGNAL(triggered(bool)), this, SLOT(on_action_about(bool)));
	connect(action_help, SIGNAL(triggered(bool)), this, SLOT(on_action_help(bool)));
}

void mirror::changeEvent(QEvent *e)
{
	if ((e->type() == QEvent::WindowStateChange) && this->isMinimized())
	{
		hide();

		trayIcon->show();

		QString strMsg = player->get_name() + QStringLiteral("  Lv:") + QString::number(player->get_lv());
		trayIcon->setToolTip(strMsg);
		if (!bFirstMinimum)
		{
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
void mirror::on_action_setting(bool checked)
{
	QString message = QStringLiteral("��δ���ţ������ڴ�");
	QMessageBox::information(this, QStringLiteral("����"), message);
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

bool mirror::LoadVerify()
{
	std::string strPath("./db/verify.db");
	hVerify = CreateFileA(strPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hVerify == INVALID_HANDLE_VALUE)
	{
		LogIns.append(LEVEL_ERROR, __FUNCTION__, mirErr_FileOpen);
		return false;
	}
	return true;
}
bool mirror::LoadJobSet()
{
	char MD5[] = "a86ace7a058165a91358bf8245207e7a";
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
	nSkipBytes = count * Len_jobAdd * (player->get_voc() - 1) + (player->get_voc() - 1) * sizeof(count);
	if (nSkipBytes > 0)
	{
		if (nSkipBytes != out.skipRawData(nSkipBytes))
			return false;
	}

	while (count--)
	{
		out >> job.level >> job.exp >> job.hp >> job.mp >> job.dc1 >> job.dc2 >> job.mc1 >> job.mc2
			>> job.sc1 >> job.sc2 >> job.ac >> job.mac;

		g_JobAddSet.append(job);
	}	
	return true;
}
bool mirror::LoadSkillBasic()
{
	char MD5[] = "d06b45fb097735fa90f8970e3c83f942";
	QFile file("./db/skill_basic.db");
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
	
	Info_SkillBasic sb;
	QImage img;

	while (!out.atEnd())
	{
		out >> sb.ID >> sb.name >> img >> sb.level >> sb.spell_basic >> sb.spell_add >> sb.cd;
		out >> sb.type >> sb.no >> sb.descr;

		sb.icon = QPixmap::fromImage(img);
		g_SkillBasic[sb.ID] = sb;
	}
	return true;
}
bool mirror::LoadSkillDamage()
{
	char MD5[] = "73663aa1631094d126f3560359f27128";
	QFile file("./db/skill_damage.db");
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
	Info_SkillDamage sd;

	while (!out.atEnd())
	{
		out >> sd.id >> sd.type >> sd.times >> sd.extra >> sd.basic >> sd.add;
		g_SkillDamage[sd.id] = sd;
	}

	return true;
}
bool mirror::LoadSkillBuff()
{
	char MD5[] = "00bada62a5b688a97bbe1f10be21fd2c";
	QFile file("./db/skill_buff.db");
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
	Info_SkillBuff sb;

	while (!out.atEnd())
	{
		out >> sb.id >> sb.time >> sb.rhp >> sb.damage >> sb.defense >> sb.speed;
		g_SkillBuff[sb.id] = sb;
	}

	return true;
}
bool mirror::LoadSkillSummon()
{
	char MD5[] = "fb9f469be969b77a57670dc058d22d2b";
	QFile file("./db/skill_summon.db");
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
	Info_SkillSummon sm;

	while (!out.atEnd())
	{
		out >> sm.id >> sm.photo >> sm.type >> sm.hp >> sm.damage1 >> sm.damage2 >> sm.defense;
		g_SkillSummon[sm.id] = sm;
	}

	return true;
}
bool mirror::LoadItemList()
{
	char MD5[] = "536ede363a7852e46a624fc9c9ef95f6";
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

		g_ItemList.insert(item.ID, item);
	}

	return true;
}
bool mirror::LoadEquipList()
{
	char MD5[] = "2cdb577ba5afea76838801f436c159c4";

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
	quint32 secrettype;

	while (!out.atEnd())
	{		
		out >> equip.ID >> equip.name >> img >> equip.lv >> equip.luck >> equip.spd >> equip.hp >> equip.mp;
		out >> equip.ac >> equip.mac >> equip.dc1 >> equip.dc2 >> equip.mc1 >> equip.mc2 >> equip.sc1 >> equip.sc2;
		out >> equip.need >> equip.needLvl >> equip.price;

		equip.icon = QPixmap::fromImage(img);
		g_EquipList.insert(equip.ID, equip);
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
// 	Info_Equip equip = { 0 };
// 	QList<itemID> list = { 312009, 311016 };
// 	for (auto iter = list.begin(); iter != list.end(); iter++)
// 	{
// 		equip.ID = *iter;
// 		m_bag_equip.append(equip);
// 	}
}

bool mirror::LoadDistribute()
{
	char MD5[] = "c88b1b5fa381db45e5707cf2e699b58b";
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
	char MD5[] = "05ac6f25ededb61223e5787969d1acf9";

	QFile file("./db/Monster.db");
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
		out >> mon.ID >> mon.name >> mon.Head >> mon.boss >> mon.level >> mon.exp >> mon.hp >> mon.mp;
		out >> mon.DC1 >> mon.DC2 >> mon.MC1 >> mon.MC2 >> mon.AC >> mon.MAC >> hit >> mon.interval;
		g_MonsterInfo.insert(mon.ID, mon);
	}

	return true;
}

bool mirror::LoadDropSet()
{
	char MD5[] = "25249bb71843327c17145e0aa8d9788b";
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
		g_mapDropSet.insert(monsterID, listDrop);
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
	char MD5[] = "84b7266c3e511b9d1d485763994bd87c";
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

	char name[128];
	qint32 ver_file, ver_major, ver_minor, ver_build, level, vocation, gender;
	quint64 coin, gold, reputation, exp;
	quint32 nTmp, nItemID, nItemCount;
	Info_Equip equip;
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

	out.readRawData(name, 128);
	out >> vocation >> gender >> coin >> gold >> reputation >> exp >> level;

	player = new CPlayer(name, static_cast<RoleVoc>(vocation), level, gender, coin, gold, reputation);
	if (player == nullptr)
	{
		return false;
	}
	player->set_exp(exp);

	out.readRawData((char *)player->get_onBodyEquip_point(), sizeof(Info_Equip) * MaxEquipCountForRole);

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
	roleSkill sk2 = { 0, 0, true };
	quint32 SaveVer = ver_major * 1000000 + ver_minor * 1000 + ver_build;
	out >> nTmp;
	for (quint32 i = 0; i < nTmp; i++)
	{
		if (SaveVer < 3006)
		{
			out >> sk2.id >> sk2.level;
		}
		else
		{
			out >> sk2.id >> sk2.level >> sk2.usdIndex;
		}
		
		m_skill_study[sk2.id] = sk2;
	}
	return true;
}

bool mirror::updateSaveFileVersion()
{
	return false;
}

bool mirror::verifyRoleInfo()
{
// 	qint32 nTmp, level;
// 	bool bTest = true;
// 	bTest &= roleInfo.coin == (g_falseRole.coin + 1) << 1;
// 	bTest &= roleInfo.gold == (g_falseRole.gold + 1) << 1;
// 	bTest &= roleInfo.reputation == (g_falseRole.reputation + 1) << 1;
// 	bTest &= roleInfo.exp == (g_falseRole.exp + 1) << 1;
// 	bTest &= roleInfo.level == (g_falseRole.level + 1) << 1;
// 	if (!bTest)
// 	{
// 		return false;
// 	}
// 
// 
// 	quint8 *p = &roleInfo.mark_1;
// 	for (quint8 i = 0; i < markCount; i++)
// 	{
// 		if (*(p + i * 2) != 0)
// 		{
// 			return false;
// 		}
// 	}
	return true;
}
bool mirror::verifyXSpeed(QDateTime time_c)
{
	//�ȶԵ�ǰʱ����ļ�����ȡʱ�䡣����ǰʱ�����(����)�ļ�����޸�ʱ�䣬��˵��������ϵͳ�����١�
	//ע���˷���ֻ��win7������ϵͳ�Ͽ��á���Ϊwin7������ϵͳʹ�ü��٣�Ҳ�����޸�ϵͳʱ�䣬ֻ��hook��ǰ����
	//��ʱ����غ�����
	QString strTmp = "./mirror.tmp";
	QFile tmpFile(strTmp);
	if (!tmpFile.open(QIODevice::WriteOnly))
	{
		return false;
	}
	tmpFile.write(strTmp.toStdString().c_str());
	tmpFile.close();


	QFileInfo fi(strTmp);
	QDateTime time_f = fi.lastModified();

	QString str_C = time_c.toString("yy-MM-dd HH:mm:ss");
	QString str_F = time_f.toString("yy-MM-dd HH:mm:ss");
	
	if (time_c > time_f)
	{
		return false;
	}

	return true;
}

bool mirror::silentSave(const QString SaveFileName)
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
	out.writeRawData(player->get_name(), 128);
	out << player->get_voc() << player->get_gender();
	out << player->get_coin() << player->get_gold() << player->get_rep() << player->get_exp() << player->get_lv();

	//��������װ��
	out.writeRawData((char *)player->get_onBodyEquip_point(), sizeof(Info_Equip) * MaxEquipCountForRole);
	
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
		out << iter->id << iter->level << iter->usdIndex;
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
	QDataStream ds(&file);
	ds.writeRawData(save_cryptograph.data(), save_cryptograph.size());
	ds.writeRawData(save_plain.data(), save_plain.size());
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
	m_skill_study.remove(0);

	role_skill *dlg_skill = new role_skill(this, player->get_voc(), &m_skill_study);
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
