#include "mirror.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <time.h>
#include "login_create.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	  
	qsrand(clock());
	bool bRes = false;

	//�����ǰû�д浵�ļ����򵯳��Ի�������Ҵ�����ɫ��
	if (!QFile::exists(SaveFileName))
	{
		login_create lc;
		if (QDialog::Accepted == lc.exec())
		{
			if (lc.getCreateRes())
			{
				bRes = true;
			}
		}
	}
	else
	{
		bRes = true;
	}

	if (bRes)
	{
		mirror *w = new mirror;
		w->show();
	}
	else
	{
		exit(0);
	}
	return a.exec();
}
