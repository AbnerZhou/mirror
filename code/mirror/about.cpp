#include "about.h"

about::about(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.edit_display->setStyleSheet("QTextEdit{ background:transparent} ");

	resize(QSize(450, 500));
	
	ui.edit_display->append(QStringLiteral("��Ʒ��: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n������: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n��  ��: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n��  ��: ��ţ"));
	ui.edit_display->append(QStringLiteral("\nUI���: Asia"));
//	ui.edit_display->append(QStringLiteral("\n��  ��: ��ţ"));
	ui.edit_display->append(QStringLiteral("\n��  ��: ����"));
	ui.edit_display->append(QStringLiteral("\n�������: ݫ��"));
	ui.edit_display->append(QStringLiteral("\n�������: ݫ�� ��ţ"));
	ui.edit_display->append(QStringLiteral("\n�ؿ����: ݫ�� ��ţ"));
	ui.edit_display->append(QStringLiteral("\n����ָ��: m����m"));
	ui.edit_display->append(QStringLiteral("\n��̳����: m����m"));
	ui.edit_display->append(QStringLiteral("\n��    ��: ��ţ ����"));
	ui.edit_display->append(QStringLiteral("\n�ر��л(���������Ⱥ�): "));
	ui.edit_display->append(QStringLiteral("Daniel \t��ɫ���� \t���� \tOnly once \t�񾭲�������ת"));
	ui.edit_display->append(QStringLiteral("�׷� \tСС��� \tShaKespeare \t����"));
}

about::~about()
{

}
