#include "dlg_detail.h"
#include <QTextEdit>

Dlg_Detail::Dlg_Detail(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	hide();
}

Dlg_Detail::~Dlg_Detail()
{

}

void Dlg_Detail::on_btn_quit_clicked()
{
	hide();
}

void Dlg_Detail::DisplayEquipInfo(QPoint pos, const Info_equip *equip, const RoleInfo *roleInfo)
{
	bool bSatisfy;
	QString strTmp;
	qint32 lineCount = 0;

	//������·���װ��������Ҫ�����ж�һ�½�ɫ�Ը��Ƿ���װ����Ҫ�Ա�����ϡ�
	strTmp = QStringLiteral("`<font color = yellow>");
	//��ȡ�����װ�������
	int Type = (equip->ID % 100000) / 1000;
	if (Type == 2 || Type == 3)
	{
		//�Ա𲻷������ɴ�����
		//�˴��жϵļ�Ҫ���ͣ� genderȡֵ(male:1 female:2) Typeȡֵ(male:2 female:3)���ʼ�1����ȼ���
		if (roleInfo->gender != (Type - 1))
			strTmp = QStringLiteral("`<font color = red>");
	}
	strTmp += equip->name + QStringLiteral("</font>");
	ui.edit_display->setText(strTmp);

	ui.edit_display->append(QStringLiteral(" ����:1 �־�:99/99"));
	lineCount += 2;

	if (equip->luck > 0)
	{
		ui.edit_display->append(QStringLiteral(" ����+") + QString::number(equip->luck));
		++lineCount;
	}
	if (equip->acc > 0)
	{
		ui.edit_display->append(QStringLiteral(" ׼ȷ+") + QString::number(equip->acc));
		++lineCount;
	}

	if (equip->ac1 > 0 || equip->ac2 > 0)
	{
		strTmp = QStringLiteral(" ����") + QString::number(equip->ac1) + QStringLiteral("-") + QString::number(equip->ac2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}

	if (equip->mac1 > 0 || equip->mac2 > 0)
	{
		strTmp = QStringLiteral(" ħ��") + QString::number(equip->mac1) + QStringLiteral("-") + QString::number(equip->mac2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (equip->dc1 > 0 || equip->dc2 > 0)
	{
		strTmp = QStringLiteral(" ����") + QString::number(equip->dc1) + QStringLiteral("-") + QString::number(equip->dc2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (equip->mc1 > 0 || equip->mc2 > 0)
	{
		strTmp = QStringLiteral(" ħ��") + QString::number(equip->mc1) + QStringLiteral("-") + QString::number(equip->mc2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (equip->sc1 > 0 || equip->sc2 > 0)
	{
		strTmp = QStringLiteral(" ����") + QString::number(equip->sc1) + QStringLiteral("-") + QString::number(equip->sc2);
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	
	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ����������ϣ�����ʾΪ��ɫ������Ĭ����ɫ��
	switch (equip->need)
	{
	case 0: bSatisfy = (roleInfo->level >= equip->needLvl); break;
	case 1: bSatisfy = (roleInfo->dc2 > equip->needLvl); break;
	case 2: bSatisfy = (roleInfo->mc2 > equip->needLvl); break;
	case 3: bSatisfy = (roleInfo->sc2 > equip->needLvl); break;
	default:
		bSatisfy = false;
		break;
	}

	strTmp = "";
	if (!bSatisfy)
	{
		strTmp += QStringLiteral("`<font color = red>");
	}
	strTmp += equip_need_info[equip->need] + QString::number(equip->needLvl);
	if (!bSatisfy)
	{
		strTmp += QStringLiteral("</font>");
	}
	ui.edit_display->append(strTmp);

	ui.edit_display->append(QStringLiteral(" ******************************"));
	ui.edit_display->append(QStringLiteral("`<font color = blue>") + equip->msg + QStringLiteral("</font>"));
	lineCount += 3;
	lineCount++;

	this->move(pos);
	this->resize(199, lineCount * 18);
}

void Dlg_Detail::DisplayItemInfo(QPoint pos, const Info_Item *item, quint32 role_voc, quint32 role_lvl)
{
	QString strTmp;
	qint32 lineCount = 7;
	qint32 nTmp;

	strTmp = QStringLiteral("`<font color = yellow>") + item->name + QStringLiteral("</font>");
	ui.edit_display->setText(strTmp);

	nTmp = (item->ID / 1000) % 100;
	QString def_ItemType[21] = { QStringLiteral("��"), QStringLiteral("ҩƷ"), QStringLiteral("����"), QStringLiteral("����") };
	def_ItemType[20] = QStringLiteral("�����鼮");
	ui.edit_display->append(QStringLiteral(" ��������:") + def_ItemType[nTmp]);

	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ����������ϣ�����ʾΪ��ɫ������Ĭ����ɫ��
	bool bSatisfy = (role_lvl >= item->level);
	if (bSatisfy)
		strTmp = QStringLiteral("`<font color = white>�ȼ�:") + QString::number(item->level) + QStringLiteral("</font>");
	else
		strTmp = QStringLiteral("`<font color = red>�ȼ�:") + QString::number(item->level) + QStringLiteral("</font>");
	ui.edit_display->append(strTmp);

	if (item->vocation != 0)
	{
		if (item->vocation == role_voc)
			strTmp = QStringLiteral("`<font color = white>ְҵ:") + def_vocation[item->vocation] +  QStringLiteral("</font>");
		else
			strTmp = QStringLiteral("`<font color = red>ְҵ:") + def_vocation[item->vocation] + QStringLiteral("</font>");

		++lineCount;
		ui.edit_display->append(strTmp);
	}
	ui.edit_display->append(QStringLiteral(" ����:1"));
	ui.edit_display->append(QStringLiteral(" �۸�:") + QString::number(item->coin));
	ui.edit_display->append(" ");//����
	ui.edit_display->append(QStringLiteral("`<font color = blue>") + item->descr + QStringLiteral("</font>"));

	this->move(pos);
	this->resize(199, lineCount * 19);
}
