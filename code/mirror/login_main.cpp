#include "login_main.h"
#include <QFile>

#include <QMessageBox>
#include "login_create.h"
#include "MirrorVersion.h"
#include "mirrorlog.h"

#include "cryptography.h"
#include "MirrorVersion.h"

login_main::login_main(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_roleIndex = 0;
	setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

	if (!LoadVerify())
	{
		QString message = QStringLiteral("������֤��Ϣʧ�ܣ���mirror���桷�Ƿ��Ѿ������У�");
		QMessageBox::critical(this, QStringLiteral("������"), message);
		exit(0);
	}

	ui.btn_start->setEnabled(false);
	ui.btn_delect->setEnabled(false);
	ui.btn_recover->setEnabled(false);
	ui.btn_1_select->setEnabled(false);
	ui.btn_2_select->setEnabled(false);

	roleCount = 0;

	bgAudioList = nullptr;
	bgAudio = nullptr;
	if (QFile::exists("./sound/b-3.mp3"))
	{
		bgAudioList = new QMediaPlaylist;
		bgAudioList->setPlaybackMode(QMediaPlaylist::Loop);
		bgAudio = new QMediaPlayer;
		bgAudio->setPlaylist(bgAudioList);
		bgAudio->setVolume(80);
		bgAudioList->addMedia(QUrl::fromLocalFile("./sound/b-3.mp3"));
		bgAudioList->setCurrentIndex(0);
		bgAudio->play();
	}

	QString strTitle = QStringLiteral("mirror����_beta_%1.%2.%3").arg(version_major).arg(version_minor).arg(version_build);
	this->setWindowTitle(strTitle);
	QIcon mainIcon = QIcon(":/mirror/Resources/mainIcon.png");
	setWindowIcon(mainIcon);

	timer_main = startTimer(100);
}

login_main::~login_main()
{
	if (bgAudio != nullptr)
	{
		bgAudio->stop();		
	}
	delete bgAudio;
	delete bgAudioList;

	CloseHandle(hVerify);
}

void login_main::on_btn_1_select_clicked()
{
	m_roleIndex = 1;
	ui.btn_start->setEnabled(true);

	movie->setPaused(false);
	nChangeMovieTimer = startTimer(1000);
}

void login_main::on_btn_2_select_clicked()
{
	m_roleIndex = 2;
	ui.btn_start->setEnabled(true);
}

void login_main::on_btn_start_clicked()
{
	if (m_roleIndex != 0)
	{
		done(QDialog::Accepted);
	}
}

void login_main::on_btn_create_clicked()
{
	if (roleCount > 0)
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("��ʱ��֧�ֵڶ��浵"));
		return;
	}
	ui.btn_quit->setEnabled(false);
	//����bug.�����رմ�����ɫ���ڣ�����ֱ�ӹرճ��򣬻ᵼ�½���פ�������������Ͻ�X
	login_create *lc = new login_create(this);
	lc->setWindowFlags(Qt::SubWindow);
	lc->move(QPoint(50, 20));
	if (QDialog::Accepted == lc->exec())
	{
		loadAndDisplay_BasicRoleInfo();
	}
	ui.btn_quit->setEnabled(true);

	delete lc;
}

void login_main::on_btn_quit_clicked()
{
	done(QDialog::Rejected);
}

bool login_main::LoadVerify()
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

bool login_main::loadAndDisplay_BasicRoleInfo(void)
{
	QFile file(SaveFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	qint32 ver_file, ver_major, ver_minor, ver_build, nTmp;

	QDataStream out(&file);
	out.skipRawData(2000);
	out >> ver_major >> ver_minor >> ver_build >> ver_file;
	out.readRawData(rolename, 128);
	out >> nTmp >> gender;
	out >> coin >> gold >> reputation >> exp >> level;
	file.close();

	ui.lbl_1_name->setText(rolename);
	ui.lbl_1_level->setText(QString::number(level));
	ui.lbl_1_voc->setText(def_vocation[nTmp]);
	vocation = static_cast<RoleVoc>(nTmp);

	ShowUnSelectMovie();
	movie->start();
	movie->setPaused(true);
	ui.btn_1_select->setEnabled(true);
	++roleCount;
	return true;
}

void login_main::ShowUnSelectMovie()
{
	qint32 headNo = ((vocation - 1) * 2 + gender) * 10 + 1;
	QString path = (":/mirror/Resources/role/") + QString::number(headNo) + ".gif";

	QPoint pos;
	switch (headNo)
	{
	case 11: pos = QPoint(100, 40); break;
	case 21: pos = QPoint(50, 60); break;
	case 31: pos = QPoint(90, 40); break;
	case 41: pos = QPoint(60, 50); break;
	case 51: pos = QPoint(80, 40); break;
	case 61: pos = QPoint(70, 50); break;
	default: pos = QPoint(100, 40); break;
	}
	ui.lbl_1_role->move(pos + QPoint(50, 25));

	movie = new QMovie(path);
	ui.lbl_1_role->setMovie(movie);
	movie->setSpeed(100);
}

void login_main::ShowSelectMovie()
{
	qint32 headNo = ((vocation - 1) * 2 + gender) * 10 + 0;
	QString path = (":/mirror/Resources/role/") + QString::number(headNo) + ".gif";

	QPoint pos;
	switch (headNo)
	{
	case 10: pos = QPoint(100, 39); break;
	case 20: pos = QPoint(50, 60); break;
	case 30: pos = QPoint(90, 40); break;
	case 40: pos = QPoint(60, 50); break;
	case 50: pos = QPoint(80, 40); break;
	case 60: pos = QPoint(70, 50); break;
	default: pos = QPoint(100, 40); break;
	}
	ui.lbl_1_role->move(pos + QPoint(50, 25));

	movie = new QMovie(path);
	ui.lbl_1_role->setMovie(movie);
	movie->setSpeed(50);
	movie->start();
}

void login_main::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == nChangeMovieTimer)
	{
		killTimer(nChangeMovieTimer);
		ShowSelectMovie();
	}
	else if (event->timerId() == timer_main)
	{
		killTimer(timer_main);
		if (QFile::exists(SaveFileName))
		{
			if (!CheckSaveFile())
			{
				exit(0);
			}
			loadAndDisplay_BasicRoleInfo();
			on_btn_1_select_clicked();
		}
	}
}

bool login_main::updateSaveFileVersion()
{
	QFile file(SaveFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	qint32 ver_file, ver_major, ver_minor, ver_build, nTmp;
	char rolename[128] = { '\0' };

	QDataStream out(&file);
	out.skipRawData(2000);
	out >> ver_major >> ver_minor >> ver_build >> ver_file;
	out.readRawData(rolename, 128);
	out >> nTmp >> gender;
	out >> coin >> gold >> reputation >> exp >> level;
	file.close();

	QByteArray save_plain, save_cryptograph;
	QDataStream in(&save_plain, QIODevice::WriteOnly);
	in << version_major << version_minor << version_build << SaveFileVer;
	//������Ϣ
	in.writeRawData(rolename, 128);
	in << nTmp << gender;
	in << coin << gold << reputation << exp << level;

	//����װ����
	Info_Equip onWearEquip[MaxEquipCountForRole] = { 0 };
	in.writeRawData((char *)&onWearEquip, sizeof(onWearEquip));

	//���߱��������ֿ߲⡢װ��������װ���ֿ��Ϊ�ա�
	quint32 bag_item_size = 3;
	in << bag_item_size << 220001 << 1 << 220003 << 1 << 220004 << 1;

	quint32 store_item_size, bag_equip_size, store_equip_size;
	store_item_size = store_equip_size = 0;
	in << store_item_size;

	//�����ڷ��û���װ����
	Info_Equip equip = { 0 };
	QVector<itemID> VecEquip = { 301001, 302001, 303001 };
	in << VecEquip.size();
	for (quint32 i = 0; i < VecEquip.size(); i++)
	{
		equip.ID = VecEquip[i];
		in.writeRawData((char *)&equip, sizeof(Info_Equip));
	}

	in << store_equip_size;

	//��ѧ�����б�
	quint32 skill_study_count = 0;
	in << skill_study_count;

	char *pchar = save_plain.data();

	if (!cryptography::Encrypt(save_cryptograph, save_plain))
	{
		return false;
	}

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

bool login_main::CheckSaveFile()
{	
	QFile file(SaveFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	qint32 ver_file, ver_major, ver_minor, ver_build, SaveVer, ApplicationVer;
	bool res = true;

	QDataStream out(&file);
	out.skipRawData(2000);
	out >> ver_major >> ver_minor >> ver_build >> ver_file;
	file.close();

	SaveVer = ver_major * 1000000 + ver_minor * 1000 + ver_build;
	ApplicationVer = version_major * 1000000 + version_minor * 1000 + version_build;
	if (SaveVer == 0)
	{
		//�浵��
		if (QFile::exists(SaveFileName))
		{
			QFile::remove(SaveFileName);
		}
		QFile::copy(BackFileName, SaveFileName);

		QString message = QStringLiteral("��ǰ�浵�����ѳ����޸���������������Ϸ��");
		QMessageBox::critical(this, QStringLiteral("�浵�޸�"), message);

		res = false;
	}
	else if (SaveVer > ApplicationVer)
	{
		//�洢ʱ����Ϸ�汾���ڵ�ǰ��Ϸ�汾
		QString message = QStringLiteral("��ǰ�浵�ļ���ʽ�޷�ʶ�������Ƿ�����Ϊ��Ϸ�汾���͡�");
		message += QStringLiteral("\n��ǰ��Ϸ�汾��%1, �浵������Ϸ�汾��%2").arg(ApplicationVer).arg(SaveVer);
		QMessageBox::critical(this, QStringLiteral("�浵����ʶ��"), message);

		res = false;
	}
	else if (ver_file != SaveFileVer)
	{
		if (ver_file == 8)
		{
			//�浵ת��
			QString message = QStringLiteral("��⵽��ǰ�浵�ļ��汾���ɣ��Ƿ�ת�������°汾��\n��ע�⣬�˴�ת���������ɫ���м��ܡ����ߡ�װ�����������ȼ�����������ҡ�����ת�����̲����棡���ȱ��ݴ浵Ȼ��YES��");
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
			QString message = QStringLiteral("ϵͳ�޷�ʶ��ǰ�浵(�汾��%1)��").arg(ver_file);
			QMessageBox::critical(this, QStringLiteral("����"), message);
		}
		res = false;
	}

	return res;
}