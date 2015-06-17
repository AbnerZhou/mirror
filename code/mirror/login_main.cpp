#include "login_main.h"
#include <QFile>

#include <QMessageBox>
#include "login_create.h"


login_main::login_main(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_roleIndex = 0;

	ui.lbl_2_name->setVisible(false);
	ui.lbl_2_level->setVisible(false);
	ui.lbl_2_voc->setVisible(false);
	ui.btn_start->setEnabled(false);
	ui.btn_create->setEnabled(false);
	ui.btn_delect->setEnabled(false);
	ui.btn_recover->setEnabled(false);
	ui.btn_1_select->setEnabled(false);
	ui.btn_2_select->setEnabled(false);

	if (QFile::exists(SaveFileName))
	{
		loadAndDisplay_BasicRoleInfo();
	}
	else
	{
		ui.btn_create->setEnabled(true);
	}	
}

login_main::~login_main()
{

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
	ui.btn_create->setEnabled(false);
	ui.btn_quit->setEnabled(false);
	//����bug.�����رմ�����ɫ���ڣ�����ֱ�ӹرճ��򣬻ᵼ�½���פ�̡�
	login_create *lc = new login_create(this);
	lc->setWindowFlags(Qt::SubWindow);
	lc->move(QPoint(50, 20));
	if (QDialog::Accepted == lc->exec())
	{
		loadAndDisplay_BasicRoleInfo();
	}
	else
	{
		ui.btn_create->setEnabled(true);
	}
	ui.btn_quit->setEnabled(true);

	delete lc;
	lc = nullptr;
}

void login_main::on_btn_quit_clicked()
{
	done(QDialog::Rejected);
}

bool login_main::loadAndDisplay_BasicRoleInfo(void)
{
	QFile file(SaveFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	qint32 ver_file, ver_major, ver_minor, ver_build, nTmpVer1, nTmpVer2;
	quint32 nTmp, nItemID, nItemCount;
	QByteArray md5Arr_s, TmpArr1, TmpArr2;

	TmpArr1 = file.read(2000);
	TmpArr2 = file.readAll();
	file.close();

	QDataStream out(TmpArr2);
	out >> ver_major >> ver_minor >> ver_build >> ver_file;
	nTmpVer1 = ver_major * 1000000 + ver_minor * 1000 + ver_build;
	nTmpVer2 = version_major * 1000000 + version_minor * 1000 + version_build;
	if (nTmpVer1 > nTmpVer2)
	{
		//�浵�洢ʱ����Ϸ�汾���ڵ�ǰ��Ϸ�汾
		QString message = QStringLiteral("��ǰ�浵�ļ���ʽ�޷�ʶ�������Ƿ�����Ϊ��Ϸ�汾���͡�");
		QMessageBox::critical(this, QStringLiteral("�浵����ʶ��"), message);
		exit(0);
	}
	if (ver_file != SaveFileVer)
	{
// 		if (ver_file == 3)
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

	ui.lbl_1_name->setText(roleInfo.name);
	ui.lbl_1_level->setText(QString::number(roleInfo.level));
	ui.lbl_1_voc->setText(def_vocation[roleInfo.vocation]);
	ShowUnSelectMovie();
	movie->start();
	movie->setPaused(true);
	ui.btn_1_select->setEnabled(true);
	return true;
}

void login_main::ShowUnSelectMovie()
{
	qint32 headNo = ((roleInfo.vocation - 1) * 2 + roleInfo.gender) * 10 + 1;
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
	qint32 headNo = ((roleInfo.vocation - 1) * 2 + roleInfo.gender) * 10 + 0;
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
	killTimer(nChangeMovieTimer);
	ShowSelectMovie();
}