#include "dlg_sign.h"
#include "gamemanager.h"
#include "Player.h"

dlg_sign::dlg_sign(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	trys = 0;
	udpsocket = nullptr;

	ui.btn_retry->setVisible(false);

	sl.append("1.cn.pool.ntp.org");
	sl.append("210.72.145.44");
	sl.append("ntp.sjtu.edu.cn");
	sl.append("s2c.time.edu.cn");
	sl.append("s2d.time.edu.cn");
	sl.append("s2e.time.edu.cn");
	sl.append("s2f.time.edu.cn");
	sl.append("s2g.time.edu.cn");
	sl.append("time.nist.gov");
	sl.append("time.windows.com");

	qint32 keepSign = GameMgrIns.get_keepSign();
	ui.lbl_total->setText(QStringLiteral("����ǩ��������%1").arg(keepSign));

	QString strTmp;
	strTmp = QStringLiteral("1�������ճ����������");
	strTmp += QStringLiteral("\n2�����%1��").arg(keepSign);
	ui.lbl_info->setText(strTmp);

	QString strF("yyyy-MM-dd HH:mm:ss");
	QDateTime dt;
	dt.setTime_t(GameMgrIns.get_preSignTime());
	strTmp = QStringLiteral("�ϴ�ǩ��ʱ��:%1").arg(dt.toString(strF));
	ui.lbl_status->setText(strTmp);
}

dlg_sign::~dlg_sign()
{

}

void dlg_sign::connectsucess()
{
	ui.lbl_status->append(QStringLiteral("�����ӣ����ڻ�ȡ����ʱ��"));

	qint8 LI = 0;
	qint8 VN = 3;
	qint8 MODE = 3;
	qint8 STRATUM = 0;
	qint8 POLL = 4;
	qint8 PREC = -6;
	QDateTime Epoch(QDate(1900, 1, 1));
	qint32 second = quint32(Epoch.secsTo(QDateTime::currentDateTime()));
	qint32 temp = 0;
	QByteArray timeRequest(48, 0);
	timeRequest[0] = (LI << 6) | (VN << 3) | (MODE);
	timeRequest[1] = STRATUM;
	timeRequest[2] = POLL;
	timeRequest[3] = PREC & 0xff;
	timeRequest[5] = 1;
	timeRequest[9] = 1;
	timeRequest[40] = (temp = (second & 0xff000000) >> 24);
	temp = 0;
	timeRequest[41] = (temp = (second & 0x00ff0000) >> 16);
	temp = 0;
	timeRequest[42] = (temp = (second & 0x0000ff00) >> 8);
	temp = 0;
	timeRequest[43] = ((second & 0x000000ff));
	udpsocket->flush();
	udpsocket->write(timeRequest);
	udpsocket->flush();
}

void dlg_sign::readingDataGrams()
{
	killTimer(retryTimer);
	ui.lbl_status->append(QStringLiteral("���ڶ�ȡ����ʱ��..."));
	QByteArray newTime;
	QDateTime Epoch(QDate(1900, 1, 1));
	QDateTime unixStart(QDate(1970, 1, 1));
	do
	{
		newTime.resize(udpsocket->pendingDatagramSize());
		udpsocket->read(newTime.data(), newTime.size());
	} while (udpsocket->hasPendingDatagrams());
	QByteArray TransmitTimeStamp;
	TransmitTimeStamp = newTime.right(8);
	quint32 seconds = TransmitTimeStamp[0];
	quint8 temp = 0;
	for (int j = 1; j <= 3; j++)
	{
		seconds = seconds << 8;
		temp = TransmitTimeStamp[j];
		seconds = seconds + temp;
	}

	QDateTime time;
	time.setTime_t(seconds - Epoch.secsTo(unixStart));
	this->udpsocket->disconnectFromHost();
	this->udpsocket->close();
	Sign(time);
}

void dlg_sign::on_btn_ok_clicked()
{
	ui.btn_ok->setEnabled(false);
	retryTimer = startTimer(600);
	TryConnect();
	return;
}

void dlg_sign::TryConnect()
{
	if (trys >= sl.count())
	{
		ui.lbl_status->append(QStringLiteral("���з��������ѳ���"));
		return;
	}

	delete udpsocket;
	udpsocket = new QUdpSocket(this);
	if (udpsocket == nullptr)
	{
		ui.lbl_status->append(QStringLiteral("��ʼ������ʧ��"));
		return;
	}
	connect(udpsocket, SIGNAL(connected()), this, SLOT(connectsucess()));
	connect(udpsocket, SIGNAL(readyRead()), this, SLOT(readingDataGrams()));
	connect(udpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));

	udpsocket->connectToHost(sl.at(trys), 123);

	ui.lbl_status->append(QStringLiteral("��������ʱ�������:%1").arg(sl.at(trys)));
	trys++;
}

void dlg_sign::socketError()
{
	QString string = udpsocket->errorString();
	ui.lbl_status->append(string);
}

void dlg_sign::Sign(QDateTime curTime)
{
	QDateTime pre;
	pre.setTime_t(GameMgrIns.get_preSignTime());

	//��¼����ǩ��ʱ��
	GameMgrIns.set_SignTime(curTime.toTime_t());

	qint64 nTmp = pre.daysTo(curTime);

	if (nTmp > 1){
		//�ϴ�ǩ���Ѿ���һ����ǰ
		GameMgrIns.reset_keepSign();
		GameMgrIns.reset_DaysTaskCount();
		ui.lbl_total->setText(QStringLiteral("����ǩ��������%1").arg(GameMgrIns.get_keepSign()));
		ui.lbl_status->append(QStringLiteral("ǩ���ɹ�"));
	}
	else if (nTmp == 1)
	{
		//�ϴ�ǩ��������
		PlayerIns.add_coin(GameMgrIns.get_keepSign() * 10000);
		GameMgrIns.add_keepSign();
		GameMgrIns.reset_DaysTaskCount();
		ui.lbl_total->setText(QStringLiteral("����ǩ��������%1").arg(GameMgrIns.get_keepSign()));
		ui.lbl_status->append(QStringLiteral("ǩ���ɹ�"));
	}else if (nTmp == 0) {
		//�ϴ�ǩ���ǽ���
		ui.lbl_status->append(QStringLiteral("������Ѿ�ǩ���ˡ�"));
	} else {
		//�ϴ�ǩ�������죡����
		ui.lbl_status->append(QStringLiteral("������Խ�ˡ�"));
	}
}

void dlg_sign::timerEvent(QTimerEvent *event)
{
	delete this->udpsocket;
	udpsocket = nullptr;

	TryConnect();
}