#include "dlg_about.h"

dlg_about::dlg_about(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.edit_display->setStyleSheet("QTextEdit{ background:transparent} ");
	
	ui.edit_display->append(QStringLiteral("��  ��: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n��  ��: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n��  ��: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n��  ��: Asia �׷�"));
	ui.edit_display->append(QStringLiteral("\n��  Ч: ����"));
	ui.edit_display->append(QStringLiteral("\n�������: --"));
	ui.edit_display->append(QStringLiteral("\n�������: --"));
	ui.edit_display->append(QStringLiteral("\n�ؿ����: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n����ָ��: m����m"));
	ui.edit_display->append(QStringLiteral("\n��̳����: m����m"));
	ui.edit_display->append(QStringLiteral("\n��    ��: ��ţ ���� ������ɭ�������"));
	ui.edit_display->append(QStringLiteral("\n�ر��л(���������Ⱥ�): "));
	ui.edit_display->append(QStringLiteral("Daniel \t��ɫ���� \t���� \tOnly once \t�񾭲�������ת"));
	ui.edit_display->append(QStringLiteral("СС��� \tShaKespeare \t���� \tݫ��"));
}

dlg_about::~dlg_about()
{

}
