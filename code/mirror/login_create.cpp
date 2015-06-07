#include "login_create.h"
#include <QMessageBox>
#include <QFile>
#include "RoleDefine.h"
#include "def_System_para.h"

login_create::login_create(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.edit_display->setVisible(false);
	ui.lbl_role_photo->setAttribute(Qt::WA_TranslucentBackground, true);

	m_vocation = 1;
	m_gender = 1;
	bCreate = false;

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
}

login_create::~login_create()
{
	if (bgAudio != nullptr)
	{
		bgAudio->stop();
		delete bgAudio;
	}
	if (bgAudioList != nullptr)
	{
		delete bgAudioList;
	}
}

void login_create::changePhoto()
{
	qint32 headNo = ((m_vocation - 1) * 2 + m_gender) * 10;
	QString photo = (":/role/Resources/role/") + QString::number(headNo) + ".png";
	ui.lbl_role_photo->setPixmap(QPixmap(photo));
}
void login_create::on_btn_vocation_1_clicked()
{
	m_vocation = 1;
	changePhoto();
}
void login_create::on_btn_vocation_2_clicked()
{
	m_vocation = 2;
	changePhoto();
}
void login_create::on_btn_vocation_3_clicked()
{
	m_vocation = 3;
	changePhoto();
}
void login_create::on_btn_gender_m_clicked()
{
	m_gender = 1;
	changePhoto();
}
void login_create::on_btn_gender_f_clicked()
{
	m_gender = 2;
	changePhoto();
}
void login_create::on_btn_ok_clicked()
{
	QString strTmp = ui.edit_role_name->text();
	if (strTmp.isEmpty())
	{
		QString title = QStringLiteral("��ʾ");
		QString message = QStringLiteral("�û�������Ϊ�ա�");
		QMessageBox msgBox(QMessageBox::Question, title, message);
		msgBox.addButton(QStringLiteral(" �� "), QMessageBox::AcceptRole);
		msgBox.exec();
	}
	else
	{
		bCreate = CreateRole(strTmp);
		if (bCreate)
		{
			done(QDialog::Accepted);
		}
	}
}

bool login_create::CreateRole(const QString &name)
{
	RoleInfo myRole;
	roleAddition addtion = { 0 };

	myRole.reputation = myRole.exp = 0;

	myRole.vocation = m_vocation;
	myRole.gender = m_gender;
	myRole.level = 1;
	myRole.coin = 20000;
	myRole.gold = 1000;
#ifdef _DEBUG
	myRole.exp = 900000000;
	myRole.coin = 10000000;
#endif

	QFile file(SaveFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		return false;
	}

	QDataStream out(&file);
	out << SaveFileVer;
	//������Ϣ
	out << name << myRole.vocation << myRole.gender;
	out << myRole.coin << myRole.gold << myRole.reputation << myRole.exp << myRole.level;

	//��չ��Ϣ���������Ե㣬����װ����������ȵȵȡ�
	out.writeRawData((char *)&addtion, sizeof(roleAddition));

	//ս���еļ���,Ĭ��ӵ�С����������ܡ�
	quint32 skill_fighting_count = 1;
	out << skill_fighting_count << 220000 << 1;

	//���߱��������ֿ߲⡢װ��������װ���ֿ��Ϊ�ա�
	quint32 bag_item_size, store_item_size, bag_equip_size, store_equip_size;
	bag_item_size = store_item_size = store_equip_size = 0;
	out << bag_item_size << store_item_size;
	
	//�����ڷ��û���װ����
	Info_Equip equip = { 0 };
	QVector<itemID> VecEquip = { 301001, 302001, 303001 };
	out << VecEquip.size();
	for (quint32 i = 0; i < VecEquip.size(); i++)
	{
		equip.ID = VecEquip[i];
		out.writeRawData((char *)&equip, sizeof(Info_Equip));
	}

	out << store_equip_size;

	//��ѧ�����б�Ĭ��ӵ�С�����������
	quint32 skill_study_count = 1;
	out << skill_study_count << 220000 << 1;

	file.close();
	return true;
}