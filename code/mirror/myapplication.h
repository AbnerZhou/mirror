#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QApplication>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <qwidget.h>

class MyApplication : public QApplication
{
	Q_OBJECT

public:
	MyApplication(int argc, char **argv);
	~MyApplication();

	bool isRunning();
	QWidget *w;                        // MainWindowָ��

private slots:
	void _newLocalConnection();

private:
	// ��ʼ����������
	void _initLocalConnection();
	// ���������
	void _newLocalServer();
	// �����
	void _activateWindow();

private:
	QLocalServer *_localServer;
	QString _serverName;

	bool _isRunning;
	
};

#endif // MYAPPLICATION_H
