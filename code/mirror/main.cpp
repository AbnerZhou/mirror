#include "mirror.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include "login_create.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

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
	return a.exec();
}
