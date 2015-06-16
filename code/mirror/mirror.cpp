#include "mirror.h"
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include "def_item_equip.h"
#include "def_takInfo.h"
#include "about.h"
#include "mirrorlog.h"

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
	ui.btn_about->setEnabled(false);
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
#ifdef _DEBUG
	LogIns.init(LEVEL_INFO);
#else
	LogIns.init(LEVEL_ERROR);
#endif	
	
	g_widget = this;
	bFirstMinimum = false;

	QString strTitle = QStringLiteral("mirror����_beta_0.2.0");
	
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

//	GiveSomeItem();	//_test
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
	ui.lbl_btnBack->setPixmap(QPixmap(":/ui/Resources/ui/7.png"));
}
void mirror::on_btn_role_clicked(void)
{
	ui.stackedWidget_main->setCurrentIndex(1);
	m_tab_role->updateRoleInfo();
	ui.lbl_btnBack->setPixmap(QPixmap(":/ui/Resources/ui/8.png"));
}
void mirror::on_btn_city_clicked(void)
{
	m_tab_city->hideAllDlg();
	ui.stackedWidget_main->setCurrentIndex(2);
	ui.lbl_btnBack->setPixmap(QPixmap(":/ui/Resources/ui/9.png"));
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
	char MD5[] = "562cfed093c8b982ecf1c17b9f6cfbfc";
	QFile file("./db/jobSet.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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
	char MD5[] = "16ca744b08a01f81c25b9a5fb787ba81";
	QFile file("./db/skill.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	
	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
		return false;
	}

	QDataStream out(documentContent);
	
	Info_skill skill;
	QImage img;

	while (!out.atEnd())
	{
		out >> skill.ID >> skill.name >> img >> skill.level >> skill.spell[0] >> skill.spell[1] >> skill.spell[2] >> skill.cd;
		out >> skill.times >> skill.damage[0] >> skill.damage[1] >> skill.damage[2] >> skill.buff >> skill.descr;

		skill.icon = QPixmap::fromImage(img);
		g_skillList.append(skill);
	}
	return true;
}
bool mirror::LoadBuff()
{
	char MD5[] = "48d11bc188a825d181ea733acf39305e";
	QFile file("./db/buff.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	info_buff buff;
	
	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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
	char MD5[] = "8f121046e9099887ee117548690f44e0";
	QFile file("./db/item_item.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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

		g_ItemList.append(item);
	}

	return true;
}
bool mirror::LoadEquipList()
{
	char MD5[] = "6f1cddebd32c8942dd40e40dc6857fab";

	QFile file("./db/item_equip.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
		return false;
	}

	QDataStream out(documentContent);
	Info_basic_equip equip;
	QImage img;

	while (!out.atEnd())
	{		
		out >> equip.ID >> equip.name >> img >> equip.lv >> equip.luck >> equip.acc >> equip.ag >> equip.spd >> equip.md >> equip.ep >> equip.ed;
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

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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
	for (int i = 301040; i <= 301055; i++)
	{
		equip.ID = i;
		m_bag_equip.append(equip);
	}
// 	for (int i = 302017; i <= 302029; i++)
// 	{
// 		equip.ID = i;
// 		m_bag_equip.append(equip);
// 	}
	for (int i = 303017; i <= 303029; i++)
	{
		equip.ID = i;
		m_bag_equip.append(equip);
	}
	for (int i = 304014; i <= 304035; i++)
	{
		equip.ID = i;
		m_bag_equip.append(equip);
	}
}

bool mirror::LoadDistribute()
{
	char MD5[] = "d5d7b10ba276e026d7e284a1a8bbbc3e";
	QFile file("./db/distribute.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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
	char MD5[] = "bce285d5d2236e924bc8fe87c8b584cd";

	QFile file("./db/Monster_normal1.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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
	char MD5[] = "6c7f649964477b0980215f13eb6470cb";
	QFile file("./db/Monster_boss1.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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
	char MD5[] = "b815261737779781cd7ab33dda8123d4";
	QFile file("./db/drop.db");
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QByteArray documentContent = file.readAll();
	file.close();

	if (!verifyDB_MD5(MD5, documentContent, __FUNCTION__))
	{
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

bool mirror::verifyDB_MD5(const char *MD5, const QByteArray &documentContent, const char *dbName)
{
	QByteArray arr = QCryptographicHash::hash(documentContent, QCryptographicHash::Md5).toHex();
	for (int i = 0; i < 16; i++)
	{
		if (MD5[i] != arr.at(i))
		{
			LogIns.append(LEVEL_ERROR, dbName, mirErr_MD5);
			return false;
		}
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
	file.close();

	out >> ver;
	if (ver != SaveFileVer)
	{
		file.close();
// 		if (ver == 3)
// 		{
// 			//�浵ת��
// 			QString message = QStringLiteral("��⵽��ǰ�浵�ļ��汾���ɣ��Ƿ�ת�������°汾��\n��ע�⣬��ת�������棡���ȱ��ݴ浵Ȼ��YES��");
// 			if (QMessageBox::Yes == QMessageBox::question(this, QStringLiteral("ת���浵"), message))
// 			{
// 				if(!updateSaveFileVersion())
// 				{
// 					QString message = QStringLiteral("�浵ת��ʧ�ܡ�");
// 					QMessageBox::critical(this, QStringLiteral("ת���浵"), message);
// 				}
// 				else
// 				{
// 					QString message = QStringLiteral("�浵ת���ɹ�,������������Ϸ��");
// 					QMessageBox::information(this, QStringLiteral("ת���浵"), message);
// 				}
// 			}
// 		}
// 		else
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

	return true;
}

bool mirror::updateSaveFileVersion()
{
	return false;
}

bool mirror::verifyRoleInfo()
{
	qint32 nTmp;
	nTmp = g_roleAddition.strength + g_roleAddition.wisdom + g_roleAddition.spirit + g_roleAddition.life + g_roleAddition.agility + g_roleAddition.potential;
	if (nTmp != (roleInfo.level - 1) * 5)
	{
		return false;
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
		QMessageBox::information(this, QStringLiteral("�ֶ�����"), message);
	}
	else
	{
		QString message = QStringLiteral("�޷����棬�浵�ļ��޷����ʡ�");
		QMessageBox::critical(this, QStringLiteral("�ֶ�����"), message);
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

void mirror::timerEvent(QTimerEvent *event)
{	
	if (!silentSave())
	{
		QString message = QStringLiteral("�޷����棬�浵�ļ��޷����ʡ�");
		QMessageBox::critical(this, QStringLiteral("�Զ�����"), message);
	}
}