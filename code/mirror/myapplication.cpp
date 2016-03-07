#include "myapplication.h"

#define  TIME_OUT 500

MyApplication::MyApplication(int argc, char **argv)
	:QApplication(argc, argv)
	, w(nullptr), _isRunning(false), _localServer(nullptr)
{
	_serverName = "277060BA-18B7-4923-89C4-F50BAC6ABC91";
	_initLocalConnection();
}

MyApplication::~MyApplication()
{
	delete _localServer;
}

bool MyApplication::isRunning()
{
	return _isRunning;
}

////////////////////////////////////////////////////////////////////////////////
// ˵����
// ͨ��socketͨѶʵ�ֳ���ʵ�����У��������µ�����ʱ�����ú���
////////////////////////////////////////////////////////////////////////////////
void MyApplication::_newLocalConnection() {
	QLocalSocket *socket = _localServer->nextPendingConnection();
	if (socket) {
		socket->waitForReadyRead(2 * TIME_OUT);
		delete socket;

		// ���������磺��ȡ��������
		//����Ҫ

		_activateWindow();
	}
}

////////////////////////////////////////////////////////////////////////////////
// ˵����
// ͨ��socketͨѶʵ�ֳ���ʵ�����У�
// ��ʼ���������ӣ�������Ӳ���server���򴴽��������˳�
////////////////////////////////////////////////////////////////////////////////
void MyApplication::_initLocalConnection() {
	_isRunning = false;

	QLocalSocket socket;
	socket.connectToServer(_serverName);
	if (socket.waitForConnected(TIME_OUT)) {
		fprintf(stderr, "%s already running.\n",
			_serverName.toLocal8Bit().constData());
		_isRunning = true;
		// ���������磺�������������͵������
		return;
	}

	//���Ӳ��Ϸ��������ʹ���һ��
	_newLocalServer();
}

////////////////////////////////////////////////////////////////////////////////
// ˵����
// ����LocalServer
////////////////////////////////////////////////////////////////////////////////
void MyApplication::_newLocalServer() {
	_localServer = new QLocalServer(this);
	connect(_localServer, SIGNAL(newConnection()), this, SLOT(_newLocalConnection()));
	if (!_localServer->listen(_serverName)) {
		// ��ʱ����ʧ�ܣ������ǳ������ʱ,�������̷����µ�,�Ƴ�֮
		if (_localServer->serverError() == QAbstractSocket::AddressInUseError) {
			QLocalServer::removeServer(_serverName); // <-- �ص�
			_localServer->listen(_serverName); // �ٴμ���
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// ˵����
// ����������
////////////////////////////////////////////////////////////////////////////////
void MyApplication::_activateWindow() {
	if (w) {
		w->show();
		w->raise();
		w->activateWindow(); // �����
	}
}