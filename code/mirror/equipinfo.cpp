#include "equipinfo.h"
#include <QTextEdit>

EquipInfo::EquipInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

EquipInfo::~EquipInfo()
{

}

void EquipInfo::updateInfo(QPoint pos, const Info_equip &equip, const RoleInfo &Role)
{
	QString strTmp;
	qint32 lineCount = 0;

	strTmp = QStringLiteral("`<font color = yellow>") + equip.name + QStringLiteral("</font>");
	ui.edit_display->setText(strTmp);
	ui.edit_display->append(QStringLiteral(" ����:1 �־�:99/99"));
	lineCount += 2;

	if (equip.ID < 301000)
	{
		//��������ac1�������ˣ�ac2����׼ȷ
		if (equip.ac1 > 0)
		{
			ui.edit_display->append(QStringLiteral(" ����+") + QString::number(equip.ac1));
			++lineCount;
		}
		if (equip.ac2 > 0)
		{
			ui.edit_display->append(QStringLiteral(" ׼ȷ+") + QString::number(equip.ac2));
			++lineCount;
		}
	}
	else
	{
		if (equip.ac1 > 0 || equip.ac2 > 0)
		{
			strTmp = QStringLiteral(" ����") + QString::number(equip.ac1) + QStringLiteral("-") + QString::number(equip.ac2);
			ui.edit_display->append(strTmp);
			++lineCount;
		}
	}

	if (equip.mac1 > 0 || equip.mac2 > 0)
	{
		strTmp = QStringLiteral(" ħ��") + QString::number(equip.mac1) + QStringLiteral("-") + QString::number(equip.mac2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (equip.dc1 > 0 || equip.dc2 > 0)
	{
		strTmp = QStringLiteral(" ����") + QString::number(equip.dc1) + QStringLiteral("-") + QString::number(equip.dc2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (equip.mc1 > 0 || equip.mc2 > 0)
	{
		strTmp = QStringLiteral(" ħ��") + QString::number(equip.mc1) + QStringLiteral("-") + QString::number(equip.mc2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (equip.sc1 > 0 || equip.sc2 > 0)
	{
		strTmp = QStringLiteral(" ����") + QString::number(equip.sc1) + QStringLiteral("-") + QString::number(equip.sc2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	
	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ����������ϣ�����ʾΪ��ɫ������Ĭ����ɫ��
	bool bSatisfy;
	switch (equip.need)
	{
	case 0: bSatisfy = (Role.level >= equip.needLvl); break;
	case 1: bSatisfy = (Role.dc2 > equip.needLvl); break;
	case 2: bSatisfy = (Role.mc2 > equip.needLvl); break;
	case 3: bSatisfy = (Role.sc2 > equip.needLvl); break;
	default:
		bSatisfy = false;
		break;
	}
	strTmp = "";
	if (!bSatisfy)
	{
		strTmp += QStringLiteral("`<font color = red>");
	}
	strTmp += equip_need_info[equip.need] + QString::number(equip.needLvl);
	if (!bSatisfy)
	{
		strTmp += QStringLiteral("</font>");
	}
	ui.edit_display->append(strTmp);

	ui.edit_display->append(QStringLiteral(" ******************************"));
	ui.edit_display->append(QStringLiteral("`<font color = blue>") + equip.msg + QStringLiteral("</font>"));
	lineCount += 3;
	lineCount++;
	
	this->move(pos);
	this->resize(199, lineCount * 19);
}
